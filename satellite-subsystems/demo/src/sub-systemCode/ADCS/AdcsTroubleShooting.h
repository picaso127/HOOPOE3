#ifndef ADCSTROUBLESHOOTING_H_
#define ADCSTROUBLESHOOTING_H_

#define CHANNEL_OFF_DELAY 50

//the err code for the trouble shooting
typedef enum TroubleErrCode
{
	TRBL_SUCCESS,
	TRBL_FAIL,
	TRBL_ADCS_INIT_ERR,
	TRBL_BOOT_ERROR,
	TRBL_CHANNEL_OFF,
	TRBL_CMD_ERR,
	TRBL_WRONG_SUB_TYPE,
	TRBL_WRONG_TYPE,
	TRBL_NULL_DATA,
	TRBL_FS_INIT_ERR,
	TRBL_FS_WRITE_ERR,
	TRBL_FS_READ_ERR,
	TRBL_FRAM_WRITE_ERR,
	TRBL_FRAM_READ_ERR,
	TRBL_INPUT_PARAM_ERR,
	TRBL_QUEUE_CREATE_ERR,
	TRBL_SEMAPHORE_CREATE_ERR,
	TRBL_TLM_ERR,
	TRBL_QUEUE_EMPTY

}TroubleErrCode;


/*!
 * @solve the ADCS SYSTEM problems by their spaspic err code
 *
 * get:
 * 		problems err codes
 * return:
 * 		log code todo:check what log get?
 */
int AdcsTroubleShooting(TroubleErrCode trbl);

TroubleErrCode FindErr();

#endif /* ADCSTROUBLESHOOTING_H_ */
