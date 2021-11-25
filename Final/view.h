#ifndef _IF_VIEW_INCLUDE
#define _IF_VIEW_INCLUDE

#define WINH 550
#define WINW 590
#define FONT_SIZE 18
#define SCROLL_MAX_RANGE 65535
#define FONT_WIDTH 9
#define MIN(x, y) x > y ? y : x
#define MAX(x, y) x > y ? x : y

#include "text.h"

// Client window size
typedef struct winSize {
	int w, h;
} winSize_t;

// mode
typedef enum mode {
	WRAP,
	NOWRAP
} mode_t;

// view model
typedef struct view_t {
	// mode WRAP/NOWRAP
	mode_t mode;
	// size of Client Window
	winSize_t winSize;
	// array of pointers to lines of text
	char** data;
	// count lines in text
	unsigned int linesCount;
	// max count of lines in Client window
	unsigned int heightWinInLines;
	// max symbols that can enter in line
	unsigned int maxSymbCount;
	// max symbols that are in line
	unsigned int maxSymInLine;
	// number the first line to draw
	unsigned int firstLine;
	// number of the part of the first string to draw
	long cntSubline;
	// count of lines in formated view in Client window
	unsigned int lineCountFormated;
	// scroll position
	long vertScrollPos, horzScrollPos;
	long vertScrollPosMax, horzScrollPosMax;
	long vertScrollUnit, horzScrollUnit;
}view_t;

/*
BRIEF: init view model, count the numer of strings in text, set font
ARGS:
tx - text model
vw - view model
winH - height of client window
winW - width of client window
hwnd - hadnle to a window
RETURN:
TRUE - if all ok
FALSE - some error
*/
int VM_ViewInit(text_t tx, view_t* vw, HWND hwnd); // init

/*
BRIEF: Separate text to the lines ended \n and count the lines in ClientWindow when mode == wrap
ARGS:
tx - text model
vw - view model
RETURN:
TRUE - if all ok
FALSE - some errors
*/
int VM_ParseText(text_t tx, view_t* vw); // заполнить

/*
BRIEF: Change window Client size in view model
ARGS:
vw - view model
winW - new width of client window
winH - new height of client window
RETURN: None
*/
void VM_UpdateSize(view_t* vw, int winW, int winH);

/*
BRIEF: count the number (in text definitions) of printed lines together with the layout
ARGS:
vw - view model
RETURN: count of lines from the firstLine to firstLine + height of Client window
*/
int VM_CountTotalLinesInWin(view_t* vw);

/*
* BRIEF: handle the vertical scroll with wrap mode, change index firstLine and subline
* ARGS:
* vw - view model
* shift - shift of scroll. shift < 0 - scroll down, shift > 0 - scroll up
* RETURN: None
*/
void VM_ShiftVerticalWrap(view_t* vw, int shift);

/*
* BRIEF: handle the vertical scroll with nowrap mode, change index firstLine and subline
* ARGS:
* vw - view model
* shift - shift of scroll. shift < 0 - scroll down, shift > 0 - scroll up
* RETURN: None
*/
void VM_ShiftVerticalNoWRap(view_t* vw, int shift);

/*
* BRIEF: function that drawing text to client window
* ARGS:
* hwnd - handle to a window
* vw - view model
* RETURN: None
*/
void VM_DrawText(HWND hwnd, view_t* vw);

/*
* BRIEF: count new vertical scroll position
* ARGS:
* vw - view model
* RETURN: None
*/
void VM_FixVertScrollPos(view_t* vw);

/*
* BRIEF:  handle the horizontal scroll with nowrap mode
* ARGS:
* vw - view model
* shift - shift of scroll. shift < 0 - scroll right, shift > 0 - scroll left
* RETURN: None
*/
void VM_ShiftHorizontal(view_t* vw, int shift);

/*
* BRIEF: count new horizontal scroll position
* ARGS:
* hwnd - handle to a window
* vw - view model
* RETURN: None
*/
void VM_FixHorScrollPos(view_t* vw);

/*
* BRIEF: count the number of all lines during layout
* ARGS:
* vw - view model
* RETURN: None
*/
void VM_RecountLines(view_t* vw);

#endif // _IF_VIEW_INCLUDE
