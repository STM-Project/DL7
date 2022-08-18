/*
 * UserMessageID.h
 *
 *  Created on: 19.01.2017
 *      Author: Tomaszs
 */

#ifndef UTILITIES_USERMESSAGEID_H_
#define UTILITIES_USERMESSAGEID_H_

#include "WM.h"

#define WM_USER_LOGIN   										WM_USER+1
#define WM_USER_ARCHIVE   									WM_USER+2
#define WM_USER_ALARM   										WM_USER+3
#define WM_USER_WIN_NAME 										WM_USER+4
#define WM_USER_REFRESH 										WM_USER+5
#define WM_USER_SEL_CHANGED 								WM_USER+5
#define WM_USER_SAVE_TEMP 									WM_USER+6
#define WM_USER_NEWARCHIVEID 								WM_USER+7
#define WM_USER_REFRESH_LANG 								WM_USER+8
#define WM_REFRESH_CHAR 										WM_USER+9
#define WM_USER_BOARD_FIRMWARE_UPGRADE  		WM_USER+10
#define WM_USER_BOARD_WRITE_DATA_INPROGRESS WM_USER+11
#define WM_USER_REFRESH_SKIN								WM_USER+12
#define WM_USER_REFRESH_USB_LISTVIEW				WM_USER+13

extern WM_MESSAGE UserMessage;

void USERMESSAGE_ChangeWinTitle(const char * newTitle);
void USERMESSAGE_RefreshArchiveID(void);
void USERMESSAGE_Archive(int state);
void USERMESSAGE_SendBroadcast(int MsgId);

#endif /* UTILITIES_USERMESSAGEID_H_ */
