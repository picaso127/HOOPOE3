/*
 * GlobalParam.c
 *
 *  Created on: Mar 28, 2019
 *      Author: elain
 */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "freertosExtended.h"

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/LED.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <hal/Storage/FRAM.h>

#include <string.h>

#include "Global.h"
#include "logger.h"
#include "GlobalParam.h"

#define TEMP_CALIBRATION		100
#define TRXVU_TEMP_CALIBRATION(ADC) ((temp_t)ADC * -0.07669 + 195.6037)
#define ATTITUDE_CALIBRATION 	10

global_param current_global_param;
xSemaphoreHandle xCGP_semaphore = NULL;

int init_GP()
{
	if (xCGP_semaphore != NULL)
	{
		return E_IS_INITIALIZED;
	}
	//1. Initialize semaphores for system state and global parameters
	vSemaphoreCreateBinary(xCGP_semaphore);
	//2. check that the semaphores created succefully
	if(xCGP_semaphore == NULL)
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
		return 1;
	}
	//3. read the system state before the restart
	current_global_param.state.fields.ADCS = SWITCH_OFF;
	current_global_param.state.fields.Tx = SWITCH_OFF;
	current_global_param.state.fields.cam_operational = SWITCH_OFF;
	current_global_param.state.fields.dump = SWITCH_OFF;
	current_global_param.state.fields.cammera = SWITCH_OFF;
	current_global_param.state.fields.transponder_active = SWITCH_OFF;
	current_global_param.state.fields.mute = SWITCH_OFF;
	current_global_param.state.fields.anttena_deploy = SWITCH_OFF;
	FRAM_read_exte(&current_global_param.state.raw, STATES_ADDR, 1);
	//4. Initialize current_global_param
	current_global_param.Vbatt = 0;
	current_global_param.curBat = 0;
	current_global_param.cur3V3 = 0;
	current_global_param.cur5V = 0;

	current_global_param.tempComm_LO = 0;
	current_global_param.tempComm_PA = 0;
	for (int i = 0; i < 4; i++)
	{
		current_global_param.tempEPS[i] = 0;
	}
	for (int i = 0; i < 2; i++)
	{
		current_global_param.tempBatt[i] = 0;
	}

	current_global_param.RxDoppler = 0;
	current_global_param.RxRSSI = 0;
	current_global_param.TxForw = 0;
	current_global_param.TxRefl = 0;

	for (int i = 0; i < 3; i++)
	{
		current_global_param.Attitude[i] = 0;
	}

	current_global_param.FS_failFlag = 0;
	current_global_param.numOfDelayedCommand = 0;
	current_global_param.EPS_state = 0;
	current_global_param.numOfResets = 0;

	time_unix set_time;
	byte raw_time[TIME_SIZE];
	FRAM_read_exte(raw_time, TIME_ADDR, TIME_SIZE);
	set_time = (time_unix)raw_time[0];
	set_time += (time_unix)(raw_time[1] << 8);
	set_time += (time_unix)(raw_time[2] << 16);
	set_time += (time_unix)(raw_time[3] << 24);
	current_global_param.lastReset = set_time - TIME_RESET_SEC;

	current_global_param.ground_conn = FALSE;

	return 0;
}

Boolean get_system_state(systems_state_parameters param)
{
	int i_error;
	Boolean return_value = SWITCH_ON;
	if (xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		i_error = FRAM_read_exte((byte*)&(current_global_param.state), STATES_ADDR, 1);
		check_int("can't get system state from FRAM", i_error);
		switch (param)
		{
		case mute_param:
			if (current_global_param.state.fields.mute == 1)
				return_value = SWITCH_ON;
			else
				return_value = SWITCH_OFF;
			break;
		case cam_param:
			if (current_global_param.state.fields.cammera == 1)
				return_value = SWITCH_ON;
			else
				return_value = SWITCH_OFF;
			break;
		case anttena_deploy_param:
			if (current_global_param.state.fields.anttena_deploy == 1)
				return_value = SWITCH_ON;
			else
				return_value = SWITCH_OFF;
			break;
		case transponder_active_param:
			if (current_global_param.state.fields.transponder_active == 1)
				return_value = SWITCH_ON;
			else
				return_value = SWITCH_OFF;
			break;
		case dump_param:
			if (current_global_param.state.fields.dump == 1)
				return_value = SWITCH_ON;
			else
				return_value = SWITCH_OFF;
			break;
		case cam_operational_param:
			if (current_global_param.state.fields.cam_operational == 1)
				return_value = SWITCH_ON;
			else
				return_value = SWITCH_OFF;
			break;
		case Tx_param:
			if (current_global_param.state.fields.Tx == 1)
				return_value = SWITCH_ON;
			else
				return_value = SWITCH_OFF;
			break;
		case ADCS_param:
			if (current_global_param.state.fields.ADCS == 1)
				return_value = SWITCH_ON;
			else
				return_value = SWITCH_OFF;
			break;
		}
		portBASE_TYPE lu_check = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCST_semaphore in get_system_state", lu_check);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}

void set_system_state(systems_state_parameters param, Boolean set_state)
{
	int i_error;
	portBASE_TYPE lu_error;
	if (xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		i_error = FRAM_read_exte((byte*)&current_global_param.state, STATES_ADDR, 1);
		check_int("can't read from FRAM in set_system_state", i_error);
		switch (param)
		{
		case mute_param:
			if (set_state == SWITCH_ON)
				current_global_param.state.fields.mute = 1;
			else
				current_global_param.state.fields.mute = 0;
			break;
		case cam_param:
			if (set_state == SWITCH_ON)
				current_global_param.state.fields.cammera= 1;
			else
				current_global_param.state.fields.cammera = 0;
			break;
		case anttena_deploy_param:
			if (set_state == SWITCH_ON)
				current_global_param.state.fields.anttena_deploy = 1;
			else
				current_global_param.state.fields.anttena_deploy = 0;
			break;
		case transponder_active_param:
			if (set_state == SWITCH_ON)
				current_global_param.state.fields.transponder_active = 1;
			else
				current_global_param.state.fields.transponder_active = 0;
			break;
		case dump_param:
			if (set_state == SWITCH_ON)
				current_global_param.state.fields.dump = 1;
			else
				current_global_param.state.fields.dump = 0;
			break;
		case cam_operational_param:
			if (set_state == SWITCH_ON)
				current_global_param.state.fields.cam_operational = 1;
			else
				current_global_param.state.fields.cam_operational = 0;
			break;
		case Tx_param:
			if (set_state == SWITCH_ON)
				current_global_param.state.fields.Tx = 1;
			else
				current_global_param.state.fields.Tx = 0;
			break;
		case ADCS_param:
			if (set_state == SWITCH_ON)
				current_global_param.state.fields.ADCS = 1;
			else
				current_global_param.state.fields.ADCS = 0;
			break;
		}

		FRAM_write_exte(&current_global_param.state.raw, STATES_ADDR, 1);
		check_int("can't read from FRAM in set_system_state", i_error);
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCST_semaphore in set_system_state", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}

systems_state get_systems_state_param()
{
	systems_state return_value;
	if (xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.state;
		portBASE_TYPE lu_check = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCST_semaphore in get_system_state", lu_check);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}

//global params set/get
void get_current_global_param(global_param* param_out)
{
	if (xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		param_out->Vbatt = current_global_param.Vbatt;
		param_out->curBat = current_global_param.curBat;
		param_out->cur3V3 = current_global_param.cur3V3;
		param_out->cur5V = current_global_param.cur5V;

		param_out->tempComm_LO = current_global_param.tempComm_LO;
		param_out->tempComm_PA = current_global_param.tempComm_PA;
		for (int i = 0; i < 4; i++)
		{
			param_out->tempEPS[i] = current_global_param.tempEPS[i];
		}
		for (int i = 0; i < 2; i++)
		{
			param_out->tempBatt[i] = current_global_param.tempBatt[i];
		}

		param_out->RxDoppler = current_global_param.RxDoppler;
		param_out->RxRSSI = current_global_param.RxRSSI;
		param_out->TxForw = current_global_param.TxForw;
		param_out->TxRefl = current_global_param.TxRefl;

		for (int i = 0; i < 3; i++)
		{
			param_out->Attitude[i] = current_global_param.Attitude[i];
		}

		param_out->FS_failFlag = current_global_param.FS_failFlag;
		param_out->numOfDelayedCommand = current_global_param.numOfDelayedCommand;
		param_out->EPS_state = current_global_param.EPS_state;
		param_out->numOfResets = current_global_param.numOfResets;
		param_out->lastReset = current_global_param.lastReset;

		param_out->state = current_global_param.state;
		portBASE_TYPE lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_current_global_param", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
//	CGP-> Vbatt
void set_Vbatt(voltage_t param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.Vbatt = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_Vbatt_previous", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
voltage_t get_Vbatt()
{
	portBASE_TYPE lu_error;
	voltage_t return_value;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.Vbatt;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_Vbatt", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
//	CGP-> current system
current_t get_curBat()
{
	portBASE_TYPE lu_error;
	current_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.curBat;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_curBat(current_t param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.curBat = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> cur3V3
current_t get_cur3V3()
{
	portBASE_TYPE lu_error;
	current_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.cur3V3;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur3V3", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_cur3V3(current_t param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.cur3V3 = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> cur5V
current_t get_cur5V()
{
	portBASE_TYPE lu_error;
	current_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.cur5V;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur5V", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_cur5V(current_t param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.cur3V3 = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> tempComm_LO
temp_t get_tempComm_LO()
{
	portBASE_TYPE lu_error;
	temp_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = TRXVU_TEMP_CALIBRATION(current_global_param.tempComm_LO);
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur5V", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_tempComm_LO(unsigned short param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.tempComm_LO = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> tempComm_PA
temp_t get_tempComm_PA()
{
	portBASE_TYPE lu_error;
	temp_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = TRXVU_TEMP_CALIBRATION(current_global_param.tempComm_PA);
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur5V", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_tempComm_PA(unsigned short param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.tempComm_PA = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> tempEPS
temp_t get_tempEPS(int index)
{
	if (index < 0 || index > 4)
	{
		//error
		return -22222;
	}
	portBASE_TYPE lu_error;
	temp_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = (temp_t)current_global_param.tempEPS[index];
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur5V", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_tempEPS(int index, short param)
{
	if (index < 0 || index > 4)
	{
		//error
		return;
	}
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.tempEPS[index] = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> tempBatt
temp_t get_tempBatt(int index)
{
	if (index < 0 || index > 2)
	{
		//error
		return -22222;
	}
	portBASE_TYPE lu_error;
	temp_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = (temp_t)current_global_param.tempBatt[index];
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur5V", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_tempBatt(int index, short param)
{
	if (index < 0 || index > 2)
	{
		//error
		return;
	}
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.tempBatt[index] = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> RxDoppler
unsigned short get_RxDoppler()
{
	portBASE_TYPE lu_error;
	unsigned short return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.RxDoppler;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_RxDoppler", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_RxDoppler(unsigned short param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.RxDoppler = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in set_RxDoppler", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> RxRSSI
unsigned short get_RxRSSI()
{
	portBASE_TYPE lu_error;
	unsigned short return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.RxRSSI;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_RxRSSI", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_RxRSSI(unsigned short param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.RxRSSI = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in set_RxRSSI", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> TxRefl
unsigned short get_TxRefl()
{
	portBASE_TYPE lu_error;
	unsigned short return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.TxRefl;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_TxRefl", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_TxRefl(unsigned short param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.TxRefl = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in set_TxRefl", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> TxFrow
unsigned short get_TxForw()
{
	portBASE_TYPE lu_error;
	unsigned short return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.TxForw;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_TxForw", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_TxForw(unsigned short param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.TxForw = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in set_TxForw", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> Attitude
short get_Attitude(int index)
{
	if (index < 0 || index > 3)
	{
		//error
		return -22222;
	}
	portBASE_TYPE lu_error;
	temp_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = (temp_t)(current_global_param.Attitude[index] / ATTITUDE_CALIBRATION);
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur5V", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_Attitude(int index, short param)
{
	if (index < 0 || index > 3)
	{
		//error
		return;
	}
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.Attitude[index] = (short)(param * ATTITUDE_CALIBRATION);
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> numOfPics
uint8_t get_FS_failFlag()
{
	portBASE_TYPE lu_error;
	uint8_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.FS_failFlag;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_numOfPics", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_FS_failFlag(uint8_t param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.FS_failFlag = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in set_numOfPics", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> eps_state
uint8_t get_EPSState()
{
	portBASE_TYPE lu_error;
	uint8_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.EPS_state;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_numOfAPRS", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_EPSState(uint8_t param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.EPS_state = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in set_numOfAPRS", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> numOfDelayedCommand
uint8_t get_numOfDelayedCommand()
{
	portBASE_TYPE lu_error;
	uint8_t return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.numOfDelayedCommand;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_numOfDelayedCommand", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
void set_numOfDelayedCommand(uint8_t param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.numOfDelayedCommand = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in set_numOfDelayedCommand", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
// CGP-> numOfResets
void set_numOfResets(unsigned int num)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.numOfResets = num;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in set_numOfResets", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
unsigned int get_numOfResets()
{
	portBASE_TYPE lu_error;
	int num = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		num = current_global_param.numOfResets;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_numOfResets", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return num;
}
// CGP ->lastReset
void set_lastReset(time_unix param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.lastReset = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
time_unix get_lastReset()
{
	portBASE_TYPE lu_error;
	time_unix return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.lastReset;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur5V", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
//CGP ->connection to ground state
void set_ground_conn(Boolean param)
{
	portBASE_TYPE lu_error;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		current_global_param.ground_conn = param;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_curBat", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
}
Boolean get_ground_conn()
{
	portBASE_TYPE lu_error;
	Boolean return_value = 0;
	if(xSemaphoreTake_extended(xCGP_semaphore, MAX_DELAY) == pdTRUE)
	{
		return_value = current_global_param.ground_conn;
		lu_error = xSemaphoreGive_extended(xCGP_semaphore);
		check_portBASE_TYPE("can't return xCGP_semaphore in get_cur5V", lu_error);
	}
	else
	{
		WriteErrorLog(LOG_ERR_SAMAPHORE_GLOBAL_PARAM, SYSTEM_OBC, -1);
	}
	return return_value;
}
