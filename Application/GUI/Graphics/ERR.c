/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                           www.segger.com                           *
**********************************************************************
*                                                                    *
* C-file generated by                                                *
*                                                                    *
*        Bitmap Converter for emWin V5.24.                           *
*        Compiled Jan 27 2014, 11:27:47                              *
*        (c) 1998 - 2013 Segger Microcontroller GmbH && Co. KG       *
*                                                                    *
**********************************************************************
*                                                                    *
* Source file: ERR                                                   *
* Dimensions:  35 * 21                                               *
* NumColors:   2                                                     *
* Description: Plik z grafiką awaryją. uzywany w przypadku awarii
*              plików graficznych na karcie uSD                      *
**********************************************************************
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

extern GUI_CONST_STORAGE GUI_BITMAP bmERR;

/*********************************************************************
*
*       Palette
*
*  Description
*    The following are the entries of the palette table.
*    The entries are stored as a 32-bit values of which 24 bits are
*    actually used according to the following bit mask: 0xBBGGRR
*
*    The lower   8 bits represent the Red   component.
*    The middle  8 bits represent the Green component.
*    The highest 8 bits represent the Blue  component.
*/
static GUI_CONST_STORAGE GUI_COLOR _ColorsERR[] = {
  0xFFFFFF, 0xFF0000
};

static GUI_CONST_STORAGE GUI_LOGPALETTE _PalERR = {
  2,  // Number of entries
  0,  // No transparency
  &_ColorsERR[0]
};

static GUI_CONST_STORAGE unsigned char _acERR[] = {
  ________, ________, ________, ________, ________,
  ________, ________, ________, ________, ________,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______,
  __XXX___, ________, ________, ______XX, X_______,
  __XX____, ________, ________, _______X, X_______,
  __XX___X, XXXX__XX, XXX___XX, XXX____X, X_______,
  __XX___X, X_____XX, __XX__XX, __XX___X, X_______,
  __XX___X, X_____XX, __XX__XX, __XX___X, X_______,
  __XX___X, X_____XX, __XX__XX, __XX___X, X_______,
  __XX___X, XXX___XX, XXX___XX, XXX____X, X_______,
  __XX___X, X_____XX, __XX__XX, __XX___X, X_______,
  __XX___X, X_____XX, __XX__XX, __XX___X, X_______,
  __XX___X, X_____XX, __XX__XX, __XX___X, X_______,
  __XX___X, XXXX__XX, __XX__XX, __XX___X, X_______,
  __XX____, ________, ________, _______X, X_______,
  __XXX___, ________, ________, ______XX, X_______,
  __XXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, X_______,
  ___XXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, ________,
  ________, ________, ________, ________, ________,
  ________, ________, ________, ________, ________
};

GUI_CONST_STORAGE GUI_BITMAP bmERR = {
  35, // xSize
  21, // ySize
  5, // BytesPerLine
  1, // BitsPerPixel
  _acERR,  // Pointer to picture data (indices)
  &_PalERR   // Pointer to palette
};

/*************************** End of file ****************************/
