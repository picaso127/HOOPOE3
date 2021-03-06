/*
 * DB_RequestHandling.c
 *
 *  Created on: Jun 6, 2019
 *      Author: Roy's Laptop
 */

#include <freertos/FreeRTOS.h>
#include <string.h>

#include "../Drivers/GeckoCameraDriver.h"
#include "DB_RequestHandling.h"

ImageDataBaseResult TakePicture(ImageDataBase database, unsigned char* data)
{
	Boolean8bit isTestPattern = 0;

	memcpy(&isTestPattern, data, sizeof(Boolean8bit));

	TurnOnGecko();

	return takePicture(database, isTestPattern);
}
ImageDataBaseResult TakeSpecialPicture(ImageDataBase database, unsigned char* data)
{
	Boolean8bit isTestPattern = 0;
	uint32_t frameAmount = 0;
	uint32_t frameRate = 0;
	byte adcGain = 0;
	byte pgaGain = 0;
	uint16_t sensorOffset = 0;
	uint32_t exposure = 0;

	memcpy(&isTestPattern, data, sizeof(Boolean8bit));
	memcpy(&frameAmount, data + 1, sizeof(uint32_t));
	memcpy(&frameRate, data + 5, sizeof(uint32_t));
	memcpy(&adcGain, data + 9, sizeof(byte));
	memcpy(&pgaGain, data + 10, sizeof(byte));
	memcpy(&sensorOffset, data + 11, sizeof(uint16_t));
	memcpy(&exposure, data + 13, sizeof(uint32_t));

	TurnOnGecko();

	return takePicture_withSpecialParameters(database, frameAmount, frameRate, adcGain, pgaGain, sensorOffset, exposure, isTestPattern);
}
ImageDataBaseResult DeletePictureFile(ImageDataBase database, unsigned char* data)
{
	imageid cameraID = 0;
	fileType GroundFileType = 0;

	memcpy(&cameraID, data, sizeof(imageid));
	memcpy(&GroundFileType, data + 2, sizeof(byte));

	if (cameraID == 0)
		return clearImageDataBase();
	else
		return DeleteImageFromOBC(database, cameraID, GroundFileType);
}
ImageDataBaseResult DeletePicture(ImageDataBase database, unsigned char* data)
{
	imageid cameraID = 0;
	fileType GroundFileType = 0;

	memcpy(&cameraID, data, sizeof(imageid));
	memcpy(&GroundFileType, data + 2, sizeof(byte));

	TurnOnGecko();

	return DeleteImageFromPayload(database, cameraID);
}
ImageDataBaseResult TransferPicture(ImageDataBase database, unsigned char* data)
{
	imageid cameraID = 0;

	memcpy(&cameraID, data, sizeof(imageid));

	TurnOnGecko();

	return transferImageToSD(database, cameraID);
}
ImageDataBaseResult CreateThumbnail(unsigned char* data)
{
	imageid cameraID = 0;
	byte reductionLevel = 0;
	Boolean8bit Skipping = TRUE_8BIT;

	memcpy(&cameraID, data, sizeof(imageid));
	memcpy(&reductionLevel, data + 2, sizeof(byte));

	ImageDataBaseResult DB_result = CreateImageThumbnail(cameraID, reductionLevel, Skipping);

	return DB_result;
}
ImageDataBaseResult CreateJPG(unsigned char* data)
{
	imageid cameraID = 0;
	unsigned int quality_factor = 0;

	memcpy(&cameraID, data, sizeof(imageid));
	memcpy(&quality_factor, data + 2, sizeof(byte));

	ImageDataBaseResult DB_result = compressImage(cameraID, quality_factor);

	return DB_result;
}
ImageDataBaseResult UpdatePhotographyValues(ImageDataBase database, unsigned char* data)
{
	unsigned int frameAmount;
	unsigned int frameRate;
	unsigned char adcGain;
	unsigned char pgaGain;
	unsigned short sensorOffset;
	unsigned int exposure;

	memcpy(&frameAmount, data, sizeof(int));
	memcpy(&frameRate, data + 4, sizeof(int));
	memcpy(&adcGain, data + 8, sizeof(char));
	memcpy(&pgaGain, data + 9, sizeof(char));
	memcpy(&sensorOffset, data + 10, sizeof(short));
	memcpy(&exposure, data + 12, sizeof(int));

	setCameraPhotographyValues(database, frameAmount, frameRate, adcGain, pgaGain, sensorOffset, exposure);

	return DataBaseSuccess;
}
ImageDataBaseResult setChunkDimensions(unsigned char* data)
{
	uint16_t width = 0;
	uint16_t height = 0;

	memcpy(&width, data, sizeof(uint16_t));
	memcpy(&height, data + sizeof(uint16_t), sizeof(uint16_t));

	return setChunkDimensions_inFRAM(width, height);
}

int GeckoSetRegister(unsigned char* data)
{
	uint8_t reg_index = 0;
	uint32_t reg_val = 0;

	memcpy(&reg_index, data, sizeof(uint8_t));
	memcpy(&reg_val, data + sizeof(uint8_t), sizeof(uint32_t));

	return GECKO_SetRegister(reg_index, reg_val);
}
