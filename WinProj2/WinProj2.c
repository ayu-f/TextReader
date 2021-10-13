#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include"text.h"
#include"view.h"

//void FixVertScroll(view_t* view) {
//	unsigned i;
//
//	// Move scroll so it points to the same string it did before changing mode
//	for (i = 0; i < view->linebreaksLen - 1; ++i)
//		if (view->topSymb >= view->linebreaks[i] && view->topSymb < view->linebreaks[i + 1]) {
//			view->vertScrollPos = i;
//			break;
//		}
//}


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

typedef enum mode {
	FORMATED,
	ORIGINAL
}mode_t;


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
	wincl.lpszMenuName = NULL;                 /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
	/* Use Windows's default colour as the background of the window */
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

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
	mode_t mode = FORMATED;
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

		// Set the vertical scrolling
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = vw.lineCountFormated - 1;
		si.nPage = vw.heightWinInLines;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		break;
	}
	case WM_SIZE: {
		VM_UpdateSize(&vw, LOWORD(lParam), HIWORD(lParam));
		free(vw.data);
		vw.data = NULL;
		VM_ParseText(tx, &vw);

		// Set the vertical scrolling
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = vw.lineCountFormated - 1;
		si.nPage = vw.heightWinInLines;
		//si.nPos = 0;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	}
	case WM_VSCROLL: {
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
			/*thumbPos = HIWORD(wParam), dividedPos;
			if (vw.heightWinInLines > 65535) {
				dividedPos = (double)thumbPos / (double)65535;
				thumbPos = (int)(dividedPos * (double)vw.heightWinInLines);
			}
			si.nPos = thumbPos - si.nPos;*/
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hwnd, SB_VERT, &si);

		// If the position has changed, scroll window and update it.
		if (si.nPos != yPos)
		{
			VM_ShiftVerticalFormated(&vw, yPos - si.nPos);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
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
