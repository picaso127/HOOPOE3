/*
 *
 * EPS.c
 *
 *  Created on: 23  2018
 *      Author: I7COMPUTER
 */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Timing/Time.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/LED.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <hal/Storage/FRAM.h>

#include <satellite-subsystems/IsisSolarPanelv2.h>

#include <string.h>
#include "../Global/Global.h"
#include "../Global/logger.h"
#include "../Global/GlobalParam.h"
#include "../payload/Drivers/GeckoCameraDriver.h"
#include "../payload/Request Management/AutomaticImageHandler.h"
#include "../EPS.h"

#define CAM_STATE	 0x0F
#define ADCS_STATE	 0xF0

#define CALCAVARAGE3(vbatt_prev) (((vbatt_prev)[0] + (vbatt_prev)[1] + (vbatt_prev)[2])/3)
#define CALCAVARAGE2(vbatt_prev, vbatt) ((vbatt_prev + vbatt) / 2)

#define CHECK_CHANNEL_0(preState, currState) ((unsigned char)preState.fields.channel3V3_1 != currState.fields.output[0])
#define CHECK_CHANNEL_3(preState, currState) ((unsigned char)preState.fields.channel5V_1 != currState.fields.output[3])
#define CHECK_CHANNEL_CHANGE(preState, currState) CHECK_CHANNEL_0(preState, currState) || CHECK_CHANNEL_3(preState, currState)

voltage_t VBatt_previous;
float alpha = EPS_ALPHA_DEFFAULT_VALUE;
gom_eps_channelstates_t switches_states;
EPS_mode_t batteryLastMode;
EPS_enter_mode_t enterMode[NUM_BATTERY_MODE];

#define DEFULT_VALUES_VOL_TABLE	{ {6700, 7000, 7400}, {7500, 7100, 6800}}

static Boolean get_FRAMVoltageTable(voltage_t voltage_table[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2]);

voltage_t round_vol(voltage_t vol)
{
	int rounding_mul2 = EPS_ROUNDING_FACTOR * 2;
	if (vol % rounding_mul2 > EPS_ROUNDING_FACTOR)
	{
		return (voltage_t)((double)(vol) / rounding_mul2) * rounding_mul2 + rounding_mul2;
	}
	else
	{
		return (voltage_t)((double)(vol) / rounding_mul2) * rounding_mul2;
	}
}

Boolean8bit  get_shut_ADCS()
{
	Boolean8bit mode;
	int error = FRAM_read_exte(&mode, SHUT_ADCS_ADDR, 1);
	if (error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_FRAM_OVERRIDE_VALUE, SYSTEM_EPS, error);
	check_int("shut_ADCS, FRAM_read", error);
	return mode;
}
void shut_ADCS(Boolean mode)
{
	int error = FRAM_write_exte((byte*)&mode, SHUT_ADCS_ADDR, 1);
	if (error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_FRAM_OVERRIDE_VALUE, SYSTEM_EPS, error);
	check_int("shut_ADCS, FRAM_write", error);
}

Boolean8bit  get_shut_CAM()
{
	Boolean8bit mode;
	int error = FRAM_read_exte(&mode, SHUT_CAM_ADDR, 1);
	if (error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_FRAM_OVERRIDE_VALUE, SYSTEM_EPS, error);
	check_int("shut_CAM, FRAM_read", error);
	return mode;
}
void shut_CAM(Boolean mode)
{
	int error = FRAM_write_exte((byte*)&mode, SHUT_CAM_ADDR, 1);
	if (error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_FRAM_OVERRIDE_VALUE, SYSTEM_EPS, error);
	check_int("shut_CAM, FRAM_write", error);
}


Boolean update_powerLines(gom_eps_channelstates_t newState)
{
	gom_eps_hk_t eps_tlm;
	int i_error = GomEpsGetHkData_general(0, &eps_tlm);
	check_int("can't get gom_eps_hk_t for vBatt in EPS_Conditioning", i_error);
	if (i_error != 0)
		return FALSE;

	if (CHECK_CHANNEL_CHANGE(newState, eps_tlm))
	{
		i_error = GomEpsSetOutput(0, switches_states);
		if (i_error)
			WriteErrorLog((log_errors)LOG_ERR_EPS_UPDATE_POWER_LINES, SYSTEM_EPS, i_error);
		check_int("can't set channel state in EPS_Conditioning", i_error);

		i_error = FRAM_write_exte(&switches_states.raw, EPS_STATES_ADDR, 1);
		if (i_error)
			WriteErrorLog((log_errors)LOG_ERR_EPS_WRITE_STATE, SYSTEM_EPS, i_error);
		check_int("EPS_Conditioning, FRAM_write", i_error);

		return TRUE;
	}
	else
	{
		return FALSE;

	}
}

Boolean updateCam_powerLines()
{
	if (get_system_state(cam_operational_param) == SWITCH_OFF && getPIOs())
		stopAction();

	return TRUE;
}

EPS_mode_t get_EPS_mode_t()
{
	return batteryLastMode;
}

void init_enterMode()
{
	enterMode[0].fun = EnterCriticalMode;
	enterMode[0].type = critical_mode;
	enterMode[1].fun = EnterSafeMode;
	enterMode[1].type = safe_mode;
	enterMode[2].fun = EnterCruiseMode;
	enterMode[2].type = cruise_mode;
	enterMode[3].fun = EnterFullMode;
	enterMode[3].type = full_mode;
}

void EPS_Init()
{
	unsigned char eps_i2c_address = EPS_I2C_ADDR;
	int error = GomEpsInitialize(&eps_i2c_address, 1);
	/*if(0 != error)
	{
		printf("error in GomEpsInitialize = %d\n",error);
	}*/

	error = IsisSolarPanelv2_initialize(slave0_spi);
	check_int("EPS_Init, IsisSolarPanelv2_initialize", error);
	IsisSolarPanelv2_sleep();

	voltage_t voltage_table[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2] = DEFULT_VALUES_VOL_TABLE;
	get_FRAMVoltageTable(voltage_table);

	gom_eps_hk_t eps_tlm;
	error = GomEpsGetHkData_general(0, &eps_tlm);
	check_int("GomEpsGetHkData_general, EPS_init", error);
	voltage_t current_vbatt = round_vol(eps_tlm.fields.vbatt);

	init_enterMode();

	Boolean changeMode = FALSE;

	for (int i = 0; i < NUM_BATTERY_MODE - 1; i++)
	{
		if (current_vbatt < voltage_table[0][i])
		{
			enterMode[i].fun(&switches_states, &batteryLastMode);
			update_powerLines(switches_states);
			changeMode = TRUE;
		}
	}

	if (!changeMode)
	{
		enterMode[NUM_BATTERY_MODE - 1].fun(&switches_states, &batteryLastMode);
		update_powerLines(switches_states);
	}

	set_Vbatt(eps_tlm.fields.vbatt);
	VBatt_previous = current_vbatt;
}


void reset_FRAM_EPS()
{
	int i_error;

	reset_EPS_voltages();

	//reset EPS_STATES_ADDR
	byte data = 0;
	i_error = FRAM_write_exte(&data, EPS_STATES_ADDR, 1);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_RESET_FRAM, SYSTEM_EPS, i_error);
	check_int("reset_FRAM_EPS, FRAM_write", i_error);
	data = SWITCH_OFF;
	i_error = FRAM_write_exte(&data, SHUT_ADCS_ADDR, 1);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_RESET_FRAM, SYSTEM_EPS, i_error);
	check_int("reset_FRAM_EPS, FRAM_write", i_error);
	i_error = FRAM_write_exte(&data, SHUT_CAM_ADDR, 1);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_RESET_FRAM, SYSTEM_EPS, i_error);
	check_int("reset_FRAM_EPS, FRAM_write", i_error);
	alpha = EPS_ALPHA_DEFFAULT_VALUE;
	i_error = FRAM_write_exte((byte*)&alpha, EPS_ALPHA_ADDR, 4);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_RESET_FRAM, SYSTEM_EPS, i_error);
	check_int("can't FRAM_write_exte(EPS_ALPHA_ADDR), reset_FRAM_EPS", i_error);
}

void reset_EPS_voltages()
{
	int i_error;
	//reset EPS_VOLTAGES_ADDR
	voltage_t voltages[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2] = DEFULT_VALUES_VOL_TABLE;
	voltage_t comm_voltage  = 7250;

	i_error = FRAM_write_exte((byte*)voltages, EPS_VOLTAGES_ADDR, EPS_VOLTAGES_SIZE_RAW);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_RESET_FRAM, SYSTEM_EPS, i_error);
	check_int("reset_FRAM_EPS, FRAM_read", i_error);

	i_error = FRAM_write_exte((byte*)&comm_voltage, BEACON_LOW_BATTERY_STATE_ADDR, 2);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_RESET_FRAM, SYSTEM_EPS, i_error);
	check_int("reset_FRAM_EPS, FRAM_read", i_error);

	i_error = FRAM_write_exte((byte*)&comm_voltage, TRANS_LOW_BATTERY_STATE_ADDR, 2);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_RESET_FRAM, SYSTEM_EPS, i_error);
	check_int("reset_FRAM_EPS, FRAM_read", i_error);
}


void writeState_log(EPS_mode_t mode, voltage_t vol)
{
	switch(mode)
	{
	case full_mode:
		resumeAction();
		WriteEpsLog(EPS_ENTER_FULL_MODE, (int)vol);
		break;
	case cruise_mode:
		stopAction();
		WriteEpsLog(EPS_ENTER_CRUISE_MODE, (int)vol);
		break;
	case safe_mode:
		stopAction();
		WriteEpsLog(EPS_ENTER_SAFE_MODE, (int)vol);
		break;
	case critical_mode:
		stopAction();
		WriteEpsLog(EPS_ENTER_CRITICAL_MODE, (int)vol);
		break;
	}
}

static void battery_downward(voltage_t current_VBatt, voltage_t previuosVBatt, voltage_t voltage_table[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2])
{
	for (int i = 0; i < EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2; i++)
	{
		if (current_VBatt < voltage_table[0][i])
		{
			if (previuosVBatt > voltage_table[0][i])
			{
				batteryLastMode = enterMode[i].type;
				//printf("down,, EPS voltage: %u, previouse voltage: %u\n", current_VBatt, previuosVBatt);
				writeState_log(batteryLastMode, current_VBatt);
			}
		}
	}
}

static void battery_upward(voltage_t current_VBatt, voltage_t previuosVBatt, voltage_t voltage_table[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2])
{
	for (int i = 0; i < EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2; i++)
	{
		if (current_VBatt > voltage_table[1][i])
		{
			if (previuosVBatt < voltage_table[1][i])
			{
				batteryLastMode = enterMode[NUM_BATTERY_MODE - 1 - i].type;
				//printf("up,, EPS voltage: %u, previouse voltage: %u\n", current_VBatt, previuosVBatt);
				writeState_log(batteryLastMode, current_VBatt);
			}
		}
	}
}

static void sanityCheck_EPS(voltage_t current_VBatt, voltage_t voltage_table[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2])
{
	Boolean check = FALSE;
	if (batteryLastMode == full_mode)
	{
		if (voltage_table[0][2] <= current_VBatt)
		{
			check = TRUE;
		}
	}
	else if (batteryLastMode == cruise_mode)
	{
		if (voltage_table[0][1] <= current_VBatt && current_VBatt <= voltage_table[1][0])
		{
			check = TRUE;
		}
	}
	else if (batteryLastMode == safe_mode)
	{
		if (voltage_table[0][0] <= current_VBatt && current_VBatt <= voltage_table[1][1])
		{
			check = TRUE;
		}
	}
	else if (batteryLastMode == critical_mode)
	{
		if (current_VBatt <= voltage_table[1][2])
		{
			check = TRUE;
		}
	}

	if (check == FALSE)
	{
		Boolean changeMode = FALSE;

		for (int i = 0; i < NUM_BATTERY_MODE - 1; i++)
		{
			if (current_VBatt < voltage_table[0][i])
			{
				batteryLastMode = i;
				changeMode = TRUE;
			}
		}

		if (!changeMode)
		{
			batteryLastMode = NUM_BATTERY_MODE - 1;
			update_powerLines(switches_states);
		}
	}
}


Boolean overRide_ADCS(gom_eps_channelstates_t* switches_states)
{
	if(get_shut_ADCS())
	{
		switches_states->raw = 0;
		set_system_state(ADCS_param, SWITCH_OFF);

		return FALSE;
	}

	return TRUE;
}

Boolean overRide_Camera()
{
	if(get_shut_CAM())
	{
		set_system_state(cam_operational_param, SWITCH_OFF);

		return FALSE;
	}

	return TRUE;
}

void EPS_Conditioning()
{
	gom_eps_hk_t eps_tlm;
	int i_error = GomEpsGetHkData_general(0, &eps_tlm);
	check_int("can't get gom_eps_hk_t for vBatt in EPS_Conditioning", i_error);
	if (i_error != 0)
	{
		WriteErrorLog((log_errors)LOG_ERR_EPS_GET_TLM, SYSTEM_EPS, i_error);
		return;
	}
	if (eps_tlm.fields.vbatt > EPS_VOL_LOGIC_MAX + 100 || eps_tlm.fields.vbatt < EPS_VOL_LOGIC_MIN - 100 )
	{
		WriteErrorLog((log_errors)LOG_ERR_EPS_VOLTAGE, SYSTEM_EPS, (int)eps_tlm.fields.vbatt);
		return;
	}
	set_Vbatt(eps_tlm.fields.vbatt);

	i_error = FRAM_read_exte((byte*)&alpha, EPS_ALPHA_ADDR, 4);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_FRAM_ALPHA, SYSTEM_EPS, i_error);
	check_int("can't FRAM_read_exte(EPS_ALPHA_ADDR) for vBatt in EPS_Conditioning", i_error);
	if (!CHECK_EPS_ALPHA_VALUE(alpha))
	{
		alpha = EPS_ALPHA_DEFFAULT_VALUE;
	}

	voltage_t current_VBatt = round_vol(eps_tlm.fields.vbatt);
	voltage_t VBatt_filtered = (voltage_t)((float)current_VBatt * alpha + (1 - alpha) * (float)VBatt_previous);

	voltage_t voltage_table[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2] = DEFULT_VALUES_VOL_TABLE;
	Boolean ret = get_FRAMVoltageTable(voltage_table);
	if (!ret)
		return;

	if (VBatt_filtered < VBatt_previous)
	{
		battery_downward(VBatt_filtered, VBatt_previous, voltage_table);
	}
	else if (VBatt_filtered > VBatt_previous)
	{
		battery_upward(VBatt_filtered, VBatt_previous, voltage_table);
	}

	sanityCheck_EPS(VBatt_filtered, voltage_table);

	enterMode[batteryLastMode].fun(&switches_states, &batteryLastMode);
	set_EPSState((uint8_t)batteryLastMode);
	update_powerLines(switches_states);
	updateCam_powerLines();
	VBatt_previous = VBatt_filtered;
}

//EPS modes
void EnterFullMode(gom_eps_channelstates_t* switches_states, EPS_mode_t* mode)
{
	*mode = full_mode;
	set_system_state(Tx_param, SWITCH_ON);

	if (overRide_ADCS(switches_states))
	{
		switches_states->fields.quadbatSwitch = 0;
		switches_states->fields.quadbatHeater = 0;
		switches_states->fields.channel3V3_1 = 1;
		switches_states->fields.channel3V3_2 = 0;
		switches_states->fields.channel3V3_3 = 0;
		switches_states->fields.channel5V_1 = 1;
		switches_states->fields.channel5V_2 = 0;
		switches_states->fields.channel5V_3 = 0;

		set_system_state(ADCS_param, SWITCH_ON);
	}

	if (overRide_Camera())
		set_system_state(cam_operational_param, SWITCH_ON);
}

void EnterCruiseMode(gom_eps_channelstates_t* switches_states, EPS_mode_t* mode)
{
	*mode = cruise_mode;
	set_system_state(Tx_param, SWITCH_ON);

	if (overRide_ADCS(switches_states))
	{
		switches_states->fields.quadbatSwitch = 0;
		switches_states->fields.quadbatHeater = 0;
		switches_states->fields.channel3V3_1 = 1;
		switches_states->fields.channel3V3_2 = 0;
		switches_states->fields.channel3V3_3 = 0;
		switches_states->fields.channel5V_1 = 1;
		switches_states->fields.channel5V_2 = 0;
		switches_states->fields.channel5V_3 = 0;

		set_system_state(ADCS_param, SWITCH_ON);
	}

	set_system_state(cam_operational_param, SWITCH_OFF);
}

void EnterSafeMode(gom_eps_channelstates_t* switches_states, EPS_mode_t* mode)
{
	*mode = safe_mode;
	set_system_state(Tx_param, SWITCH_OFF);

	if (overRide_ADCS(switches_states))
	{
		switches_states->fields.quadbatSwitch = 0;
		switches_states->fields.quadbatHeater = 0;
		switches_states->fields.channel3V3_1 = 1;
		switches_states->fields.channel3V3_2 = 0;
		switches_states->fields.channel3V3_3 = 0;
		switches_states->fields.channel5V_1 = 1;
		switches_states->fields.channel5V_2 = 0;
		switches_states->fields.channel5V_3 = 0;

		set_system_state(ADCS_param, SWITCH_ON);
	}

	set_system_state(cam_operational_param, SWITCH_OFF);
}

void EnterCriticalMode(gom_eps_channelstates_t* switches_states, EPS_mode_t* mode)
{
	*mode = critical_mode;
	switches_states->fields.quadbatSwitch = 0;
	switches_states->fields.quadbatHeater = 0;
	switches_states->fields.channel3V3_1 = 0;
	switches_states->fields.channel3V3_2 = 0;
	switches_states->fields.channel3V3_3 = 0;
	switches_states->fields.channel5V_1 = 0;
	switches_states->fields.channel5V_2 = 0;
	switches_states->fields.channel5V_3 = 0;

	set_system_state(cam_operational_param, SWITCH_OFF);
	set_system_state(Tx_param, SWITCH_OFF);
	set_system_state(ADCS_param, SWITCH_OFF);
}


static Boolean get_FRAMVoltageTable(voltage_t voltage_table[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2])
{
	int i_error = FRAM_read_exte((byte*)voltage_table, EPS_VOLTAGES_ADDR, EPS_VOLTAGES_SIZE_RAW);
	if (i_error)
		WriteErrorLog((log_errors)LOG_ERR_EPS_READ_VOLTAGE_TABLE, SYSTEM_EPS, i_error);

	if (!check_EPSTableCorrection(voltage_table) || i_error != 0)
	{
		voltage_t temp[2][EPS_VOLTAGE_TABLE_NUM_ELEMENTS / 2] = DEFULT_VALUES_VOL_TABLE;
		memcpy(voltage_table, temp, EPS_VOLTAGES_SIZE_RAW);
		return FALSE;
	}

	return TRUE;
}
//Write gom_eps_k_t
void WriteCurrentTelemetry(gom_eps_hk_t telemetry)
{
	/*printf("vbatt: %d\nvboost: %d, %d, %d\ncurin: %d, %d, %d\ntemp: %d, %d, %d, %d, %d, %d\ncursys: %d\ncursun: %d\nstates: %d\n",
		telemetry.fields.vbatt, telemetry.fields.vboost[0], telemetry.fields.vboost[1], telemetry.fields.vboost[2],
		telemetry.fields.curin[0], telemetry.fields.curin[1], telemetry.fields.curin[2],
		telemetry.fields.temp[0], telemetry.fields.temp[1], telemetry.fields.temp[2], telemetry.fields.temp[3], telemetry.fields.temp[4], telemetry.fields.temp[5],
		telemetry.fields.cursys, telemetry.fields.cursun, states);*/

	printf("Battery Voltage: %d mV\n", telemetry.fields.vbatt);
	printf("Boost Converters: %d, %d, %d mV\n", telemetry.fields.vboost[0], telemetry.fields.vboost[1], telemetry.fields.vboost[2]);
	printf("Currents: %d, %d, %d mA\n", telemetry.fields.curin[0], telemetry.fields.curin[1], telemetry.fields.curin[2]);
	printf("Temperatures: %d, %d, %d, %d, %d, %d C\n", telemetry.fields.temp[0], telemetry.fields.temp[1], telemetry.fields.temp[2], telemetry.fields.temp[3], telemetry.fields.temp[4], telemetry.fields.temp[5]);
	printf("Current out of Battery: %d mA\n", telemetry.fields.cursys);
	printf("Current of Sun Sensor: %d mA\n", telemetry.fields.cursun);
}

void convert_raw_voltage(byte raw[EPS_VOLTAGES_SIZE_RAW], voltage_t voltages[EPS_VOLTAGE_TABLE_NUM_ELEMENTS])
{
	int i;
	int l = 0;
	for (i = 0; i < EPS_VOLTAGE_TABLE_NUM_ELEMENTS; i++)
	{
		voltages[i] = (voltage_t)raw[l];
		l++;
		voltages[i] += (voltage_t)(raw[l] << 8);
		l++;
	}
}

Boolean check_EPSTableCorrection(voltage_t table[2][NUM_BATTERY_MODE - 1])
{
	if (table[0][0] < EPS_VOL_LOGIC_MIN || table[0][0] >= table[1][NUM_BATTERY_MODE - 2])
		return FALSE;
	if (table[1][0] > EPS_VOL_LOGIC_MAX)
		return FALSE;
	for(int i = 1; i < NUM_BATTERY_MODE - 1; i++)
	{
		if (table[0][i] <= table[1][NUM_BATTERY_MODE - 1 - i] ||
				table[0][i] >= table[1][NUM_BATTERY_MODE - 2 - i])
			return FALSE;
	}

	return TRUE;
}
