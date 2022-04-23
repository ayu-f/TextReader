#ifndef _IF_SCROLL_INCLUDE
#define _IF_SCROLL_INCLUDE

#include"view.h"

/*
* BRIEF: Change vertical scroll size (max position, current position)
* ARGS:
* vw - view model
* hwnd - handle to a window
* RETURN: None
*/
void ResizeVertScroll(view_t* view, HWND hwnd);

/*
* BRIEF: Change horizontal scroll size (max position, current position)
* ARGS:
* vw - view model
* hwnd - handle to a window
* RETURN: None
*/
void ResizeHorzScroll(view_t* vw, HWND hwnd);

/*
* BRIEF: Control window vertical scrolling
* ARGS:
* vw - view model
* wParam - scroll value
* hwnd - handle to a window
* RETURN: None
*/
void VertScroll(view_t* vw, WPARAM wParam, HWND hwnd);

/*
* BRIEF: Control window horizontal scrolling
* ARGS:
* vw - view model
* wParam - scroll value
* hwnd - handle to a window
* RETURN: None
*/
void HorzScroll(view_t* vw, WPARAM wParam, HWND hwnd);

/*
* BRIEF: Control of keyboard arrows by virtual keys
* ARGS:
* wParam - scroll value
* hwnd - handle to a window
* RETURN: None
*/
void HandleArrows(WPARAM wParam, HWND hwnd);

/*
* BRIEF: Control of keyboard characters
* ARGS:
* wParam - scroll value
* hwnd - handle to a window
* RETURN: None
*/
void HandleKeys(WPARAM wParam, HWND hwnd);

#endif // _IF_VIEW_INCLUDE
