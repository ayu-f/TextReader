#include"scroll.h"

void ResizeVertScroll(view_t* view, HWND hwnd) {
	if (view->data == NULL) {
		SetScrollRange(hwnd, SB_VERT, 0, 0, FALSE);
		return;
	}
	if (view->mode == WRAP)
		view->vertScrollPosMax = MAX(0, (int)view->lineCountFormated - (int)view->heightWinInLines);
	else
		view->vertScrollPosMax = MAX(0, (int)view->linesCount - (int)view->heightWinInLines);
	view->vertScrollPos = MIN(view->vertScrollPos, view->vertScrollPosMax);

	SetScrollRange(hwnd, SB_VERT, 0, MIN(SCROLL_MAX_RANGE, view->vertScrollPosMax), FALSE);
}

void ResizeHorzScroll(view_t* vw, HWND hwnd) {
	if (vw->mode == WRAP) {
		vw->horzScrollPosMax = 0;
		vw->horzScrollPos = 0;
		SetScrollRange(hwnd, SB_HORZ, 0, vw->horzScrollPosMax, FALSE);
		return;
	}

	vw->horzScrollPosMax = MAX(0, (int)vw->maxSymInLine - (int)vw->maxSymbCount);
	vw->horzScrollPos = MIN(vw->horzScrollPos, vw->horzScrollPosMax);

	SetScrollRange(hwnd, SB_HORZ, 0, MIN(SCROLL_MAX_RANGE, vw->horzScrollPosMax), FALSE);
}

void VertScroll(view_t* vw, WPARAM wParam, HWND hwnd) {
	int move = 0;
	int tmpVertScrl = 0;
	double dividedPos = 0.0;
	switch (LOWORD(wParam)) {
	case SB_LINEUP:
		// if scrolling is above the top of the text
		if (vw->vertScrollPos - vw->vertScrollUnit < 0)
			move = -vw->vertScrollPos;
		else
			move = -vw->vertScrollUnit;
		break;

	case SB_LINEDOWN:
		// if scrolling is below the bottom of the text
		if (vw->vertScrollPos + vw->vertScrollUnit > vw->vertScrollPosMax)
			move = vw->vertScrollPosMax - vw->vertScrollPos;
		else
			move = vw->vertScrollUnit;
		break;

	case SB_PAGEUP:
		// if scrolling is above the top of the text
		if ((int)vw->vertScrollPos - (int)vw->heightWinInLines < 0)
			move = -vw->vertScrollPos;
		else
			move = -MAX(1, vw->heightWinInLines);
		break;

	case SB_PAGEDOWN:
		// if scrolling is below the bottom of the text
		if (vw->vertScrollPos + vw->heightWinInLines > vw->vertScrollPosMax)
			move = vw->vertScrollPosMax - vw->vertScrollPos;
		else
			move = MAX(1, vw->heightWinInLines);
		break;

	case SB_THUMBTRACK:
		tmpVertScrl = HIWORD(wParam);
		// mapping a position in the text in the scroll range
		if (vw->vertScrollPosMax > SCROLL_MAX_RANGE) {
			dividedPos = (double)tmpVertScrl / (double)SCROLL_MAX_RANGE;
			tmpVertScrl = (int)(dividedPos * (double)vw->vertScrollPosMax);
		}
		move = tmpVertScrl - vw->vertScrollPos;
		break;

	default:
		move = 0;
		break;
	}

	if (move != 0) {
		vw->vertScrollPos += move;
		tmpVertScrl = vw->vertScrollPos;

		if (vw->mode == WRAP)
			VM_ShiftVerticalWrap(vw, -move);
		else
			VM_ShiftVerticalNoWRap(vw, -move);
		if (vw->vertScrollPosMax > SCROLL_MAX_RANGE) {
			dividedPos = (double)tmpVertScrl / (double)vw->vertScrollPosMax;
			tmpVertScrl = (int)(dividedPos * (double)SCROLL_MAX_RANGE);
		}
		SetScrollPos(hwnd, SB_VERT, tmpVertScrl, TRUE);

		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
	}
}

void HorzScroll(view_t* vw, WPARAM wParam, HWND hwnd) {
	int move = 0;
	int tmpHorzScrl = 0;
	double dividedPos = 0.0;

	switch (LOWORD(wParam)) {
	case SB_LINELEFT:
		// if the scroll is to the left of the beginning of the text
		if ((int)vw->horzScrollPos - (int)vw->horzScrollUnit < 0)
			move = -vw->horzScrollPos;
		else
			move = -vw->horzScrollUnit;
		break;

	case SB_LINERIGHT:
		// if the scroll is to the right of the end of the text
		if (vw->horzScrollPos + vw->horzScrollUnit > vw->horzScrollPosMax)
			move = vw->horzScrollPosMax - vw->horzScrollPos;
		else
			move = vw->horzScrollUnit;
		break;

	case SB_PAGELEFT:
		// if the scroll is to the left of the beginning of the text
		if ((int)vw->horzScrollPos - (int)vw->maxSymbCount < 0)
			move = -vw->horzScrollPos;
		else
			move = -MAX(1, vw->maxSymbCount);
		break;

	case SB_PAGERIGHT:
		// if the scroll is to the right of the end of the text
		if (vw->horzScrollPos + vw->maxSymbCount > vw->horzScrollPosMax)
			move = vw->horzScrollPosMax - vw->horzScrollPos;
		else
			move = MAX(1, vw->maxSymbCount);
		break;

	case SB_THUMBTRACK:
		tmpHorzScrl = HIWORD(wParam);
		// mapping a position in the text in the scroll range
		if (vw->horzScrollPosMax > SCROLL_MAX_RANGE) {
			dividedPos = (double)tmpHorzScrl / (double)SCROLL_MAX_RANGE;
			tmpHorzScrl = (int)(dividedPos * (double)vw->horzScrollPosMax);
		}
		move = tmpHorzScrl - vw->horzScrollPos;
		break;

	default:
		move = 0;
		break;
	}

	if (move != 0) {
		vw->horzScrollPos += move;
		tmpHorzScrl = vw->horzScrollPos;

		VM_ShiftHorizontal(vw, -move);

		if (vw->horzScrollPosMax > SCROLL_MAX_RANGE) {
			dividedPos = (double)tmpHorzScrl / (double)vw->horzScrollPosMax;
			tmpHorzScrl = (int)(dividedPos * (double)SCROLL_MAX_RANGE);
		}
		SetScrollPos(hwnd, SB_HORZ, tmpHorzScrl, TRUE);

		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
	}
}

void HandleArrows(WPARAM wParam, HWND hwnd) {
	switch (wParam) {
	case VK_UP:
		SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, (LPARAM)NULL);
		break;
	case VK_RIGHT:
		SendMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, (LPARAM)NULL);
		break;
	case VK_DOWN:
		SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)NULL);
		break;
	case VK_LEFT:
		SendMessage(hwnd, WM_HSCROLL, SB_LINEUP, (LPARAM)NULL);
		break;
	case VK_PRIOR:
		SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, (LPARAM)NULL);
		break;
	case VK_NEXT:
		SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, (LPARAM)NULL);
		break;
	}
}

void HandleKeys(WPARAM wParam, HWND hwnd) {
	switch (wParam) {
	case 'w':
		SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, (LPARAM)NULL);
		break;
	case 's':
		SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)NULL);
		break;
	case 'a':
		SendMessage(hwnd, WM_HSCROLL, SB_LINEUP, (LPARAM)NULL);
		break;
	case 'd':
		SendMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, (LPARAM)NULL);
		break;

	}
}
