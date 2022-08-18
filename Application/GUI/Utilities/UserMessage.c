/*
 * UserMessage.c
 *
 *  Created on: 12.05.2017
 *      Author: Tomaszs
 */

#include "UserMessage.h"
#include "fram.h"
#include "TitleBarDLG.h"

WM_MESSAGE UserMessage;

void USERMESSAGE_ChangeWinTitle(const char * newTitle)
{
	UserMessage.MsgId = WM_USER_WIN_NAME;
	UserMessage.hWin = TitleBar;
	UserMessage.hWinSrc = 0;
	UserMessage.Data.p = newTitle;
	WM_SendMessage(TitleBar, &UserMessage);
}

void USERMESSAGE_RefreshArchiveID(void)
{
	int ArchID = FRAM_Read(CURRENT_ARCH_ADDR);
	UserMessage.MsgId = WM_USER_NEWARCHIVEID;
	UserMessage.hWin = 0;
	UserMessage.hWinSrc = 0;
	UserMessage.Data.v = ArchID;
	WM_BroadcastMessage(&UserMessage);
}

void USERMESSAGE_Archive(int state)
{
	UserMessage.MsgId = WM_USER_ARCHIVE;
	UserMessage.hWin = TitleBar;
	UserMessage.hWinSrc = 0;
	UserMessage.Data.v = state;
	WM_SendMessage(TitleBar, &UserMessage);
}

void USERMESSAGE_SendBroadcast(int MsgId)
{
	WM_MESSAGE message;
	message.MsgId = MsgId;
	message.hWin = 0;
	message.hWinSrc = 0;
	message.Data.v = 0;
	WM_BroadcastMessage(&message);
}

