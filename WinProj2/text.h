#pragma once
#ifndef _IF_TEXT_INCLUDE
#define _IF_TEXT_INCLUDE
#pragma warning(disable:4996)
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>

#define TRUE 1
#define FALSE 0


typedef struct {
	char* text;
	unsigned int len; // длина текста
}text_t;

int TextReader(text_t* text, const char* file);

#endif // _IF_TEXT_INCLUDE
