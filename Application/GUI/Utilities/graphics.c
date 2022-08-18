/*
 * graphics.c
 *
 *  Created on: 11.01.2017
 *      Author: Tomaszs
 */

#include "graphics.h"

UINT LoadGraphicFileToRAM(const char *pathToFile, BYTE *buff)
{
	FILINFO fno;
	FIL graphicFile;
	UINT bytesReadFromFile;
	f_open(&graphicFile, pathToFile, FA_READ);
	f_stat(pathToFile,&fno);
	f_read(&graphicFile, buff, fno.fsize, &bytesReadFromFile);
	f_close(&graphicFile);

	return bytesReadFromFile;
}
