/*
 * logger.h
 *
 *  Created on: 8 ���� 2019
 *      Author: USER1
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "TLM_management.h"

typedef enum{
	/*
	 * --------Payload Errors--------
	 *
	 * BUTCHERING
	 */
	 LOG_ERR_BUTCHER_SUCCSESSFULL = 100,
	 LOG_ERR_BUTCHER_NULL_POINTER_ERR = 101,
	 LOG_ERR_BUTHCER_PARAMATER_VALUE_ERR = 102,
	 LOG_ERR_BUTCHER_UD_ERROR = 103,
	 LOG_ERR_BUTCHER_BAD_WIDTH_ERR = 104,
	/*
	 * DATABASE
	 *
	 * General
	 */
	 LOG_ERR_DataBaseSuccessfull = 110,
	 LOG_ERR_DataBaseNullPointerERR = 111,
	 LOG_ERR_DataBaseNotInSDERR = 112,
	 LOG_ERR_DataBaseTransferERR = 113,
	 LOG_ERR_DataBasealreadyInSDERR = 114,
	 LOG_ERR_DataBaseGradeCalculationERR = 115,
	 LOG_ERR_DataBaseIllegalIdERR = 116,
	 LOG_ERR_DataBaseVectorNotExistERR = 117,
	 LOG_ERR_DataBaseFramERR = 118,
	 LOG_ERR_DataBaseFullERR = 119,
	 LOG_ERR_DataBaseERR = 121,
	 LOG_ERR_DataBaseTimeERR = 122,
	 LOG_ERR_DataBaseShotPicNotLongAgoERR = 123,
	 LOG_ERR_DataBaseAlreadyMarkedERR = 124,
	 LOG_ERR_DataBase_SmallerThanTheCurrentMaxERR = 125,
	 LOG_ERR_DataBase_sizeBeyondFRAMBounderiesERR = 126,
	/*
	 * GECKO_UC_TakeImage
	 */
	 LOG_ERR_GECKO_Take_Successfull = 130,
	 LOG_ERR_GECKO_Take_ERR_TurnOffSensor = 131,
	 LOG_ERR_GECKO_Take_ERR_Set_ADC_Gain = 132,
	 LOG_ERR_GECKO_Take_ERR_Set_PGA_Gain = 133,
	 LOG_ERR_GECKO_Take_ERR_setExposure = 134,
	 LOG_ERR_GECKO_Take_ERR_setFrameAmount = 135,
	 LOG_ERR_GECKO_Take_ERR_setFrameRate = 136,
	 LOG_ERR_GECKO_Take_ERR_turnOnSensor = 137,
	 LOG_ERR_GECKO_Take_sensorTurnOnTimeoutERR = 138,
	 LOG_ERR_GECKO_Take_trainingTimeoutERR = 139,
	 LOG_ERR_GECKO_Take_trainingErrorERR = 141,
	 LOG_ERR_GECKO_Take_ERR_notInitialiseFlash = 142,
	 LOG_ERR_GECKO_Take_ERR_setImageID = 143,
	 LOG_ERR_GECKO_Take_ERR_disableTestPattern = 144,
	 LOG_ERR_GECKO_Take_ERR_startSampling = 145,
	 LOG_ERR_GECKO_Take_samplingTimeoutERR = 146,
	 LOG_ERR_GECKO_Take_ERR_clearSampleFlag = 147,
	 LOG_ERR_GECKO_Take_ERR_turnOfSensor = 148,
	/*
	 * GECKO_UC_ReadImage
	 */
	 LOG_ERR_GECKO_Read_Successfull = 150,
	 LOG_ERR_GECKO_Read_ERR_InitialiseFlash = 151,
	 LOG_ERR_GECKO_Read_ERR_SetImageID = 152,
	 LOG_ERR_GECKO_Read_ERR_StartReadout = 153,
	 LOG_ERR_GECKO_Read_readTimeoutERR = 154,
	 LOG_ERR_GECKO_Read_wordCountMismatchERR = 155,
	 LOG_ERR_GECKO_Read_pageCountMismatchERR = 156,
	 LOG_ERR_GECKO_Read_readDoneFlagNotSetERR = 157,
	 LOG_ERR_GECKO_Read_Error_ClearReadDoneFlagERR = 158,
	/*
	 * GECKO_UC_EraseImage
	 */
	 LOG_ERR_GECKO_Erase_Successfull = 160,
	 LOG_ERR_GECKO_Erase_ERR_SetImageID = 161,
	 LOG_ERR_GECKO_Erase_StartEraseERR = 162,
	 LOG_ERR_GECKO_Erase_TimeoutERR = 163,
	 LOG_ERR_GECKO_Erase_Error_ClearEraseDoneFlag = 164,
}log_errors;

typedef enum{
	SYSTEM_TRXVU,
	SYSTEM_EPS,
	SYSTEM_PAYLOAD,
	SYSTEM_OBC,
	SYSTEM_ADCS,
}log_systems;

typedef enum{
	PAYLOAD_TOOK_IMAGE,
	PAYLOAD_TRANSFERRED_IMAGE,
	PAYLOAD_COMPRESSED_IMAGE,
	PAYLOAD_ERASED_IMAGE,
	PAYLOAD_TURNED_GECKO_ON,
	PAYLOAD_TURNED_GECKO_OFF
}log_payload;

typedef enum{
	EPS_DUMMY
}log_states;

typedef enum{
	TRANSPONDER_DUMMY
}log_activation;

FileSystemResult WriteLog(void *log, char filename[5]);
FileSystemResult WriteErrorLog(log_errors ErrNum);
FileSystemResult WriteResetLog(log_systems SysNum);
FileSystemResult WritePayloadLog(log_payload log_num, int info);
FileSystemResult WriteEpsLog(log_states StateNum);
FileSystemResult WriteTransponderLog(log_activation activity);


#endif /* LOGGER_H_ */