/*
 * skins.c
 *
 *  Created on: 20 sie 2014
 *      Author: Tomaszs
 */

#include "skins.h"
#include <stdint.h>
#include "fram.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontLato30;

extern void loadTitleBarWindowGraphicsToRAM();
extern void loadMenuBarWindowGrapihicsToRAM();
extern void loadSettingsWindowLightGraphicsToRAM();
extern void loadSettingsWindowDarkGraphicsToRAM();
extern void loadStarUpWindowGraphicsToRAM();

extern GUI_CONST_STORAGE GUI_BITMAP bmDirCloseLight;
extern GUI_CONST_STORAGE GUI_BITMAP bmDirCloseDark;

extern GUI_CONST_STORAGE GUI_BITMAP bmDirOpenLight;
extern GUI_CONST_STORAGE GUI_BITMAP bmDirOpenDark;

extern GUI_CONST_STORAGE GUI_BITMAP bmFileLight;
extern GUI_CONST_STORAGE GUI_BITMAP bmFileDark;

extern GUI_CONST_STORAGE GUI_BITMAP bmdirLight;
extern GUI_CONST_STORAGE GUI_BITMAP bmdirDark;

char SkinStatus;

char *(months[12]);
const char **pMonths = months;

char *(days[7]);
const char **pDays = days;

const GUI_COLOR penColor[6] =	{ GUI_BLUE, GUI_RED, 0x0000C000, GUI_MAGENTA, GUI_ORANGE, GUI_BROWN };

void SKINS_SetDefaultFonts(void)
{
	GUI_SetDefaultFont(&GUI_FontLato30);
	LISTVIEW_SetDefaultFont(&GUI_FontLato30);
	TEXT_SetDefaultFont(&GUI_FontLato30);
	CHECKBOX_SetDefaultFont(&GUI_FontLato30);
	BUTTON_SetDefaultFont(&GUI_FontLato30);
	DROPDOWN_SetDefaultFont(&GUI_FontLato30);
	EDIT_SetDefaultFont(&GUI_FontLato30);
	RADIO_SetDefaultFont(&GUI_FontLato30);
	MULTIPAGE_SetDefaultFont(&GUI_FontLato30);
	CALENDAR_SetDefaultFont(CALENDAR_FI_CONTENT, &GUI_FontLato30);
	CALENDAR_SetDefaultFont(CALENDAR_FI_HEADER, &GUI_FontLato30);
	HEADER_SetDefaultFont(&GUI_FontLato30);

}

void SKINS_SetLightSkin(void)
{
	WIDGET_EFFECT_Simple_SetColor(0, GUI_WHITE);
	WIDGET_SetDefaultEffect(&WIDGET_Effect_None);
	WM_SetDesktopColor(GUI_WHITE);
	GUI_SetBkColor(GUI_WHITE);

	SKINS_SetDefaultFonts();

	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_UNSEL, GUI_WHITE);
	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SEL, GUI_WHITE);
	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SELFOCUS, GUI_WHITE);
	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_DISABLED, GUI_WHITE);
	LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_UNSEL, GUI_BLACK);
	LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_SEL, GUI_BLACK);
	LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_SELFOCUS, GUI_BLACK);
	LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_DISABLED, GUI_BLACK);
	LISTVIEW_SetDefaultGridColor(GUI_WHITE);

	MULTIPAGE_SKINFLEX_PROPS MultipageSkinProps;
	MultipageSkinProps.BkColor = GUI_WHITE;
	MultipageSkinProps.FrameColor = GUI_WHITE;
	MultipageSkinProps.TextColor = GUI_BLUE;
	MultipageSkinProps.aBkLower[0] = GUI_GRAY;
	MultipageSkinProps.aBkLower[1] = GUI_GRAY;
	MultipageSkinProps.aBkUpper[0] = GUI_GRAY;
	MultipageSkinProps.aBkUpper[1] = GUI_GRAY;
	MULTIPAGE_SetSkinFlexProps(&MultipageSkinProps, MULTIPAGE_SKINFLEX_PI_SELECTED);
	MultipageSkinProps.TextColor = GUI_BLACK;
	MULTIPAGE_SetSkinFlexProps(&MultipageSkinProps, MULTIPAGE_SKINFLEX_PI_ENABLED);
	MultipageSkinProps.TextColor = GUI_BLACK;
	MULTIPAGE_SetSkinFlexProps(&MultipageSkinProps, MULTIPAGE_SKINFLEX_PI_DISABLED);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	MULTIPAGE_SetDefaultBkColor(GUI_WHITE, 1);
	MULTIPAGE_SetDefaultBkColor(GUI_BLUE, 0);
	MULTIPAGE_SetDefaultTextColor(GUI_BLUE, 1);
	MULTIPAGE_SetDefaultTextColor(GUI_WHITE, 0);

	BUTTON_SKINFLEX_PROPS ButtonSkinProps;
	ButtonSkinProps.Radius = 4;
	ButtonSkinProps.aColorFrame[0] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorFrame[1] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorFrame[2] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorUpper[0] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorUpper[1] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorLower[0] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorLower[1] = GUI_LIGHTGRAY;
	BUTTON_SetSkinFlexProps(&ButtonSkinProps, BUTTON_SKINFLEX_PI_ENABLED);
	BUTTON_SetSkinFlexProps(&ButtonSkinProps, BUTTON_SKINFLEX_PI_FOCUSSED);
	BUTTON_SetSkinFlexProps(&ButtonSkinProps, BUTTON_SKINFLEX_PI_DISABLED);
	BUTTON_SetSkinFlexProps(&ButtonSkinProps, BUTTON_SKINFLEX_PI_PRESSED);
	BUTTON_SetDefaultTextColor(GUI_BLACK, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultTextColor(GUI_WHITE, BUTTON_CI_PRESSED);
	BUTTON_SetDefaultTextColor(GUI_BLACK, BUTTON_CI_DISABLED);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);

	FRAMEWIN_SKINFLEX_PROPS framewin_skin;
	framewin_skin.aColorFrame[0] = GUI_BLUE;
	framewin_skin.aColorFrame[1] = GUI_BLUE;
	framewin_skin.aColorFrame[2] = GUI_BLUE;
	framewin_skin.aColorTitle[0] = GUI_BLUE;
	framewin_skin.aColorTitle[1] = GUI_BLUE;
	framewin_skin.Radius = 0;
	framewin_skin.SpaceX = 0;
	framewin_skin.BorderSizeB = 0;
	framewin_skin.BorderSizeT = 0;
	framewin_skin.BorderSizeR = 0;
	framewin_skin.BorderSizeL = 0;
	FRAMEWIN_SetSkinFlexProps(&framewin_skin, FRAMEWIN_SKINFLEX_PI_ACTIVE);
	FRAMEWIN_SetSkinFlexProps(&framewin_skin, FRAMEWIN_SKINFLEX_PI_INACTIVE);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	FRAMEWIN_SetDefaultClientColor(GUI_WHITE);

	RADIO_SKINFLEX_PROPS RadiobuttonSkinProps;
	RadiobuttonSkinProps.ButtonSize = 20;
	RadiobuttonSkinProps.aColorButton[0] = GUI_BLUE;
	RadiobuttonSkinProps.aColorButton[1] = GUI_WHITE;
	RadiobuttonSkinProps.aColorButton[2] = GUI_WHITE;
	RadiobuttonSkinProps.aColorButton[3] = GUI_BLUE;
	RADIO_SetSkinFlexProps(&RadiobuttonSkinProps, RADIO_SKINFLEX_PI_PRESSED);
	RadiobuttonSkinProps.aColorButton[3] = GUI_WHITE;
	RADIO_SetSkinFlexProps(&RadiobuttonSkinProps, RADIO_SKINFLEX_PI_UNPRESSED);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	RADIO_SetDefaultTextColor(GUI_BLACK);
	RADIO_SetDefaultFocusColor(GUI_WHITE);

	CHECKBOX_SKINFLEX_PROPS CheckboxSkinProps;
	CheckboxSkinProps.ButtonSize = 10;
	CheckboxSkinProps.ColorCheck = GUI_BLUE;
	CheckboxSkinProps.aColorFrame[0] = GUI_BLUE;
	CheckboxSkinProps.aColorFrame[1] = GUI_WHITE;
	CheckboxSkinProps.aColorFrame[2] = GUI_WHITE;
	CheckboxSkinProps.aColorInner[0] = GUI_WHITE;
	CheckboxSkinProps.aColorInner[1] = GUI_WHITE;
	CHECKBOX_SetSkinFlexProps(&CheckboxSkinProps, CHECKBOX_SKINFLEX_PI_ENABLED);
	CHECKBOX_SetSkinFlexProps(&CheckboxSkinProps, CHECKBOX_SKINFLEX_PI_DISABLED);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);

	DROPDOWN_SKINFLEX_PROPS DropdownSkinProps;
	DropdownSkinProps.Radius = 0;
	DropdownSkinProps.ColorArrow = GUI_BLACK;
	DropdownSkinProps.aColorFrame[0] = GUI_BLUE;
	DropdownSkinProps.aColorFrame[1] = GUI_BLUE;
	DropdownSkinProps.aColorFrame[2] = GUI_BLUE;
	DropdownSkinProps.ColorSep = GUI_WHITE;
	DropdownSkinProps.ColorText = GUI_BLACK;
	DropdownSkinProps.aColorUpper[0] = GUI_WHITE;
	DropdownSkinProps.aColorUpper[1] = GUI_WHITE;
	DropdownSkinProps.aColorLower[0] = GUI_WHITE;
	DropdownSkinProps.aColorLower[1] = GUI_WHITE;
	DROPDOWN_SetSkinFlexProps(&DropdownSkinProps, DROPDOWN_SKINFLEX_PI_DISABLED);
	DROPDOWN_SetSkinFlexProps(&DropdownSkinProps, DROPDOWN_SKINFLEX_PI_EXPANDED);
	DROPDOWN_SetSkinFlexProps(&DropdownSkinProps, DROPDOWN_SKINFLEX_PI_FOCUSED);
	DROPDOWN_SetSkinFlexProps(&DropdownSkinProps, DROPDOWN_SKINFLEX_PI_ENABLED);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	DROPDOWN_SetDefaultBkColor(DROPDOWN_CI_UNSEL, GUI_WHITE);
	DROPDOWN_SetDefaultBkColor(DROPDOWN_CI_SEL, GUI_BLUE);
	DROPDOWN_SetDefaultBkColor(DROPDOWN_CI_SELFOCUS, GUI_BLUE);

	CALENDAR_SKINFLEX_PROPS CalendarSkinProps;
	CalendarSkinProps.ColorArrow = GUI_BLACK;
	CalendarSkinProps.aColorFrame[0] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorFrame[1] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorFrame[2] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorLower[0] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorLower[1] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorUpper[0] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorUpper[1] = GUI_LIGHTGRAY;
	CALENDAR_SetSkinFlexProps(&CalendarSkinProps, 0);
	CALENDAR_SetSkinFlexProps(&CalendarSkinProps, 1);
	CALENDAR_SetSkinFlexProps(&CalendarSkinProps, 2);
	CALENDAR_SetDefaultSize(CALENDAR_SI_HEADER, 35);
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_X, 40);
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_Y, 30);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_WEEKEND, GUI_LIGHTBLUE);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_WEEKDAY, GUI_WHITE);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_SEL, GUI_BLUE);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_HEADER, GUI_BLUE);
	CALENDAR_SetDefaultColor(CALENDAR_CI_WEEKEND, GUI_BLACK);
	CALENDAR_SetDefaultColor(CALENDAR_CI_WEEKDAY, GUI_BLACK);
	CALENDAR_SetDefaultColor(CALENDAR_CI_HEADER, GUI_WHITE);
	CALENDAR_SetDefaultColor(CALENDAR_CI_SEL, GUI_WHITE);
	CALENDAR_SetDefaultColor(CALENDAR_CI_FRAME, GUI_BLUE);
	CALENDAR_SetDefaultColor(CALENDAR_CI_LABEL, GUI_BLACK);

	SCROLLBAR_SKINFLEX_PROPS ScrollbarSkinProps;
	ScrollbarSkinProps.ColorArrow = GUI_BLACK;
	ScrollbarSkinProps.ColorGrasp = GUI_BLUE;
	ScrollbarSkinProps.aColorFrame[0] = GUI_BLUE;
	ScrollbarSkinProps.aColorFrame[1] = GUI_BLUE;
	ScrollbarSkinProps.aColorFrame[2] = GUI_BLUE;
	ScrollbarSkinProps.aColorLower[0] = GUI_WHITE;
	ScrollbarSkinProps.aColorLower[1] = GUI_WHITE;
	ScrollbarSkinProps.aColorShaft[0] = GUI_WHITE;
	ScrollbarSkinProps.aColorShaft[1] = GUI_WHITE;
	ScrollbarSkinProps.aColorUpper[0] = GUI_WHITE;
	ScrollbarSkinProps.aColorUpper[1] = GUI_WHITE;
	SCROLLBAR_SetSkinFlexProps(&ScrollbarSkinProps, SCROLLBAR_SKINFLEX_PI_PRESSED);
	SCROLLBAR_SetSkinFlexProps(&ScrollbarSkinProps, SCROLLBAR_SKINFLEX_PI_UNPRESSED);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SCROLLBAR_SetDefaultWidth(35);

	PROGBAR_SKINFLEX_PROPS ProgressbarSkinProps;
	ProgressbarSkinProps.ColorText = GUI_BLACK;
	ProgressbarSkinProps.ColorFrame = GUI_WHITE;
	ProgressbarSkinProps.aColorLowerR[0] = GUI_WHITE;
	ProgressbarSkinProps.aColorLowerR[1] = GUI_WHITE;
	ProgressbarSkinProps.aColorUpperR[0] = GUI_WHITE;
	ProgressbarSkinProps.aColorUpperR[1] = GUI_WHITE;
	ProgressbarSkinProps.aColorLowerL[0] = GUI_BLUE;
	ProgressbarSkinProps.aColorLowerL[1] = GUI_BLUE;
	ProgressbarSkinProps.aColorUpperL[0] = GUI_BLUE;
	ProgressbarSkinProps.aColorUpperL[1] = GUI_BLUE;
	PROGBAR_SetSkinFlexProps(&ProgressbarSkinProps, 0);
	PROGBAR_SetDefaultSkinClassic();

	SLIDER_SKINFLEX_PROPS SliderSkinProps;
	SliderSkinProps.ColorFocus = GUI_WHITE;
	SliderSkinProps.ColorTick = GUI_BLACK;
	SliderSkinProps.TickSize = 4;
	SliderSkinProps.ShaftSize = 0;
	SliderSkinProps.aColorFrame[0] = GUI_BLUE;
	SliderSkinProps.aColorFrame[1] = GUI_BLUE;
	SliderSkinProps.aColorInner[0] = GUI_BLUE;
	SliderSkinProps.aColorInner[1] = GUI_BLUE;
	SliderSkinProps.aColorShaft[0] = GUI_WHITE;
	SliderSkinProps.aColorShaft[1] = GUI_WHITE;
	SliderSkinProps.aColorShaft[2] = GUI_WHITE;
	SLIDER_SetSkinFlexProps(&SliderSkinProps, SLIDER_SKINFLEX_PI_PRESSED);
	SLIDER_SetSkinFlexProps(&SliderSkinProps, SLIDER_SKINFLEX_PI_UNPRESSED);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);

	HEADER_SKINFLEX_PROPS HeaderSkinProps;
	HeaderSkinProps.aColorFrame[0] = GUI_WHITE;
	HeaderSkinProps.aColorFrame[1] = GUI_WHITE;
	HeaderSkinProps.ColorArrow = GUI_BLUE;
	HeaderSkinProps.aColorLower[0] = GUI_WHITE;
	HeaderSkinProps.aColorLower[1] = GUI_WHITE;
	HeaderSkinProps.aColorUpper[0] = GUI_WHITE;
	HeaderSkinProps.aColorUpper[1] = GUI_WHITE;
	HEADER_SetSkinFlexProps(&HeaderSkinProps,0);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	HEADER_SetDefaultTextColor(GUI_BLUE);

	TREEVIEW_SetDefaultBkColor(TREEVIEW_CI_UNSEL, GUI_WHITE);
	TREEVIEW_SetDefaultTextColor(TREEVIEW_CI_UNSEL, GUI_BLACK);
	TREEVIEW_SetDefaultBkColor(TREEVIEW_CI_SEL, GUI_BLUE);
	TREEVIEW_SetDefaultTextColor(TREEVIEW_CI_SEL, GUI_WHITE);

	EDIT_SetDefaultBkColor(EDIT_CI_DISABLED, GUI_WHITE);
	EDIT_SetDefaultBkColor(EDIT_CI_ENABLED, GUI_WHITE);
	EDIT_SetDefaultTextColor(EDIT_CI_DISABLED, GUI_BLACK);
	EDIT_SetDefaultTextColor(EDIT_CI_ENABLED, GUI_BLACK);
	TEXT_SetDefaultTextColor(GUI_BLACK);
	WINDOW_SetDefaultBkColor(GUI_WHITE);
}

void SKINS_SetDarkSkin(void)
{
	WIDGET_EFFECT_Simple_SetColor(0, GUI_BLACK);
	WIDGET_SetDefaultEffect(&WIDGET_Effect_None);
	WM_SetDesktopColor(GUI_BLACK);
	GUI_SetBkColor(GUI_BLACK);

	SKINS_SetDefaultFonts();

	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_UNSEL, GUI_BLACK);
	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SEL, GUI_BLACK);
	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SELFOCUS, GUI_BLACK);
	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_DISABLED, GUI_BLACK);
	LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_UNSEL, GUI_WHITE);
	LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_SEL, GUI_WHITE);
	LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_SELFOCUS, GUI_WHITE);
	LISTVIEW_SetDefaultTextColor(LISTVIEW_CI_DISABLED, GUI_WHITE);
	LISTVIEW_SetDefaultGridColor(GUI_BLACK);

	MULTIPAGE_SKINFLEX_PROPS MultipageSkinProps;
	MultipageSkinProps.BkColor = GUI_BLACK;
	MultipageSkinProps.FrameColor = GUI_BLACK;
	MultipageSkinProps.TextColor = GUI_BLUE;
	MultipageSkinProps.aBkLower[0] = GUI_GRAY;
	MultipageSkinProps.aBkLower[1] = GUI_GRAY;
	MultipageSkinProps.aBkUpper[0] = GUI_GRAY;
	MultipageSkinProps.aBkUpper[1] = GUI_GRAY;
	MULTIPAGE_SetSkinFlexProps(&MultipageSkinProps, MULTIPAGE_SKINFLEX_PI_SELECTED);
	MultipageSkinProps.TextColor = GUI_BLACK;
	MULTIPAGE_SetSkinFlexProps(&MultipageSkinProps, MULTIPAGE_SKINFLEX_PI_ENABLED);
	MultipageSkinProps.TextColor = GUI_BLACK;
	MULTIPAGE_SetSkinFlexProps(&MultipageSkinProps, MULTIPAGE_SKINFLEX_PI_DISABLED);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	MULTIPAGE_SetDefaultBkColor(GUI_BLACK, 1);
	MULTIPAGE_SetDefaultBkColor(GUI_BLUE, 0);
	MULTIPAGE_SetDefaultTextColor(GUI_BLUE, 1);
	MULTIPAGE_SetDefaultTextColor(GUI_BLACK, 0);

	BUTTON_SKINFLEX_PROPS ButtonSkinProps;
	ButtonSkinProps.Radius = 4;
	ButtonSkinProps.aColorFrame[0] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorFrame[1] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorFrame[2] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorUpper[0] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorUpper[1] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorLower[0] = GUI_LIGHTGRAY;
	ButtonSkinProps.aColorLower[1] = GUI_LIGHTGRAY;
	BUTTON_SetSkinFlexProps(&ButtonSkinProps, BUTTON_SKINFLEX_PI_ENABLED);
	BUTTON_SetSkinFlexProps(&ButtonSkinProps, BUTTON_SKINFLEX_PI_FOCUSSED);
	BUTTON_SetSkinFlexProps(&ButtonSkinProps, BUTTON_SKINFLEX_PI_DISABLED);
	BUTTON_SetSkinFlexProps(&ButtonSkinProps, BUTTON_SKINFLEX_PI_PRESSED);
	BUTTON_SetDefaultTextColor(GUI_BLACK, BUTTON_CI_UNPRESSED);
	BUTTON_SetDefaultTextColor(GUI_WHITE, BUTTON_CI_PRESSED);
	BUTTON_SetDefaultTextColor(GUI_BLACK, BUTTON_CI_DISABLED);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);

	FRAMEWIN_SKINFLEX_PROPS framewin_skin;
	framewin_skin.aColorFrame[0] = GUI_BLUE;
	framewin_skin.aColorFrame[1] = GUI_BLUE;
	framewin_skin.aColorFrame[2] = GUI_BLUE;
	framewin_skin.aColorTitle[0] = GUI_BLUE;
	framewin_skin.aColorTitle[1] = GUI_BLUE;
	framewin_skin.Radius = 0;
	framewin_skin.SpaceX = 0;
	framewin_skin.BorderSizeB = 0;
	framewin_skin.BorderSizeT = 0;
	framewin_skin.BorderSizeR = 0;
	framewin_skin.BorderSizeL = 0;
	FRAMEWIN_SetSkinFlexProps(&framewin_skin, FRAMEWIN_SKINFLEX_PI_ACTIVE);
	FRAMEWIN_SetSkinFlexProps(&framewin_skin, FRAMEWIN_SKINFLEX_PI_INACTIVE);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	FRAMEWIN_SetDefaultClientColor(GUI_BLACK);

	RADIO_SKINFLEX_PROPS RadiobuttonSkinProps;
	RadiobuttonSkinProps.ButtonSize = 20;
	RadiobuttonSkinProps.aColorButton[0] = GUI_BLUE;
	RadiobuttonSkinProps.aColorButton[1] = GUI_BLACK;
	RadiobuttonSkinProps.aColorButton[2] = GUI_BLACK;
	RadiobuttonSkinProps.aColorButton[3] = GUI_BLUE;
	RADIO_SetSkinFlexProps(&RadiobuttonSkinProps, RADIO_SKINFLEX_PI_PRESSED);
	RadiobuttonSkinProps.aColorButton[3] = GUI_BLACK;
	RADIO_SetSkinFlexProps(&RadiobuttonSkinProps, RADIO_SKINFLEX_PI_UNPRESSED);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	RADIO_SetDefaultTextColor(GUI_WHITE);
	RADIO_SetDefaultFocusColor(GUI_BLACK);

	CHECKBOX_SKINFLEX_PROPS CheckboxSkinProps;
	CheckboxSkinProps.ButtonSize = 10;
	CheckboxSkinProps.ColorCheck = GUI_BLUE;
	CheckboxSkinProps.aColorFrame[0] = GUI_BLUE;
	CheckboxSkinProps.aColorFrame[1] = GUI_BLACK;
	CheckboxSkinProps.aColorFrame[2] = GUI_BLACK;
	CheckboxSkinProps.aColorInner[0] = GUI_BLACK;
	CheckboxSkinProps.aColorInner[1] = GUI_BLACK;
	CHECKBOX_SetSkinFlexProps(&CheckboxSkinProps, CHECKBOX_SKINFLEX_PI_ENABLED);
	CHECKBOX_SetSkinFlexProps(&CheckboxSkinProps, CHECKBOX_SKINFLEX_PI_DISABLED);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);

	DROPDOWN_SKINFLEX_PROPS DropdownSkinProps;
	DropdownSkinProps.Radius = 0;
	DropdownSkinProps.ColorArrow = GUI_WHITE;
	DropdownSkinProps.aColorFrame[0] = GUI_BLUE;
	DropdownSkinProps.aColorFrame[1] = GUI_BLUE;
	DropdownSkinProps.aColorFrame[2] = GUI_BLUE;
	DropdownSkinProps.ColorSep = GUI_BLACK;
	DropdownSkinProps.ColorText = GUI_WHITE;
	DropdownSkinProps.aColorUpper[0] = GUI_BLACK;
	DropdownSkinProps.aColorUpper[1] = GUI_BLACK;
	DropdownSkinProps.aColorLower[0] = GUI_BLACK;
	DropdownSkinProps.aColorLower[1] = GUI_BLACK;
	DROPDOWN_SetSkinFlexProps(&DropdownSkinProps, DROPDOWN_SKINFLEX_PI_DISABLED);
	DROPDOWN_SetSkinFlexProps(&DropdownSkinProps, DROPDOWN_SKINFLEX_PI_EXPANDED);
	DROPDOWN_SetSkinFlexProps(&DropdownSkinProps, DROPDOWN_SKINFLEX_PI_FOCUSED);
	DROPDOWN_SetSkinFlexProps(&DropdownSkinProps, DROPDOWN_SKINFLEX_PI_ENABLED);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	DROPDOWN_SetDefaultBkColor(DROPDOWN_CI_UNSEL, GUI_BLACK);
	DROPDOWN_SetDefaultBkColor(DROPDOWN_CI_SEL, GUI_BLUE);
	DROPDOWN_SetDefaultBkColor(DROPDOWN_CI_SELFOCUS, GUI_BLUE);

	CALENDAR_SKINFLEX_PROPS CalendarSkinProps;
	CalendarSkinProps.ColorArrow = GUI_BLACK;
	CalendarSkinProps.aColorFrame[0] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorFrame[1] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorFrame[2] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorLower[0] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorLower[1] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorUpper[0] = GUI_LIGHTGRAY;
	CalendarSkinProps.aColorUpper[1] = GUI_LIGHTGRAY;
	CALENDAR_SetSkinFlexProps(&CalendarSkinProps, 0);
	CALENDAR_SetSkinFlexProps(&CalendarSkinProps, 1);
	CALENDAR_SetSkinFlexProps(&CalendarSkinProps, 2);
	CALENDAR_SetDefaultSize(CALENDAR_SI_HEADER, 35);
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_X, 40);
	CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_Y, 30);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_WEEKEND, GUI_LIGHTBLUE);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_WEEKDAY, GUI_WHITE);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_SEL, GUI_BLUE);
	CALENDAR_SetDefaultBkColor(CALENDAR_CI_HEADER, GUI_BLUE);
	CALENDAR_SetDefaultColor(CALENDAR_CI_WEEKEND, GUI_BLACK);
	CALENDAR_SetDefaultColor(CALENDAR_CI_WEEKDAY, GUI_BLACK);
	CALENDAR_SetDefaultColor(CALENDAR_CI_HEADER, GUI_WHITE);
	CALENDAR_SetDefaultColor(CALENDAR_CI_SEL, GUI_WHITE);
	CALENDAR_SetDefaultColor(CALENDAR_CI_FRAME, GUI_BLUE);
	CALENDAR_SetDefaultColor(CALENDAR_CI_LABEL, GUI_BLACK);

	SCROLLBAR_SKINFLEX_PROPS ScrollbarSkinProps;
	ScrollbarSkinProps.ColorArrow = GUI_WHITE;
	ScrollbarSkinProps.ColorGrasp = GUI_BLUE;
	ScrollbarSkinProps.aColorFrame[0] = GUI_BLUE;
	ScrollbarSkinProps.aColorFrame[1] = GUI_BLUE;
	ScrollbarSkinProps.aColorFrame[2] = GUI_BLUE;
	ScrollbarSkinProps.aColorLower[0] = GUI_BLACK;
	ScrollbarSkinProps.aColorLower[1] = GUI_BLACK;
	ScrollbarSkinProps.aColorShaft[0] = GUI_BLACK;
	ScrollbarSkinProps.aColorShaft[1] = GUI_BLACK;
	ScrollbarSkinProps.aColorUpper[0] = GUI_BLACK;
	ScrollbarSkinProps.aColorUpper[1] = GUI_BLACK;
	SCROLLBAR_SetSkinFlexProps(&ScrollbarSkinProps, SCROLLBAR_SKINFLEX_PI_PRESSED);
	SCROLLBAR_SetSkinFlexProps(&ScrollbarSkinProps, SCROLLBAR_SKINFLEX_PI_UNPRESSED);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SCROLLBAR_SetDefaultWidth(35);

	PROGBAR_SKINFLEX_PROPS ProgressbarSkinProps;
	ProgressbarSkinProps.ColorText = GUI_WHITE;
	ProgressbarSkinProps.ColorFrame = GUI_BLACK;
	ProgressbarSkinProps.aColorLowerR[0] = GUI_BLACK;
	ProgressbarSkinProps.aColorLowerR[1] = GUI_BLACK;
	ProgressbarSkinProps.aColorUpperR[0] = GUI_BLACK;
	ProgressbarSkinProps.aColorUpperR[1] = GUI_BLACK;
	ProgressbarSkinProps.aColorLowerL[0] = GUI_BLUE;
	ProgressbarSkinProps.aColorLowerL[1] = GUI_BLUE;
	ProgressbarSkinProps.aColorUpperL[0] = GUI_BLUE;
	ProgressbarSkinProps.aColorUpperL[1] = GUI_BLUE;
	PROGBAR_SetSkinFlexProps(&ProgressbarSkinProps, 0);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);

	SLIDER_SKINFLEX_PROPS SliderSkinProps;
	SliderSkinProps.ColorFocus = GUI_BLACK;
	SliderSkinProps.ColorTick = GUI_WHITE;
	SliderSkinProps.TickSize = 4;
	SliderSkinProps.ShaftSize = 0;
	SliderSkinProps.aColorFrame[0] = GUI_BLUE;
	SliderSkinProps.aColorFrame[1] = GUI_BLUE;
	SliderSkinProps.aColorInner[0] = GUI_BLUE;
	SliderSkinProps.aColorInner[1] = GUI_BLUE;
	SliderSkinProps.aColorShaft[0] = GUI_BLACK;
	SliderSkinProps.aColorShaft[1] = GUI_BLACK;
	SliderSkinProps.aColorShaft[2] = GUI_BLACK;
	SLIDER_SetSkinFlexProps(&SliderSkinProps, SLIDER_SKINFLEX_PI_PRESSED);
	SLIDER_SetSkinFlexProps(&SliderSkinProps, SLIDER_SKINFLEX_PI_UNPRESSED);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);

	HEADER_SKINFLEX_PROPS HeaderSkinProps;
	HeaderSkinProps.aColorFrame[0] = GUI_BLACK;
	HeaderSkinProps.aColorFrame[1] = GUI_BLACK;
	HeaderSkinProps.ColorArrow = GUI_BLUE;
	HeaderSkinProps.aColorLower[0] = GUI_BLACK;
	HeaderSkinProps.aColorLower[1] = GUI_BLACK;
	HeaderSkinProps.aColorUpper[0] = GUI_BLACK;
	HeaderSkinProps.aColorUpper[1] = GUI_BLACK;
	HEADER_SetSkinFlexProps(&HeaderSkinProps,0);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	HEADER_SetDefaultTextColor(GUI_BLUE);

	TREEVIEW_SetDefaultBkColor(TREEVIEW_CI_UNSEL, GUI_BLACK);
	TREEVIEW_SetDefaultTextColor(TREEVIEW_CI_UNSEL, GUI_WHITE);
	TREEVIEW_SetDefaultBkColor(TREEVIEW_CI_SEL, GUI_BLUE);
	TREEVIEW_SetDefaultTextColor(TREEVIEW_CI_SEL, GUI_WHITE);

	EDIT_SetDefaultBkColor(EDIT_CI_DISABLED, GUI_BLACK);
	EDIT_SetDefaultBkColor(EDIT_CI_ENABLED, GUI_BLACK);

	EDIT_SetDefaultTextColor(EDIT_CI_DISABLED, GUI_WHITE);
	EDIT_SetDefaultTextColor(EDIT_CI_ENABLED, GUI_WHITE);
	TEXT_SetDefaultTextColor(GUI_WHITE);
	WINDOW_SetDefaultBkColor(GUI_BLACK);
}

void SKINS_UpdateCalendar(void)
{
	for (int i = 0; i < 12; i++)
		months[i] = GUI_LANG_GetText(132 + i);
	CALENDAR_SetDefaultMonths(pMonths);

	for (int i = 0; i < 7; i++)
		days[i] = GUI_LANG_GetText(144 + i);
	CALENDAR_SetDefaultDays(pDays);

}

void MULTIEDIT_SetSkin(MULTIEDIT_HANDLE hObj, unsigned Index)
{
	if (SkinStatus)
	{
		MULTIEDIT_SetTextColor(hObj, Index, GUI_WHITE);
		MULTIEDIT_SetBkColor(hObj, Index, GUI_BLACK);
	}
	else
	{
		MULTIEDIT_SetTextColor(hObj, Index, GUI_BLACK);
		MULTIEDIT_SetBkColor(hObj, Index, GUI_WHITE);
	}
}

void EDIT_SetSkin(EDIT_Handle hObj, unsigned Index)
{
	if (SkinStatus)
	{
		EDIT_SetTextColor(hObj, Index, GUI_WHITE);
		EDIT_SetBkColor(hObj, Index, GUI_BLACK);
	}
	else
	{
		EDIT_SetTextColor(hObj, Index, GUI_BLACK);
		EDIT_SetBkColor(hObj, Index, GUI_WHITE);
	}
}

void DROPDOWN_SetTextSkin(DROPDOWN_Handle hObj)
{
	DROPDOWN_SetBkColor(hObj, DROPDOWN_CI_SEL, GUI_BLUE);
	DROPDOWN_SetTextColor(hObj, DROPDOWN_CI_SEL, GUI_WHITE);
	DROPDOWN_SetBkColor(hObj, DROPDOWN_CI_SELFOCUS, GUI_BLUE);
	DROPDOWN_SetTextColor(hObj, DROPDOWN_CI_SELFOCUS, GUI_WHITE);

	if (SkinStatus)
	{
		DROPDOWN_SetTextColor(hObj, DROPDOWN_CI_UNSEL, GUI_WHITE);
		DROPDOWN_SetBkColor(hObj, DROPDOWN_CI_UNSEL, GUI_BLACK);
	}
	else
	{
		DROPDOWN_SetTextColor(hObj, DROPDOWN_CI_UNSEL, GUI_BLACK);
		DROPDOWN_SetBkColor(hObj, DROPDOWN_CI_UNSEL, GUI_WHITE);
	}
}

void TREEVIEW_SetImagesSkin(TREEVIEW_Handle hObj)
{
	if (SkinStatus)
	{
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_OPEN, &bmdirDark);
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_CLOSED, &bmdirDark);
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_PLUS, &bmDirCloseDark);
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_MINUS, &bmDirOpenDark);
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_LEAF, &bmFileDark);

	}
	else
	{
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_OPEN, &bmdirLight);
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_CLOSED, &bmdirLight);
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_PLUS, &bmDirCloseLight);
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_MINUS, &bmDirOpenLight);
		TREEVIEW_SetImage(hObj, TREEVIEW_BI_LEAF, &bmFileLight);
	}
}

void ICONVIEW_SetSkinColors(ICONVIEW_Handle hObj)
{
	if (SkinStatus)
	{
		ICONVIEW_SetTextColor(hObj, ICONVIEW_CI_BK, GUI_WHITE);
		ICONVIEW_SetTextColor(hObj, ICONVIEW_CI_SEL, GUI_WHITE);
		ICONVIEW_SetBkColor(hObj, ICONVIEW_CI_BK, GUI_BLACK);
		ICONVIEW_SetBkColor(hObj, ICONVIEW_CI_SEL, GUI_BLACK);
	}
	else
	{
		ICONVIEW_SetTextColor(hObj, ICONVIEW_CI_BK, GUI_BLACK);
		ICONVIEW_SetTextColor(hObj, ICONVIEW_CI_SEL, GUI_BLACK);
		ICONVIEW_SetBkColor(hObj, ICONVIEW_CI_BK, GUI_WHITE);
		ICONVIEW_SetBkColor(hObj, ICONVIEW_CI_SEL, GUI_WHITE);
	}
}

void LISTWHEEL_SetSkinColors(LISTWHEEL_Handle hObj)
{
	if (SkinStatus)
	{
		LISTWHEEL_SetTextColor(hObj, LISTWHEEL_CI_UNSEL, GUI_WHITE);
		LISTWHEEL_SetTextColor(hObj, LISTWHEEL_CI_SEL, GUI_WHITE);
		LISTWHEEL_SetBkColor(hObj, LISTWHEEL_CI_UNSEL, GUI_BLACK);
		LISTWHEEL_SetBkColor(hObj, LISTWHEEL_CI_SEL, GUI_RED);
	}
	else
	{
		LISTWHEEL_SetTextColor(hObj, LISTWHEEL_CI_UNSEL, GUI_BLACK);
		LISTWHEEL_SetTextColor(hObj, LISTWHEEL_CI_SEL, GUI_WHITE);
		LISTWHEEL_SetBkColor(hObj, LISTWHEEL_CI_UNSEL, GUI_WHITE);
		LISTWHEEL_SetBkColor(hObj, LISTWHEEL_CI_SEL, GUI_RED);
	}
}

GUI_COLOR SKINS_GetTextColor(void)
{
	if (SkinStatus)
		return GUI_WHITE;
	else
		return GUI_BLACK;
}

GUI_COLOR SKINS_GetBkColor(void)
{
	if (SkinStatus)
		return GUI_BLACK;
	else
		return GUI_WHITE;
}

void GRAPH_SetSkinColors(GRAPH_Handle hObj)
{
	if (SkinStatus)
	{
		GRAPH_SetColor(hObj, GUI_BLACK, GRAPH_CI_BK);
		GRAPH_SetColor(hObj, GUI_BLACK, GRAPH_CI_BORDER);
	}
	else
	{
		GRAPH_SetColor(hObj, GUI_WHITE, GRAPH_CI_BK);
		GRAPH_SetColor(hObj, GUI_WHITE, GRAPH_CI_BORDER);
	}
}

void SKINS_InitGraphics(void)
{
	SkinStatus = FRAM_Read(SKIN_ADDR);

	if (SkinStatus)
	{
		SKINS_SetDarkSkin();
		loadTitleBarWindowGraphicsToRAM();
		loadMenuBarWindowGrapihicsToRAM();
		loadSettingsWindowDarkGraphicsToRAM();
		loadStarUpWindowGraphicsToRAM();
	}
	else
	{
		SKINS_SetLightSkin();
		loadTitleBarWindowGraphicsToRAM();
		loadMenuBarWindowGrapihicsToRAM();
		loadSettingsWindowLightGraphicsToRAM();
		loadStarUpWindowGraphicsToRAM();
	}
}

void SKINS_ChangeGraphics(void)
{
	if (SkinStatus)
	{
		SkinStatus = 0;
		SKINS_SetLightSkin();
		loadSettingsWindowLightGraphicsToRAM();
		FRAM_Write(SKIN_ADDR, (uint8_t) SkinStatus);
	}
	else
	{
		SkinStatus = 1;
		SKINS_SetDarkSkin();
		loadSettingsWindowDarkGraphicsToRAM();
		FRAM_Write(SKIN_ADDR, (uint8_t) SkinStatus);
	}
}

char SKINS_GetStatus(void)
{
	return SkinStatus;
}

void SKINS_SetStatus(char newStatus)
{
	SkinStatus = newStatus;
	SKINS_ChangeGraphics();
}
