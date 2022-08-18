/*
 * LISTVIEW_files.h
 *
 *  Created on: 29.04.2021
 *      Author: TomaszSok
 */

#ifndef GUI_UTILITIES_LISTVIEW_FILES_H_
#define GUI_UTILITIES_LISTVIEW_FILES_H_

#include "DIALOG.h"

void LISTVIEW_SetFileNamesToCopy(WM_HWIN hItem);
void LISTVIEW_PopulateWithFiles(LISTVIEW_Handle hListview, char *folder);
void LISTVIEW_PopulateWithDataArchiveFiles(LISTVIEW_Handle hListview, char *folder);
void LISTVIEW_PopulateWithParametersFiles(LISTVIEW_Handle hListview, char *folder);
void LISTVIEW_PopulateWithBinaryFiles(LISTVIEW_Handle hListview, char *folder);
void LISTVIEW_PopulateWithFolders(LISTVIEW_Handle hListview, char *folder);
void LISTVIEW_GetFileFullPath(LISTVIEW_Handle hListview, char* fullPath);

#endif /* GUI_UTILITIES_LISTVIEW_FILES_H_ */
