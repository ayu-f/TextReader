#pragma once
#ifndef _IF_VIEW_INCLUDE
#define _IF_VIEW_INCLUDE

#define WINH 550
#define WINW 590
#define FONT_SIZE 18
#define FONT_WIDTH 9
#define MIN(x, y) x > y ? y : x
#define MAX(x, y) x > y ? x : y

#include "text.h"

typedef struct winSize { // for win size
	int w, h;
} winSize_t;

typedef struct line { // one line
	char* line; // char**
	int lineLen; // -
}line_t;

// view model
typedef struct view_t {
	winSize_t winSize;
	line_t* data; // массив указателей на строки текста 
	unsigned int linesCount, heightWinInLines; // count lines in text, max count of lines in window
	unsigned int maxSymbCount, maxSymInLine; // max symbols that can enter in line, max symbols that are in line

	unsigned int firstLine; // number the first line
	int cntSubline; //  number of the part of the string that is output
	unsigned int lineCountFormated; // count of line in formated view

	// Position of a scroll
	int vertScrollPos, horzScrollPos;
	// Maximum position a scroll can reach
	unsigned vertScrollMaxPos, horzScrollMaxPos;
}view_t;

int VM_ViewInit(text_t tx, view_t* vw, int winH, int winW, HWND hwnd); // init

int VM_ParseText(text_t tx, view_t* vw); // заполнить

void VM_UpdateSize(view_t* vw, int winW, int winH);

int VM_CountTotalLinesInWin(view_t* vw);

void VM_ShiftVerticalFormated(view_t* vw, int shift);

void VM_DrawText(HWND hwnd, view_t* vw);

#endif // _IF_VIEW_INCLUDE
