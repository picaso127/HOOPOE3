/*
 * ImgCompressor.h
 *
 *  Created on: Jul 14, 2019
 *      Author: Roy's Laptop
 */

#ifndef IMGCOMPRESSOR_H_
#define IMGCOMPRESSOR_H_

#include "../../DataBase/DataBase.h"

ImageDataBaseResult JPEG_compressor(uint32_t compfact, unsigned int quality_factor, char pDst_filename[FILE_NAME_SIZE], byte* buffer);

ImageDataBaseResult Create_BMP_File(char pSrc_filename_raw[FILE_NAME_SIZE], char pSrc_filename[FILE_NAME_SIZE], unsigned int compfact, byte* buffer);

/*
 * Usage: jpge <sourcefile> <destfile> <quality_factor>
 * sourcefile: Source image file, in any format stb_image.c supports.
 * destfile: Destination JPEG file.
 * quality_factor: 1-100, higher=better
 */
ImageDataBaseResult CompressImage(imageid id, fileType reductionLevel, unsigned int quality_factor);

#endif /* IMGCOMPRESSOR_H_ */
