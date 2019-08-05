/*
 * splTypes.h
 *
 *  Created on: Dec 5, 2018
 *      Author: Hoopoe3n
 */

#ifndef SPLTYPES_H_
#define SPLTYPES_H_

//Types (TM)
#define BEACON_T 						3
#define APRS 							20
#define ACK_TYPE 						13
#define DUMP_T 							173
#define IMAGE_DUMP_T 					11
#define ACK_ST 							90
#define TM_ADCS_ST 						42
#define ADCS_SC_ST						55

//Types (TC)

#define NONE_APRS_T 					33
#define COMM_T 							13
#define GENERAL_T 						20
#define TC_ADCS_T 						154
#define GENERALLY_SPEAKING_T			195
#define PAYLOAD_T						251
#define EPS_T							6
#define SOFTWARE_T 						42
#define SPECIAL_OPERATIONS_T			69

//SubType (TM)
#define BEACON_ST 						25
#define APRS_PACKET_FRAM 				33
//dumps subTypes:
#define EPS_DUMP_ST 					13
#define CAM_DUMP_ST 					23
#define COMM_DUMP_ST 					45
#define ADCS_DUMP_ST 					78
#define SP_DUMP_ST						91

#define IMAGE_DUMP_THUMBNAIL4_ST		100
#define IMAGE_DUMP_THUMBNAIL3_ST		101
#define IMAGE_DUMP_THUMBNAIL2_ST		102
#define IMAGE_DUMP_THUMBNAIL1_ST		103
#define IMAGE_DUMP_RAW_ST				104
#define IMAGE_DUMP_JPG_ST				105

//ADCS science sub-types
#define ADCS_CSS_DATA_ST 				122
#define ADCS_MAGNETIC_FILED_ST			123
#define ADCS_CSS_SUN_VECTOR_ST			124
#define ADCS_WHEEL_SPEED_ST				125
#define ADCS_SENSORE_RATE_ST			126
#define ADCS_MAG_CMD_ST					127
#define ADCS_WHEEL_CMD_ST				128
#define ADCS_MAG_RAW_ST					129
#define ADCS_IGRF_MODEL_ST				130
#define ADCS_GYRO_BIAS_ST				131
#define ADCS_INNO_VEXTOR_ST				132
#define ADCS_ERROR_VEC_ST				133
#define ADCS_QUATERNION_COVARIANCE_ST 	134
#define ADCS_ANGULAR_RATE_COVARIANCE_ST 135
#define ADCS_ESTIMATED_ANGLES_ST		136
#define ADCS_ESTIMATED_AR_ST			137
#define ADCS_ECI_POS_ST					138
#define ADCS_SAV_VEL_ST					139
#define ADCS_ECEF_POS_ST				140
#define ADCS_LLH_POS_ST					141
#define ADCS_EST_QUATERNION_ST			142

//camm staff
#define IMAGE_DUMP_ST 					122

//Subtypes (TC)
//comm
#define MUTE_ST							0
#define UNMUTE_ST						1
#define ACTIVATE_TRANS_ST				24
#define SHUT_TRANS_ST					25
#define CHANGE_TRANS_RSSI_ST			26
#define APRS_DUMP_ST					90
#define STOP_DUMP_ST					91
#define TIME_FREQUENCY_ST				5

//general
#define SOFT_RESET_ST					68
#define HARD_RESET_ST					66
#define RESET_SAT_ST 					67
#define GRACEFUL_RESET_ST				69
#define UPLOAD_TIME_ST 					17

//generally speaking
#define GENERIC_I2C_ST					0
#define DUMP_ST							33
#define DELETE_PACKETS_ST				35
#define RESET_FILE_ST					45
#define RESTSRT_FS_ST					46
#define DUMMY_FUNC_ST					122
#define REDEPLOY						56
#define ARM_DISARM						57

//payload
#define SEND_PIC_CHUNCK_ST				1
#define UPDATE_STN_PARAM_ST 			2
#define GET_IMG_DATA_BASE_ST			7
#define RESET_DATA_BASE_ST				11
#define DELETE_PIC_ST					42
#define UPD_DEF_DUR_ST					69
#define	OFF_CAM_ST						73
#define ON_CAM_ST						101
#define MOV_IMG_CAM_OBS_ST				123
#define TAKE_IMG_DEF_VAL_ST				213
#define TAKE_IMG_ST						255
#define CREATE_TB_FROM_IMAGE_ST			51

//eps
#define ALLOW_ADCS_ST					99
#define SHUT_ADCS_ST					100
#define ALLOW_CAM_ST					101
#define SHUT_CAM_ST						102
#define UPD_LOGIC_VOLT_ST				255
#define UPD_COMM_VOLTAGE				20
#define CHANGE_HEATER_TMP_ST			0

//adcs
#define ADCS_GENRIC_ST 					11
#define ADCS_SET_BOOT_INDEX_ST			98
#define ADCS_RUN_BOOT_PROGRAM_ST		99
#define ADCS_SET_SGP4_ORBIT_PARAMS_ST	100
#define ADCS_SET_SGP4_ORBIT_INC_ST		101
#define ADCS_SET_SGP4_ORBIT_ECC_ST		102
#define ADCS_SET_SGP4_ORBIT_EPOCH_ST	103
#define ADCS_SET_SGP4_ORBIT_RAAN_ST		104
#define ADCS_SET_SGP4_ORBIT_ARG_OF_PER_ST	105
#define ADCS_SET_SGP4_ORBIT_BSTAR_DRAG_ST	106
#define ADCS_SET_SGP4_ORBIT_MEAN_MOT_ST	107
#define ADCS_SET_SGP4_ORBIT_MEAN_ANOM_ST	108

#define ADCS_RESET_BOOT_REGISTER_ST		109
#define ADCS_SAVE_CONFIG_ST				110
#define ADCS_SAVE_ORBIT_PARAM_ST		111
#define ADCS_SET_UNIX_TIME_ST			112
#define ADCS_CACHE_ENABLE_ST			113
#define ADCS_DEPLOY_MAG_BOOM_ST			114
#define ADCS_RUN_MODE_ST				115
#define ADCS_SET_MTQ_CONFIG_ST			117
#define ADCS_SET_MAGNETMTR_MOUNT_ST		118
#define ADCS_SET_MAGNETMTR_OFFSET_ST	119
#define ADCS_SET_MAGNETMTR_SENSTVTY_ST	120
#define ADCS_SET_DETUMB_CTRL_PARAM_ST	121
#define ADCS_SET_YWHEEL_CTRL_PARAM_ST	122
#define ADCS_SET_RWHEEL_CTRL_PARAM_ST	123
#define ADCS_SET_MOMENT_INTERTIA_ST		124
#define ADCS_PROD_INERTIA_ST			125
#define ADCS_ESTIMATION_PARAM1_ST		126
#define ADCS_ESTIMATION_PARAM2_ST		127
#define ADCS_GET_CURR_UNIX_TIME_ST		128
#define ADCS_MTQ_CONFIG_ST				135
#define ADCS_RW_CONFIG_ST				136
#define ADCS_GYRO_CONFIG_ST				137
#define ADCS_CSS_CONFIG_ST				138
#define ADCS_CSS_RELATIVE_SCALE_ST		139
#define ADCS_CSS_THRESHOLD_ST			140
#define ADCS_MAGNETMTR_MOUNT_TRNSFRM_ST	141
#define ADCS_MAGETMTR_CHNL_1_3_OFFST_ST	142
#define ADCS_MAGNETMTR_SENS_MATRIX_ST	143
#define ADCS_RATE_SENSOR_OFFSET_ST		144
#define ADCS_RATE_SENSOR_MULT_ST		145
#define ADCS_DETUMBLING_GAIN_CONFIG_ST	146
#define ADCS_Y_MOMENT_GAIN_CONFIG_ST	147
#define ADCS_REF_WHEEL_MOMENTUM_ST		148
#define ADCS_RWHEEL_GAIN_ST				149
#define ADCS_MOMENT_INERTIA_ST			150
#define ADCS_NOISE_CONFIG_ST			151
#define ADCS_USE_IN_EKF_ST				152
#define ADCS_SET_EST_MODE				153
#define ADCS_GET_FULL_CONFIG_ST			154
#define ADCS_GET_TLM_BY_ID_ST			172
#define ADCS_DUMMY_FUNC					199
//#define ADCS_

//sw
#define RESET_DELAYED_CM_LIST_ST 		32
#define RESET_APRS_LIST_ST				15
#define RESET_FRAM_ST					42

//special operations
#define DELETE_UNF_CUF_ST				156
#define UPLOAD_CUF_ST					157
#define CON_UNF_CUF_ST					158
#define PAUSE_UP_CUF_ST					159
#define EXECUTE_CUF						160
#define REVERT_CUF						161
#endif /* SPLTYPES_H_ */
