/*
 * LISTVIEW_files.c
 *
 *  Created on: 29.04.2021
 *      Author: TomaszSok
 */

#include "LISTVIEW_files.h"
#include "ff.h"
#include <string.h>
#include "mini-printf.h"
#include "fatfs.h"
#include "USBtask.h"

static FILINFO t_filinfo;
static DIR t_dir;

static int IsParameterFile(const char *fileName)
{
	if (strstr(fileName, ".par"))
		return 1;
	else if (strstr(fileName, ".PAR"))
		return 1;
	else
		return 0;
}

static int IsBinaryFile(const char *fileName)
{
	if (strstr(fileName, ".bin"))
		return 1;
	else if (strstr(fileName, ".BIN"))
		return 1;
	else
		return 0;
}

static int IsDataArchiveFile(const char *fileName)
{
	if (strstr(fileName,"AD"))
		return 1;
	else
		return 0;
}

void LISTVIEW_SetFileNamesToCopy(WM_HWIN hItem)
{
	int selectedRow = LISTVIEW_GetSel(hItem);
	char folder[20]={0};
	char fileName[80]={0};
	if(0<=selectedRow)
	{
		LISTVIEW_GetItemTextSorted(hItem, 0, selectedRow, folder, 20);
		LISTVIEW_GetItemTextSorted(hItem, 1, selectedRow, fileName, 80);
		mini_snprintf(SrcFilePath, 100, "%s/%s",folder,fileName);
		mini_snprintf(DstFilePath, 50, "1:%s", fileName);
	}
}

void LISTVIEW_PopulateWithFiles(LISTVIEW_Handle hListview, char *folder)
{
	char date[20]={0};
	GUI_ConstString row[3]={0};

	row[0] = folder;
	if (FR_OK == f_opendir(&t_dir, folder))
	{
		if (FR_OK == f_readdir(&t_dir, &t_filinfo))
		{
			while (0 != t_filinfo.fname[0])
			{
				if (AM_DIR != t_filinfo.fattrib)
				{
					row[1] = t_filinfo.fname;
					FATFS_CopyFileDateToString(date, &t_filinfo);
					row[2] = date;
					LISTVIEW_AddRow(hListview, row);
				}
				if (FR_OK != f_readdir(&t_dir, &t_filinfo))
					break;
			}
		}
		f_closedir(&t_dir);
	}
}

void LISTVIEW_PopulateWithDataArchiveFiles(LISTVIEW_Handle hListview, char *folder)
{
	char date[20]={0};
	GUI_ConstString row[3]={0};

	row[0] = folder;
	if (FR_OK == f_opendir(&t_dir, folder))
	{
		if (FR_OK == f_readdir(&t_dir, &t_filinfo))
		{
			while (0 != t_filinfo.fname[0])
			{
				if (AM_DIR != t_filinfo.fattrib)
				{
					if(IsDataArchiveFile(t_filinfo.fname))
					{
						row[1] = t_filinfo.fname;
						FATFS_CopyFileDateToString(date, &t_filinfo);
						row[2] = date;
						LISTVIEW_AddRow(hListview, row);
					}
				}
				if (FR_OK != f_readdir(&t_dir, &t_filinfo))
					break;
			}
		}
		f_closedir(&t_dir);
	}
}

void LISTVIEW_PopulateWithParametersFiles(LISTVIEW_Handle hListview, char *folder)
{
	char date[20]={0};
	GUI_ConstString row[3]={0};

	row[0] = folder;
	if (FR_OK == f_opendir(&t_dir, folder))
	{
		if (FR_OK == f_readdir(&t_dir, &t_filinfo))
		{
			while (0 != t_filinfo.fname[0])
			{
				if (AM_DIR != t_filinfo.fattrib)
				{
					if(IsParameterFile(t_filinfo.fname))
					{
						row[1] = t_filinfo.fname;
						FATFS_CopyFileDateToString(date, &t_filinfo);
						row[2] = date;
						LISTVIEW_AddRow(hListview, row);
					}
				}
				if (FR_OK != f_readdir(&t_dir, &t_filinfo))
					break;
			}
		}
		f_closedir(&t_dir);
	}
}

void LISTVIEW_PopulateWithBinaryFiles(LISTVIEW_Handle hListview, char *folder)
{
	char date[20]={0};
	GUI_ConstString row[3]={0};

	row[0] = folder;
	if (FR_OK == f_opendir(&t_dir, folder))
	{
		if (FR_OK == f_readdir(&t_dir, &t_filinfo))
		{
			while (0 != t_filinfo.fname[0])
			{
				if (AM_DIR != t_filinfo.fattrib)
				{
					if(IsBinaryFile(t_filinfo.fname))
					{
						row[1] = t_filinfo.fname;
						FATFS_CopyFileDateToString(date, &t_filinfo);
						row[2] = date;
						LISTVIEW_AddRow(hListview, row);
					}
				}
				if (FR_OK != f_readdir(&t_dir, &t_filinfo))
					break;
			}
		}
		f_closedir(&t_dir);
	}
}

void LISTVIEW_PopulateWithFolders(LISTVIEW_Handle hListview, char *folder)
{
	GUI_ConstString row[2]={0};

	row[0] = folder;
	if (FR_OK == f_opendir(&t_dir, folder))
	{
		if (FR_OK == f_readdir(&t_dir, &t_filinfo))
		{
			while (0 != t_filinfo.fname[0])
			{
				if (AM_DIR == t_filinfo.fattrib)
				{
					row[1] = t_filinfo.fname;
					LISTVIEW_AddRow(hListview, row);
				}
				if (FR_OK != f_readdir(&t_dir, &t_filinfo))
					break;
			}
		}
		f_closedir(&t_dir);
	}
}

void LISTVIEW_GetFileFullPath(LISTVIEW_Handle hListview, char* fullPath)
{
	int selectedRow = LISTVIEW_GetSel(hListview);
	char buffer[50]={0};
	if(0<=selectedRow)
	{
		LISTVIEW_GetItemTextSorted(hListview, 0, selectedRow, buffer, 50);
		strcpy(fullPath,buffer);
		strcat(fullPath,"/");
		LISTVIEW_GetItemTextSorted(hListview, 1, selectedRow, buffer, 50);
		strcat(fullPath,buffer);
		strcat(fullPath,"\0");
	}
	else
	{
		strcpy(fullPath,"\0");
	}
}
