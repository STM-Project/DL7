#include "StartUpDLG.h"
#include "ff.h"

#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_IMAGE_0     (GUI_ID_USER + 0x01)

extern GUI_CONST_STORAGE GUI_BITMAP bmERR;

static BYTE ALIGN_32BYTES(bufferForLogoBigGraphic[768000]) __attribute__ ((section(".sdram")));
UINT bytesreadFromLogoBig;

static const GUI_WIDGET_CREATE_INFO _aSplashScreenDialogCreate[] =
		{
				{ WINDOW_CreateIndirect, "StartUp_Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
				{ IMAGE_CreateIndirect, "Logo", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
		};

void loadStarUpWindowGraphicsToRAM()
{
	FILINFO fno;
	FIL graphicFile;
	f_open(&graphicFile, "0:image/blogo.dta", FA_READ);
	f_stat("0:image/blogo.dta",&fno);
	if( fno.fsize <= 768000)
			f_read(&graphicFile, bufferForLogoBigGraphic, fno.fsize, &bytesreadFromLogoBig);
	f_close(&graphicFile);
}

static void _cbSplashScreenDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:
		WINDOW_SetBkColor(pMsg->hWin, GUI_BLACK);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
		if (bytesreadFromLogoBig)
			IMAGE_SetDTA(hItem, &bufferForLogoBigGraphic, bytesreadFromLogoBig);
		else
			IMAGE_SetBitmap(hItem, &bmERR);
		break;

	default:
		WM_DefaultProc(pMsg);
	}
}

WM_HWIN CreateSplashScreenDLG(void)
{
	WM_HWIN hWin;
	hWin = GUI_CreateDialogBox(_aSplashScreenDialogCreate, GUI_COUNTOF(_aSplashScreenDialogCreate), _cbSplashScreenDialog, WM_HBKWIN, 0, 0);
	return hWin;
}
