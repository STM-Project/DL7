#include "draw.h"

/************************************ POLYGON ******************************************/

void DRAW_Polygon(const GUI_POINT * pPoint, int cop, int x, int y, U8 size,	GUI_COLOR Color_FILL, GUI_COLOR Color_BORDER)
{
	GUI_SetPenSize(size);
	GUI_SetColor(Color_FILL);
	GUI_FillPolygon(pPoint, cop, x, y);
	GUI_SetColor(Color_BORDER);
	GUI_DrawPolygon(pPoint, cop, x, y);
}

/************************************** CIRCLE ***************************************/

void DRAW_Circle(int x0, int y0, int r, U8 size, GUI_COLOR Color_FILL, GUI_COLOR Color_BORDER)

{
	GUI_SetPenSize(size);
	GUI_SetColor(Color_FILL);
	GUI_FillCircle(x0, y0, r);
	GUI_SetColor(Color_BORDER);
	GUI_DrawCircle(x0, y0, r);
}

/*************************************** LINE *************************************/

void DRAW_Line(int x0, int y0, int x1, int y1, U8 size, GUI_COLOR Color)
{
	GUI_SetPenSize(size);
	GUI_SetColor(Color);
	GUI_DrawLine(x0, y0, x1, y1);
}

/************************************RECTANGLE***********************************************************************/

void DRAW_Rect(int x0, int y0, int xSize, int ySize, U8 size, GUI_COLOR Color_FILL, GUI_COLOR Color_BORDER)
{
	if (Color_FILL != Color_BORDER)
	{
		GUI_SetColor(Color_BORDER);
		GUI_DrawRoundedFrame(x0, y0, x0+xSize-1, y0+ySize-1,0,size);

		GUI_SetColor(Color_FILL);
		GUI_FillRect(x0+size, y0+size, x0+xSize-size-1, y0+ySize-size-1);
	}
	else
	{
		GUI_SetColor(Color_BORDER);
		GUI_FillRect(x0, y0, x0+xSize-1, y0+ySize-1);
	}
}

/************************************FRAME***********************************************************************/

void DRAW_RoundedFrame(int x0, int y0, int xSize, int ySize, int r, U8 size, GUI_COLOR Color)
{
	GUI_SetColor(Color);
	GUI_DrawRoundedFrame(x0, y0, x0+xSize, y0+ySize, r, size);
}

void DRAW_RoundedRect(int x0, int y0, int xSize, int ySize, int r, GUI_COLOR Color)
{
	GUI_SetPenSize(1);
	GUI_SetColor(Color);
	GUI_AA_FillRoundedRect(x0, y0, x0+xSize-1, y0+ySize-1, r);
}

/************************************ DISPLAY TEXT ******************************/

void DRAW_Text(const char GUI_UNI_PTR * s, int x, int y, GUI_COLOR Color,	GUI_COLOR ColorBg, const GUI_FONT GUI_UNI_PTR *pNewFont)
{
	GUI_SetFont((const GUI_FONT GUI_UNI_PTR *) pNewFont);
	GUI_SetColor(Color);
	if (ColorBg != GUI_TRANSPARENT)
	{
		GUI_SetBkColor(ColorBg);
		GUI_DispStringHCenterAt((const char GUI_UNI_PTR *) s, x, y);
	}
	else
	{
		GUI_SetTextMode(GUI_TM_TRANS);
		GUI_DispStringHCenterAt((const char GUI_UNI_PTR *) s, x, y);
		GUI_SetTextMode(GUI_TM_NORMAL);
	}
}

/************************************ DISPLAY I32 NUMBER ******************************/

void DRAW_I32Number(I32 v, int x, int y, GUI_COLOR Color, GUI_COLOR ColorBg,
U8 Len, const GUI_FONT GUI_UNI_PTR *pNewFont)
{
	GUI_SetBkColor(ColorBg);
	GUI_SetFont((const GUI_FONT GUI_UNI_PTR *) pNewFont);
	GUI_SetColor(Color);
	GUI_DispDecAt(v, x, y, Len);
}

/************************************ DISPLAY FLOAT NUMBER ******************************/

void DRAW_FloatNumber(float v, int x, int y, GUI_COLOR Color,	GUI_COLOR ColorBg, char Len, char Decs,	const GUI_FONT GUI_UNI_PTR *pNewFont)
{
	GUI_SetBkColor(ColorBg);
	GUI_GotoXY(x, y);
	GUI_SetFont((const GUI_FONT GUI_UNI_PTR *) pNewFont);
	GUI_SetColor(Color);
	GUI_DispFloatFix(v, Len, Decs);
}
