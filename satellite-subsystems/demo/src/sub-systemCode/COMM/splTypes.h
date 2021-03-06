/*
 * splTypes.h
 *
 *  Created on: Dec 5, 2018
 *      Author: DataBaseRefactorn
 */

#ifndef SPLTYPES_H_
#define SPLTYPES_H_

//Types (TM)
#define TM_GENERAL_T						13
#define BEACON_T							3
#define APRS								20
#define ACK_TYPE							TM_GENERAL_T
#define LOG_T								TM_GENERAL_T
#define DUMP_T								173
#define GECKO_REGISTERS_T					10
#define IMAGE_DUMP_T						11
#define IMAGE_DATABASE_DUMP_T				12
#define ACK_ST								90
#define TM_ADCS_T 							154
#define TM_ONLINE_TM_T	        			88

//Types (TC)
#define COMM_T 								13
#define GENERAL_T 							20
#define TC_ADCS_T 							154
#define GENERALLY_SPEAKING_T				195
#define PAYLOAD_T							251
#define EPS_T								6
#define SOFTWARE_T 							42
#define CUF_T								69
#define TC_ONLINE_TM_T				    TM_ONLINE_TM_T

//SubType (TM)
#define BEACON_ST 							25
#define APRS_PACKET_FRAM 					33
#define LOG_EVENTS_ST						49
#define LOG_ERROR_ST						50
#define OFFLINE_SETTING_ST					255

//Payload dump subTypes:
#define IMAGE_DATABASE_DUMP_ST				96
#define IMAGE_DATABASE_DUMP_FIRST_CHUNK_ST	97

#define IMAGE_DUMP_THUMBNAIL6_ST			98
#define IMAGE_DUMP_THUMBNAIL5_ST			99
#define IMAGE_DUMP_THUMBNAIL4_ST			100
#define IMAGE_DUMP_THUMBNAIL3_ST			101
#define IMAGE_DUMP_THUMBNAIL2_ST			102
#define IMAGE_DUMP_THUMBNAIL1_ST			103
#define IMAGE_DUMP_RAW_ST					104
#define IMAGE_DUMP_JPG_ST					105
#define IMAGE_DUMP_JPG_FIRST_CHUNK_ST		106

#define GECKO_REGISTERS_ST					107

//Subtypes (TC)
//comm
#define MUTE_ST								0
#define UNMUTE_ST							1
#define ACTIVATE_TRANS_ST					24
#define SHUT_TRANS_ST						25
#define CHANGE_TRANS_RSSI_ST				26
#define APRS_DUMP_ST						90
#define STOP_DUMP_ST						91
#define TIME_FREQUENCY_ST					5
#define UPDATE_BIT_RATE_ST					55

//general
#define SOFT_RESET_ST						68
#define HARD_RESET_ST						66
#define RESET_SAT_ST 						67
#define GRACEFUL_RESET_ST					69
#define UPLOAD_TIME_ST 						17
#define GENERIC_I2C_ST						0
#define DUMP_ST								33
#define DELETE_PACKETS_ST					35
#define FORMAT_SD_ST						36
#define RESET_FILE_ST						45
#define RESTSRT_FS_ST						46
#define DUMMY_FUNC_ST						122
#define REDEPLOY							56
#define ARM_DISARM							57
#define STOP_TM_ST							200
#define RESUME_TM_ST						201

//payload
#define SEND_IMG_CHUNCK_CHUNK_FIELD_ST		1
#define SEND_IMG_CHUNCK_BIT_FIELD_ST 		2
#define TAKE_IMG_ST							3
#define TAKE_IMG_SPECIAL_VAL_ST				4
#define TAKE_IMG_WITH_TIME_INTERVALS	 	5
#define UPDATE_PHOTOGRAPHY_VALUES_ST		6
#define DELETE_IMG_FILE_ST					7
#define DELETE_IMG_ST						8
#define MOV_IMG_CAM_OBS_ST					9
#define CREATE_THUMBNAIL_FROM_IMG_ST		10
#define CREATE_JPEG_FROM_IMG_ST				11
#define RESET_DATA_BASE_ST					12
#define SEND_IMAGE_DATA_BASE_ST				13
#define FILE_TYPE_DUMP_ST					14
#define UPDATE_DEF_DUR_ST					15
#define SET_CHUNK_SIZE						16
#define STOP_TAKING_IMG_TIME_INTERVAL_ST	17
#define	OFF_CAM_ST							18
#define ON_CAM_ST							19
#define OFF_FUTURE_AUTO_THUMB				20
#define ON_FUTURE_AUTO_THUMB				21
#define OFF_AUTO_THUMB						22
#define ON_AUTO_THUMB						23
#define GET_GECKO_REGISTER					24
#define SET_GECKO_REGISTER					25
#define RE_INIT_CAM_MANAGER					26

//eps
#define ALLOW_ADCS_ST						99
#define SHUT_ADCS_ST						100
#define ALLOW_CAM_ST						101
#define SHUT_CAM_ST							102
#define UPD_LOGIC_VOLT_ST					255
#define UPD_COMM_VOLTAGE					20
#define CHANGE_HEATER_TMP_ST				0
#define UPDATE_EPS_ALPHA_ST					11
#define EPS_WDT_RESET_ST					13

//adcs
#define ADCS_I2C_GENRIC_ST					0
#define ADCS_COMPONENT_RESET_ST				1
#define ADCS_SET_CURR_UNIX_TIME_ST			2
#define ADCS_CACHE_ENABLE_ST				3
#define ADCS_RESET_BOOT_REGISTER_ST			6
#define ADCS_DEPLOY_MAG_BOOM_ST				7
#define ADCS_RUN_MODE_ST					10
#define ADCS_SET_PWR_CTRL_DEVICE_ST			11
#define ADCS_CLEAR_ERRORS_ST				12
#define ADCS_SET_ATT_CTRL_MODE_ST			13
#define ADCS_SET_EST_MODE_ST				14
#define ADCS_SET_MAG_OUTPUT_ST				16
#define ADCS_SET_WHEEL_SPEED_ST				17
#define ADCS_SET_MTQ_CONFIG_ST				21
#define ADCS_RW_CONFIG_ST					22
#define ADCS_GYRO_CONFIG_ST					23
#define ADCS_CSS_CONFIG_ST					24
#define ADCS_CSS_RELATIVE_SCALE_ST			25
#define ADCS_SET_MAGNETMTR_MOUNT_ST			33
#define ADCS_SET_MAGNETMTR_OFFSET_ST		34
#define ADCS_SET_MAGNETMTR_SENSTVTY_ST		35
#define ADCS_RATE_SENSOR_OFFSET_ST			36
#define ADCS_SET_STAR_TRACKER_CONFIG_ST		37
#define ADCS_SET_DETUMB_CTRL_PARAM_ST		38
#define ADCS_SET_YWHEEL_CTRL_PARAM_ST		39
#define ADCS_SET_RWHEEL_CTRL_PARAM_ST		40
#define ADCS_SET_MOMENT_INTERTIA_ST			41
#define ADCS_PROD_INERTIA_ST				42
#define ADCS_ESTIMATION_PARAM1_ST			43
#define ADCS_ESTIMATION_PARAM2_ST			44
#define ADCS_SET_SGP4_ORBIT_PARAMS_ST		45
#define ADCS_SET_SGP4_ORBIT_INC_ST			46
#define ADCS_SET_SGP4_ORBIT_ECC_ST			47
#define ADCS_SET_SGP4_ORBIT_RAAN_ST			48
#define ADCS_SET_SGP4_ORBIT_ARG_OF_PER_ST	49
#define ADCS_SET_SGP4_ORBIT_BSTAR_DRAG_ST	50
#define ADCS_SET_SGP4_ORBIT_MEAN_MOT_ST		51
#define ADCS_SET_SGP4_ORBIT_MEAN_ANOM_ST	52
#define ADCS_SET_SGP4_ORBIT_EPOCH_ST		53
#define ADCS_SET_MAGNETOMETER_MODE_ST		56
#define ADCS_SAVE_CONFIG_ST					63
#define ADCS_SAVE_ORBIT_PARAM_ST			64
#define ADCS_SAVE_IMAGE_ST					80

#define ADCS_NOP_ST							89
#define ADCS_UPDATE_TLM_ELEMENT_AT_INDEX_ST	90
#define ADCS_RESET_TLM_ELEMENTS_ST			91
#define ADCS_SET_TLM_OVERRIDE_FLAG_ST		92
#define ADCS_SET_ADCS_LOOP_PARAMETERS_ST	93
#define ADCS_UPDATE_TLM_PERIOD_VEC_ST		94
#define ADCS_UPDATE_TLM_SAVE_VEC_ST			95

#define ADCS_SET_BOOT_INDEX_ST				100
#define ADCS_RUN_BOOT_PROGRAM_ST			101
#define ADCS_GET_GENERAL_INFO_ST			128
#define ADCS_GET_BOOT_PROGRAM_INFO_ST		130
#define ADCS_GET_CURR_UNIX_TIME_ST			140
#define ADCS_GET_SRAM_LATCHUP_COUNTERS_ST	142
#define ADCS_GET_EDAC_COUNTERS_ST			143
#define ADCS_GET_COMM_STATUS_ST				144
#define ADCS_GET_CURRENT_STATE_ST			145
#define ADCS_GET_EST_ANG_ST					146
#define ADCS_GET_EST_ANG_RATE_ST			147
#define ADCS_GET_SATELLITE_POSITION_ST		150
#define ADCS_GET_MAGNETIC_FIELD_VEC_ST		151
#define ADCS_GET_COARSE_SUN_VEC_ST			152
#define ADCS_GET_FINE_SUN_VEC_ST			153
#define ADCS_GET_NADIR_VECTOR_ST			154
#define ADCS_GET_SENSOR_RATES_ST			155
#define ADCS_GET_WHEEL_SPEED_ST				156
#define ADCS_GET_MAGNETORQUER_CMD_ST		157
#define ADCS_GET_WHEEL_SPEED_CMD_ST			158
#define ADCS_GET_RAW_CSS_1_6_MEASURMENTS_ST	168
#define ADCS_GET_RAW_CSS_7_10_MEASURMENTS_ST		169	
#define ADCS_GET_RAW_MAGNETOMETER_MEAS_ST			170
#define ADCS_GET_C_SENSE_CURRENT_MEASUREMENTS_ST	171
#define ADCS_GET_C_CONTROL_CURRENT_MEASUREMENTS_ST	172
#define ADCS_GET_WHEEL_CURRENTS_TLM_ST		173
#define ADCS_GET_ADCS_TEMPERATURE_TLM_ST	174
#define ADCS_GET_RATE_SENSOR_TEMP_TLM_ST	175
#define ADCS_GET_RAW_GPS_STATUS_ST			176
#define ADCS_GET_RAW_GPS_TIME_ST			177
#define ADCS_GET_RAW_GPS_X_ST				178
#define ADCS_GET_RAW_GPS_Y_ST				179
#define ADCS_GET_RAW_GPS_Z_ST				180
#define ADCS_GET_STATE_TLM_ST				190
#define ADCS_GET_ADCS_MEASUREMENTS_ST		191
#define ADCS_GET_ACTUATORS_CMDS_ST			192
#define ADCS_GET_ESTIMATION_METADATA_ST		193
#define ADCS_GET_SENSOR_MEASUREMENTS_ST		194
#define ADCS_GET_POW_TEMP_MEAS_TLM_ST		195
#define ADCS_GET_ADCS_EXC_TIMES_ST			196
#define ADCS_GET_PWR_CTRL_DEVICE_ST			197
#define ADCS_GET_MISC_CURRENT_MEAS_ST		198
#define ADCS_GET_COMMANDED_ATTITUDE_ANGLES_ST	199
#define ADCS_GET_FULL_CONFIG_ST				206
#define ADCS_GET_SGP4_ORBIT_PARAMETERS_ST	207
#define ADCS_GET_ADCS_RAW_GPS_MEAS_ST		210
#define ADCS_GET_STAR_TRACKER_TLM_ST		211
#define ADCS_GET_ADC_RAW_RED_MAG_ST			215
#define ADCS_GET_ACP_EXECUTION_STATE_ST		220


#define ADCS_GET_TLM_SAVE_VECTOR_ST			246
#define ADCS_GET_PERIOD_VECTOR_ST			247
#define ADCS_GET_LAST_SAVE_TIMES_ST			248
#define ADCS_GET_OPERATING_FLAGS_ST			249
#define ADCS_GET_TLM_ELEM_AT_INDEX_ST		250
#define ADCS_GET_ADCS_CONFIG_PARAM_ST 		251
#define ADCS_GET_TLM_OVERRIDE_FLAG_ST		252
#define ADCS_ACK_DATA_ST					254
#define ADCS_SET_DATA_LOG_ST				255


//sw
#define RESET_DELAYED_CM_LIST_ST 			32
#define RESET_APRS_LIST_ST					15
#define RESET_FRAM_ST						42
#define FRAM_WRITE_ST						43
#define FRAM_READ_ST						44

//online TM
#define GET_ONLINE_TM_INDEX_ST				0
#define RESET_OFF_LINE_LIST_ST				1
#define ADD_ITEM_OFF_LINE_LIST_ST			2
#define	DELETE_ITEM_OFF_LINE_LIST_ST		3
#define GET_OFFLINE_LIST_SETTING_ST			4

//CUF operations
#define DELETE_UNF_CUF_ST					156
#define UPLOAD_CUF_ST						157
#define CON_UNF_CUF_ST						158
#define PAUSE_UP_CUF_ST						159
#define EXECUTE_CUF							160
#define REVERT_CUF							161

#endif /* SPLTYPES_H_ */
