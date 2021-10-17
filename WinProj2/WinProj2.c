#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include "text.h"
#include "view.h"
//#include "menu.h"
#include "resource1.h"

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

	switch (message)                  // handle the messages
	{
	case WM_DESTROY:
		PostQuitMessage(0);       // send a WM_QUIT to the message queue
		break;
	case WM_CREATE: {
		CREATESTRUCT* structPar = (CREATESTRUCT*)lParam;

		if (TextReader(&tx, (char*)structPar->lpCreateParams) == FALSE) {
			PostQuitMessage(0);
		}
		VM_ViewInit(tx, &vw, WINH, WINW, hwnd);
		VM_ParseText(tx, &vw);
		hMenu = GetMenu(hwnd);
		CheckMenuItem(hMenu, ID_VIEW_WRAP, MF_CHECKED);
	
		break;
	}
	case WM_SIZE: {
		int xClient = LOWORD(lParam);
		int yClient = HIWORD(lParam);
		VM_UpdateSize(&vw, xClient, yClient);
		free(vw.data);
		vw.data = NULL;
		VM_ParseText(tx, &vw);
		VM_FixVertScrollPos(&vw);
		VM_FixHorScrollPos(&vw);


		// Set the vertical scrolling
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = vw.mode == WRAP ? vw.lineCountFormated - 1 : vw.linesCount;
		si.nPage = vw.heightWinInLines;
		si.nPos = vw.vertScrollPos;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		// Set the horizontal scrolling
		if (vw.mode == NOWRAP) {
			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_PAGE;
			si.nMin = 0;
			si.nMax = vw.maxSymInLine;
			si.nPage = vw.maxSymbCount;
			SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		}

		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	}
	case WM_VSCROLL: {
		//VertScroll(&vw, wParam, hwnd);
		int yPos = 0;
		int thumbPos, dividedPos;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &si);

		// Save the position
		yPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case SB_LINEUP: // top arrow
			si.nPos -= 1;
			break;

		case SB_LINEDOWN: // bottom arrow
			si.nPos += 1;
			break;

		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;

		case SB_THUMBTRACK: // dragged
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hwnd, SB_VERT, &si);
		vw.vertScrollPos += abs(yPos - si.nPos);
		// If the position has changed, scroll window and update it.
		if (si.nPos != yPos)
		{
			if (vw.mode == WRAP)
				VM_ShiftVerticalWrap(&vw, yPos - si.nPos);
			else
				VM_ShiftVerticalNoWRap(&vw, yPos - si.nPos);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}

		break;
	}
	case WM_HSCROLL:
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_HORZ, &si);
		int xPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			si.nPos -= 1;
			break;
		case SB_LINERIGHT:
			si.nPos += 1;
			break;
		case SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hwnd, SB_HORZ, &si);

		// If the position has changed, scroll the window.
		if (si.nPos != xPos)
		{
			VW_ShiftHorizontal(&vw, xPos - si.nPos);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
		break;
	case WM_COMMAND:
		hMenu = GetMenu(hwnd);

		switch (LOWORD(wParam)) {
		case ID_VIEW_WRAP:
			// Switch layout checkbox and layout on/off mode
			if (vw.mode == WRAP) {
				CheckMenuItem(hMenu, ID_VIEW_WRAP, MF_UNCHECKED);
				vw.mode = NOWRAP;
				VM_FixVertScrollPos(&vw);
			}
			else {
				CheckMenuItem(hMenu, ID_VIEW_WRAP, MF_CHECKED);
				VM_FixVertScrollPos(&vw);
				vw.mode = WRAP;
			}
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			break;
		default:
			break;
		}
		break;
	case WM_PAINT: {
		VM_DrawText(hwnd, &vw);
		break;
	}
	default:                      // for messages that we don't deal with
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
