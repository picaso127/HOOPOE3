/*
 * logger.h
 *
 *  Created on: 8 ���� 2019
 *      Author: USER1
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "TLM_management.h"

#define ERROR_LOG_FILENAME "Err"
#define EVENT_LOG_FILENAME "Eve"

#define EPS_LOG_OFFSET 0
#define PAYLOAD_LOG_OFFSET 500
#define TRANSPONDER_LOG_OFFSET 1000
#define RESETS_LOG_OFFSET 1500
#define ADCS_LOG_OFFSET 2000
#define CUF_LOG_OFFSET 2500


#define LOG_STRUCT_ELEMENT_SIZE sizeof(LogStruct)

typedef struct __attribute__((__packed__))	_LogStruct
{
	int log_num;
	int info;
}LogStruct;

typedef enum _log_errors{
	/*
	 * CUF Errors
	 */
	CUF_GENERATE_SSH_FAIL = 165,
	CUF_AUTHENTICATE_FAIL = 166,
	CUF_SAVE_PERMINANTE_FAIL = 167,
	CUF_LOAD_PERMINANTE_FAIL = 168,
	CUF_UPDATE_PERMINANTE_FAIL = 169,
	CUF_EXECUTE_FAIL = 170,
	CUF_INTEGRATED_FAIL  = 171,
	CUF_ENABLE_FAIL = 172,
	CUF_DISABLE_FAIL = 173
} log_errors;

typedef enum _log_errors_OBC{
	LOG_ERR_FRAM_WRITE = 0,
	LOG_ERR_FRAM_READ = 1,
	LOG_ERR_GET_TIME = 2,
	LOG_ERR_SET_TIME = 3,
	LOG_ERR_DELETE_TM = 4,
	LOG_ERR_DELETE_FILES = 5,

	LOG_ERR_RESET_FRAM_MAIN = 30,
	LOG_ERR_FRAM_GLOBAL_PARAM = 31,
	LOG_ERR_SAMAPHORE_GLOBAL_PARAM = 32,
	LOG_ERR_SAVE_HK = 33,
	LOG_ERR_SEMAPHORE_SD = 34,
	LOG_ERR_SEMAPHORE_CMD = 35,
	LOG_ERR_READ_SD_TLM = 36,
} log_errors_OBC;

typedef enum _log_errors_COMM{
	LOG_ERR_FRAM_WRITE_ = 0,
	LOG_ERR_FRAM_READ_ = 1,
	LOG_ERR_GET_TIME_ = 2,
	LOG_ERR_SET_TIME_ = 3,
	LOG_ERR_DELETE_TM_ = 4,
	LOG_ERR_DELETE_FILES_ = 5,

	LOG_ERR_COMM_SET_BIT_RATE = 10,
	LOG_ERR_COMM_IDLE = 11,
	LOG_ERR_COMM_INIT_TRXVU = 12,
	LOG_ERR_COMM_SEMAPHORE_TRANSMITTING = 13,
	LOG_ERR_COMM_DUMP_QUEUE = 14,
	LOG_ERR_COMM_TRANSPONDER_QUEUE = 15,
	LOG_ERR_COMM_BEACON_TASK = 16,
	LOG_ERR_COMM_COUNT_FRAME = 17,
	LOG_ERR_COMM_READ_TRXVU_STATE = 18,
	LOG_ERR_COMM_SEND_FRAME = 19,
	LOG_ERR_COMM_RECIVE_FRAME = 20,
	LOG_ERR_COMM_GET_TLM = 21,

	LOG_ERR_COMM_TRANSPONDER_GET_TIME = 40,
	LOG_ERR_COMM_TRANSPONDER_FRAM_READ = 41,
	LOG_ERR_COMM_TRANSPONDER_FRAM_WRITE = 42,
	LOG_ERR_COMM_FRAM_READ_BITRATE = 43,
	LOG_ERR_COMM_FRAM_READ_BEACON = 44,
	LOG_ERR_COMM_FRAM_RESET_VALUE = 45,
	LOG_ERR_I2C_TRANSPONDER = 46,
	LOG_ERR_I2C_TRANSPONDER_RSSI = 47,

	LOG_ERR_COMM_DELAYED_COMMAND_GET_TIME = 50,
	LOG_ERR_COMM_DELAYED_COMMAND_FRAM_WRITE = 51,
	LOG_ERR_COMM_DELAYED_COMMAND_FRAM_READ = 52,

	LOG_ERR_COMM_DUMP_ENTER_FS = 53,
	LOG_ERR_COMM_DUMP_READ_FS = 54,
} log_errors_COMM;

typedef enum _log_errors_ANTS{
	LOG_ERR_FRAM_WRITE__ = 0,
	LOG_ERR_FRAM_READ__ = 1,
	LOG_ERR_GET_TIME__ = 2,
	LOG_ERR_SET_TIME__ = 3,
	LOG_ERR_DELETE_TM__ = 4,
	LOG_ERR_DELETE_FILES__ = 5,

	LOG_ERR_INIT_ANTS = 20,
	LOG_ERR_ARM_ANTS_A = 21,
	LOG_ERR_ARM_ANTS_B = 22,
	LOG_ERR_DEPLOY_ANTS = 23,
	LOG_ERR_READ_FRAM_ANTS = 24,
	LOG_ERR_WRITE_FRAM_ANTS = 25,
	LOG_ERR_GET_TIME_ANTS = 26,
	LOG_ERR_RESET_FRAM_ANTS = 27,
	LOG_ERR_ANTS_GET_TLM_A = 28,
	LOG_ERR_ANTS_GET_TLM_B = 29,
} log_errors_ANTS;

typedef enum _log_errors_EPS{
	LOG_ERR_FRAM_WRITE___ = 0,
	LOG_ERR_FRAM_READ___ = 1,
	LOG_ERR_GET_TIME___ = 2,
	LOG_ERR_SET_TIME___ = 3,
	LOG_ERR_DELETE_TM___ = 4,
	LOG_ERR_DELETE_FILES___ = 5,

	LOG_ERR_EPS_GRD_WDT = 10,
	LOG_ERR_EPS_FRAM_OVERRIDE_VALUE = 11,
	LOG_ERR_EPS_UPDATE_POWER_LINES = 12,
	LOG_ERR_EPS_WRITE_STATE = 13,
	LOG_ERR_EPS_RESET_FRAM = 14,
	LOG_ERR_EPS_READ_VOLTAGE_TABLE = 15,
	LOG_ERR_EPS_GET_TLM = 16,
	LOG_ERR_EPS_FRAM_ALPHA = 17,
	LOG_ERR_EPS_SP_WAKE = 18,
	LOG_ERR_EPS_SP_SLEEP = 19,
	LOG_ERR_EPS_SP_COLLECT_TEMP = 20,
	LOG_ERR_EPS_VOLTAGE = 21,
} log_errors_EPS;

typedef enum _log_systems{
	SYSTEM_TRXVU,
	SYSTEM_EPS,
	SYSTEM_PAYLOAD,
	SYSTEM_PAYLOAD_AUTO_HANDLING,
	SYSTEM_OBC,
	SYSTEM_ADCS,
	SYSTEM_ANTS,
	SYSTEM_CUF
} log_systems;

typedef enum _log_payload{
	PAYLOAD_TOOK_IMAGE,
	PAYLOAD_TRANSFERRED_IMAGE,
	PAYLOAD_COMPRESSED_IMAGE,
	PAYLOAD_ERASED_IMAGE,
	PAYLOAD_TURNED_GECKO_ON,
	PAYLOAD_TURNED_GECKO_OFF
} log_payload;

typedef enum _log_eps{
	EPS_ENTER_FULL_MODE,
	EPS_ENTER_CRUISE_MODE,
	EPS_ENTER_SAFE_MODE,
	EPS_ENTER_CRITICAL_MODE,
	EPS_SOFT_RESET,
	EPS_HARD_RESET,
} log_eps;

typedef enum _log_adcs{
	LOG_ADCS_SUCCESS,
	LOG_ADCS_FAIL,
	LOG_ADCS_ADCS_INIT_ERR,
	LOG_ADCS_QUEUE_CREATE_ERR,
	LOG_ADCS_SEMAPHORE_CREATE_ERR,
	LOG_ADCS_BOOT_ERROR,
	LOG_ADCS_CHANNEL_OFF,
	LOG_ADCS_CMD_ERR,
	LOG_ADCS_CMD_RECEIVED,
	LOG_ADCS_UPDATED_VECTOR,
	LOG_ADCS_WRONG_SUB_TYPE,
	LOG_ADCS_WRONG_TYPE,
	LOG_ADCS_FS_INIT_ERR,
	LOG_ADCS_FS_WRITE_ERR,
	LOG_ADCS_FS_READ_ERR,
	LOG_ADCS_FRAM_WRITE_ERR,
	LOG_ADCS_FRAM_READ_ERR,
	LOG_ADCS_INPUT_PARAM_ERR,
	LOG_ADCS_NULL_DATA,
	LOG_ADCS_TLM_SAVE_ERR,
	LOG_ADCS_TLM_ERR,
	LOG_ADCS_QUEUE_ERR,
	LOG_ADCS_MALLOC_ERR,
	LOG_ADCS_I2C_READ_ERR,
	LOG_ADCS_I2C_WRITE_ERR,
	LOG_ADCS_GENERIC_I2C_ACK,
	LOG_ADCS_HARD_RESET,
	LOG_ADCS_SOFT_RESET
} log_adcs;

typedef enum _log_transponder{
	TRANSPONDER_ACTIVATION,
	TRANSPONDER_DEACTIVATION,
	TRXVU_SOFT_RESET,
	TRXVU_HARD_ESET,
	ANTS_A_HARD_RESET,
	ANTS_B_HARD_RESET,
	ANTS_DEPLOY,
} log_transponder;

typedef enum _log_cuf
{
	CUF_EXECUTE_unauthenticated,
	CUF_INTEGRATED_unauthenticated,
	CUF_RESET_SUCCESSFULL,
	CUF_RESET_THRESHOLD_MET,
	CUF_REMOVED,
	CUF_EXECUTED,
	CUF_INTEGRATED,
	CUF_DISABLED,
	CUF_ENABLED,
} log_cuf;

/*
 * @brief writes an error in the error log
 * @param error number from enum
 * @param number of system
 * @param description of error
 */
FileSystemResult WriteErrorLog(log_errors log_num, log_systems system, int info);

/*
 * @brief writes a reset in the event log
 * @param number of system from enum
 * @param description of error
 */
FileSystemResult WriteResetLog(log_systems log_num, int info);

/*
 * @brief writes a payload event in the event log
 * @param number of event from enum
 * @param description of error
 */
FileSystemResult WritePayloadLog(log_payload log_num, int info);

/*
 * @brief writes an eps event in the event log
 * @param number of event from enum
 * @param description of error
 */
FileSystemResult WriteEpsLog(log_eps log_num, int info);

/*
 * @brief writes a transponder event in the event log
 * @param number of event from enum
 * @param description of error
 */
FileSystemResult WriteTransponderLog(log_transponder log_num, int info);

/*
 * @brief writes an adcs event in the event log
 * @param number of event from enum
 * @param description of error
 */
FileSystemResult WriteAdcsLog(log_adcs log_num, int info);

/*
 * @brief writes a CUF event in the event log
 * @param number of event from enum
 * @param description of error
 */
FileSystemResult WriteCUFLog(log_cuf log_num, int info);

#endif /* LOGGER_H_ */
