#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#define __CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#include "text.h"
#include "view.h"
//#include "menu.h"
#include "resource1.h"
#include"scroll.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nCmdShow)
{
	HWND hwnd;               /* This is the handle for our window */
	MSG messages;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */

	/* The Window structure */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS | CS_CLASSDC | CS_OWNDC;                 /* Catch double-clicks */
	wincl.cbSize = sizeof(WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);                /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
	/* Use Windows's default colour as the background of the window */
	wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	/* Register the window class, and if it fails quit the program */
	if (!RegisterClassEx(&wincl))
		return 0;

	/* The class is registered, let's create the program*/
	hwnd = CreateWindowEx(
		0,                                /* Extended possibilites for variation */
		szClassName,                      /* Classname */
		_T("Text reader"),                /* Title Text */
		WS_OVERLAPPEDWINDOW | WS_VSCROLL, /* default window with vertical scroll */
		CW_USEDEFAULT,                    /* Windows decides the position */
		CW_USEDEFAULT,                    /* where the window ends up on the screen */
		WINW,                            /* The programs width */
		WINH,                            /* and height in pixels */
		HWND_DESKTOP,                     /* The window is a child-window to desktop */
		NULL,                             /* No menu */
		hThisInstance,                    /* Program Instance handler */
		lpszArgument                      /* The only Window Creation data - filename */
	);

	/* Make the window visible on the screen */
	ShowWindow(hwnd, nCmdShow);

	/* Run the message loop. It will run until GetMessage() returns 0 */
	while (GetMessage(&messages, NULL, 0, 0))
	{
		/* Translate virtual-key messages into character messages */
		TranslateMessage(&messages);
		/* Send message to WindowProcedure */
		DispatchMessage(&messages);
	}

	/* The program return-value is 0 - The value that PostQuitMessage() gave */
	return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static text_t tx;
	static view_t vw;
	static SCROLLINFO si;
	HMENU hMenu;

	switch (message)           // handle the messages
	{
	case WM_DESTROY:
		FreeText(&tx);
		free(vw.data);
		PostQuitMessage(0); 
		_CrtDumpMemoryLeaks();
		break;
	case WM_CREATE: {
		CREATESTRUCT* structPar = (CREATESTRUCT*)lParam;

		if (TextReader(&tx, (char*)structPar->lpCreateParams) == FALSE) {
			PostQuitMessage(0);
		}
		VM_ViewInit(tx, &vw, hwnd);
		VM_ParseText(tx, &vw);
		hMenu = GetMenu(hwnd);
		CheckMenuItem(hMenu, ID_VIEW_WRAP, MF_CHECKED);

		break;
	}
	case WM_SIZE: {
		int xClient = LOWORD(lParam);
		int yClient = HIWORD(lParam);
		VM_UpdateSize(&vw, xClient, yClient);
		VM_RecountLines(&vw);
		if (vw.mode == WRAP) {
			VM_FixVertScrollPos(&vw);
		}
		else {
			VM_FixHorScrollPos(&vw);
		}

		ResizeVertScroll(&vw, hwnd);
		ResizeHorzScroll(&vw, hwnd);

		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	}
	case WM_VSCROLL: {
		VertScroll(&vw, wParam, hwnd);
		break;
	}
	case WM_HSCROLL: {
		HorzScroll(&vw, wParam, hwnd);
		break;
	}
	case WM_COMMAND: {
		OPENFILENAME ofn;
		char filename[MAX_PATH];
		hMenu = GetMenu(hwnd);

		switch (LOWORD(wParam)) {
		case ID_FILE_OPEN: {
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = filename;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(filename);
			ofn.lpstrFilter = "Text Files(*.txt)\0*.txt\0";
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			if (GetOpenFileNameA(&ofn)) {
				FreeText(&tx);
				free(vw.data);
				TextReader(&tx, ofn.lpstrFile);

				if (!(VM_ViewInit(tx, &vw, hwnd) == TRUE && VM_ParseText(tx, &vw) == TRUE)) {
					FreeText(&tx);
					MessageBox(hwnd, _T("Memory Error"), _T("Error"), MB_ICONERROR);
					PostQuitMessage(0);
				}
			}
			SendMessage(hwnd, WM_SIZE, 0, (LPARAM)NULL);
			break;
		}
		case ID_VIEW_WRAP:
			// Switch wrap checkbox and mode in view model
			if (vw.mode == WRAP) {
				CheckMenuItem(hMenu, ID_VIEW_WRAP, MF_UNCHECKED);
				vw.mode = NOWRAP;
				VM_FixVertScrollPos(&vw);
			}
			else {
				CheckMenuItem(hMenu, ID_VIEW_WRAP, MF_CHECKED);
				vw.mode = WRAP;
			}
			SendMessage(hwnd, WM_SIZE, 0, (LPARAM)NULL); // send wm_size
			int tmpVertScrl = vw.vertScrollPos;
			if (vw.vertScrollPosMax > SCROLL_MAX_RANGE) {
				double dividedPos = (double)tmpVertScrl / (double)vw.vertScrollPosMax;
				tmpVertScrl = (int)(dividedPos * (double)SCROLL_MAX_RANGE);
			}
			SetScrollPos(hwnd, SB_VERT, tmpVertScrl, TRUE);
			break;
		case ID_FILE_CLOSE: {
			FreeText(&tx);
			free(vw.data);
			vw.data = NULL;
			SendMessage(hwnd, WM_SIZE, 0, (LPARAM)NULL);
			break;
		}
		case ID_FILE_EXIT: {
			SendMessage(hwnd, WM_DESTROY, 0, (LPARAM)NULL);
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_PAINT: {
		VM_DrawText(hwnd, &vw);
		break;
	}
	default:                      // for messages that we don't deal with
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	
	return 0;
}
