/*
 * skins.h
 *
 *  Created on: 12.05.2017
 *      Author: Tomaszs
 */

#ifndef GUI_UTILITIES_SKINS_H_
#define GUI_UTILITIES_SKINS_H_

#include "DIALOG.h"

extern const GUI_COLOR penColor[6];

void SKINS_SetLightSkin(void);
void SKINS_SetDarkSkin(void);
void MULTIEDIT_SetSkin(MULTIEDIT_HANDLE hObj, unsigned Index);
void EDIT_SetSkin(EDIT_Handle hObj, unsigned Index);
void DROPDOWN_SetTextSkin(DROPDOWN_Handle hObj);
void TREEVIEW_SetImagesSkin(TREEVIEW_Handle hObj);
void ICONVIEW_SetSkinColors(ICONVIEW_Handle hObj);
void LISTWHEEL_SetSkinColors(LISTWHEEL_Handle hObj);
void GRAPH_SetSkinColors(GRAPH_Handle hObj);
void SKINS_InitGraphics(void);
void SKINS_ChangeGraphics(void);
GUI_COLOR SKINS_GetTextColor(void);
GUI_COLOR SKINS_GetBkColor(void);
char SKINS_GetStatus(void);
void SKINS_SetStatus(char newStatus);
void SKINS_UpdateCalendar(void);

#endif /* GUI_UTILITIES_SKINS_H_ */
