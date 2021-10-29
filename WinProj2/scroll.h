#pragma once
#ifndef _IF_SCROLL_INCLUDE
#define _IF_SCROLL_INCLUDE

#include"view.h"

void ResizeVertScroll(view_t* view, HWND hwnd);

void ResizeHorzScroll(view_t* vw, HWND hwnd);

void VertScroll(view_t* vw, WPARAM wParam, HWND hwnd);

void HorzScroll(view_t* vw, WPARAM wParam, HWND hwnd);

#endif // _IF_VIEW_INCLUDE