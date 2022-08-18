#ifndef MET_H
#define MET_H

#include "GUI.h"

void DRAW_Polygon(const GUI_POINT * pPoint, int cop, int x, int y, U8 size,
		GUI_COLOR Color_FILL, GUI_COLOR Color_BORDER);

void DRAW_Circle(int x0, int y0, int r, U8 size, GUI_COLOR Color_FILL,
		GUI_COLOR Color_BORDER);

void DRAW_Line(int x0, int y0, int x1, int y1, U8 size, GUI_COLOR Color);

void DRAW_Rect(int x0, int y0, int xSize, int ySize, U8 size, GUI_COLOR Color_FILL,
		GUI_COLOR Color_BORDER);

void DRAW_RoundedFrame(int x0, int y0, int xSize, int ySize, int r, U8 size, GUI_COLOR Color);

void DRAW_RoundedRect(int x0, int y0, int xSize, int ySize, int r, GUI_COLOR Color);

void DRAW_Text(const char GUI_UNI_PTR * s, int x, int y, GUI_COLOR Color,
		GUI_COLOR ColorBg, const GUI_FONT GUI_UNI_PTR *pNewFont);

void DRAW_FloatNumber(float v, int x, int y, GUI_COLOR Color,
		GUI_COLOR ColorBg, char Len, char Decs,
		const GUI_FONT GUI_UNI_PTR *pNewFont);

#endif
