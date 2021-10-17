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

// text model
typedef struct {
	// hold all text
	char* txt;
	// length of text
	unsigned int len; 
}text_t;

/*
* BREIF: Read text from file
* ARGS:
* text - text model
* file - path of file
* RETURN:
* TRUE - if all ok
* FALSE - some errors
*/
int TextReader(text_t* text, const char* file);

#endif // _IF_TEXT_INCLUDE
