#include "view.h"

int VM_ViewInit(text_t tx, view_t* vw, HWND hwnd) {
	int isChar = 0, lineReserve = 0;
	RECT wndRect;
	GetClientRect(hwnd, &wndRect);
	vw->winSize.w = wndRect.right - wndRect.left;
	vw->winSize.h = wndRect.bottom - wndRect.top;
	vw->maxSymbCount = vw->winSize.w / FONT_WIDTH;
	vw->heightWinInLines = vw->winSize.h / FONT_SIZE;
	vw->linesCount = 0;
	vw->maxSymInLine = 0;
	vw->firstLine = 0;
	vw->cntSubline = 0;
	vw->lineCountFormated = 0;
	vw->mode = WRAP;

	vw->vertScrollPos = 0;
	vw->horzScrollPos = 0;
	vw->vertScrollPosMax = 0;
	vw->horzScrollPosMax = 0;
	vw->vertScrollUnit = 0;
	vw->horzScrollUnit = 0;

	HDC hdc = GetDC(hwnd);
	HFONT textFont = CreateFont(FONT_SIZE, FONT_WIDTH, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, OUT_OUTLINE_PRECIS, PROOF_QUALITY, FF_MODERN, NULL);
	SelectObject(hdc, textFont); // font
	SetTextColor(hdc, RGB(0, 0, 0)); // color text
	SetBkMode(hdc, TRANSPARENT); // background

	if (tx.len == 0) {
		vw->data = NULL;
	}
	else {
		for (int i = 0; tx.txt[i] != '\0'; i++) { // count the number of lines
			if (isChar == 0 && tx.txt[i] != '\n') {
				vw->linesCount += lineReserve;
				lineReserve = 0;
				isChar = 1;
			}
			else if (isChar == 0 && tx.txt[i] == '\n') { // empty line, only \n
				lineReserve++;
			}
			else if (isChar == 1 && tx.txt[i] == '\n') {
				isChar = 0;
				vw->linesCount++;
			}
		}
		if (isChar == 1)
			vw->linesCount++;
	}
	return TRUE;
}

void VM_FixVertScrollPos(view_t* vw) {
	if (vw->data == NULL)
		return;
	//int k = vw->data[vw->firstLine].lineLen;
	int i, totalCurLen = ceil((double)(vw->data[vw->firstLine+1] - vw->data[vw->firstLine]) / (double)vw->maxSymbCount);
	if (vw->mode == NOWRAP) {
		vw->cntSubline = 0;
		if (vw->linesCount - vw->firstLine < vw->heightWinInLines && vw->linesCount > vw->heightWinInLines) {
			vw->firstLine = vw->linesCount - vw->heightWinInLines;
		}
		vw->vertScrollPos = vw->firstLine;
		return;
	}
	vw->vertScrollPos = 0;
	if (totalCurLen - 1 < vw->cntSubline) {
		vw->cntSubline = totalCurLen - 1;
	}

	// new position of scroll
	vw->vertScrollPos = 0;
	for (i = 0; i < vw->firstLine; i++) {
		totalCurLen = ceil((double)(vw->data[i + 1] - vw->data[i]) / (double)vw->maxSymbCount);
		if (totalCurLen == 0)
			vw->vertScrollPos++;
		else
			vw->vertScrollPos += totalCurLen;
	}
	vw->vertScrollPos += vw->cntSubline;

	if (vw->lineCountFormated - vw->vertScrollPos < vw->heightWinInLines && vw->lineCountFormated > vw->heightWinInLines) {
		VM_ShiftVerticalWrap(vw, abs(vw->heightWinInLines - (vw->lineCountFormated - vw->vertScrollPos)));
		vw->vertScrollPos = MAX(0, vw->vertScrollPos - (vw->heightWinInLines - vw->lineCountFormated + vw->vertScrollPos));
	}
	if (vw->vertScrollPosMax == 0)
		vw->cntSubline = 0;
}

void VM_FixHorScrollPos(view_t* vw) {
	if (vw->maxSymInLine - vw->horzScrollPos < vw->maxSymbCount && vw->maxSymInLine > vw->maxSymbCount) {
		VM_ShiftHorizontal(vw, -abs(vw->maxSymbCount - (vw->maxSymInLine - vw->horzScrollPos)));
		vw->horzScrollPos = MAX(0, vw->horzScrollPos - (vw->maxSymbCount - vw->maxSymInLine + vw->horzScrollPos));
	}
}

int VM_ParseText(text_t tx, view_t* vw) {
	int iLine = 0, iBegin = 0;
	vw->lineCountFormated = 0;
	if (tx.len != 0) {
		if ((vw->data = (char**)malloc(sizeof(char*) * (vw->linesCount + 1))) == NULL)
			return FALSE;
		int i;
		for (i = 0;  iLine < vw->linesCount; i++) { // заполнить массив указателей ссылками на строки текста
			if (tx.txt[i] == '\n' || tx.txt[i] == '\0') {
				vw->data[iLine++] = &(tx.txt[iBegin]);

				if (vw->maxSymInLine < i - iBegin) // количество символов строки, имеющей наибольшую длину
					vw->maxSymInLine = i - iBegin;

				if (i - iBegin > vw->maxSymbCount) { // считаем количество строк занимаемых текстом при формате с версткой
					vw->lineCountFormated += ceil((double)(i - iBegin) / (double)vw->maxSymbCount);//(i - iBegin) % vw->maxSymbCount == 0 ? 0 : 1;
				}
				else
					vw->lineCountFormated++;

				if (tx.txt[i + 1] != '\0')
					iBegin = i + 1;
			}
		}
		if (iLine != vw->linesCount) {
			vw->data[iLine] = &(tx.txt[iBegin]);
			vw->data[vw->linesCount] = &(tx.txt[tx.len]);
		}
		else
			vw->data[vw->linesCount] = &(tx.txt[tx.len]);

	}
	return TRUE;
}

void VM_RecountLines(view_t* vw) {
	if (vw->data == NULL)
		return;
	vw->lineCountFormated = 0;
	for (int i = 0; i < vw->linesCount; i++)
	{
		if (vw->data[i + 1] - vw->data[i] > vw->maxSymbCount) { // считаем количество строк занимаемых текстом при формате с версткой
			vw->lineCountFormated += ceil((double)(vw->data[i + 1] - vw->data[i]) / (double)vw->maxSymbCount);//(i - iBegin) % vw->maxSymbCount == 0 ? 0 : 1;
		}
		else
			vw->lineCountFormated++;
	}
}

void VM_UpdateSize(view_t* vw, int winW, int winH) { // обновить размер окна
	if (winH > 0 || winW > 0) {
		vw->winSize.h = winH;
		vw->winSize.w = winW;
		vw->maxSymbCount = winW / FONT_WIDTH;
		vw->heightWinInLines = winH / FONT_SIZE;
	}
	double a = ceil(vw->lineCountFormated / (int)SCROLL_MAX_RANGE);
	/*
	if (vw->mode == WRAP) {
		vw->vertScrollUnit = ceil((double)vw->lineCountFormated / (double)SCROLL_MAX_RANGE);
		vw->horzScrollUnit = ceil((double)vw->maxSymbCount / (double)SCROLL_MAX_RANGE);
	}
	else {
		vw->vertScrollUnit = ceil((double)vw->linesCount / (double)SCROLL_MAX_RANGE);
		vw->horzScrollUnit = ceil((double)vw->maxSymInLine / (double)SCROLL_MAX_RANGE);
	}
    */
    vw->vertScrollUnit = 1;
    vw->horzScrollUnit = 1;
}

int VM_CountTotalLinesInWin(view_t* vw) { // подсчитать количество (в определениях текста) напечатанных строк вместе с версткой
	int iLine = vw->firstLine, x = 0, tmpSubln = vw->cntSubline;
	for (int i = vw->firstLine; i < vw->heightWinInLines; i++) {
		if (vw->data[iLine + 1] - vw->data[iLine] > (x + 1 + tmpSubln) * vw->maxSymbCount) {
			x++;
		}
		else {
			iLine++;
			x = 0;
		}
		if (iLine >= vw->linesCount) {
			break; // end
		}
		tmpSubln = 0;
	}
	return iLine;
}

/* скролл вертикальный без верстки */
void VM_ShiftVerticalNoWRap(view_t* vw, int shift) {
	int move = abs(shift), i, totalCurLen;

	if (shift < 0) {
		for (i = 0; i < move; i++) {
			if (vw->firstLine == vw->linesCount - 1)
				return;
			vw->firstLine++;
			vw->cntSubline = 0;
		}
	}
	else if (shift > 0) {
		for (i = 0; i < move; i++) {
			if (vw->firstLine == 0 && vw->cntSubline == 0)
				return;
			vw->firstLine--;
		}
	}
}

void VM_ShiftVerticalWrap(view_t* vw, int shift) {  // скролл  вертикально
	//int iLine = VM_CountTotalLinesInWin(vw);
	int move = abs(shift), i, totalCurLen;

	if (shift < 0) { // скролл вниз
		for (i = 0; i < move; i++) {
			totalCurLen = ceil((double)(vw->data[vw->firstLine + 1] - vw->data[vw->firstLine]) / (double)vw->maxSymbCount);
			if (vw->firstLine == vw->linesCount - 1 && (totalCurLen - 1 == vw->cntSubline || totalCurLen == 0))
				return;
			else if (totalCurLen - 1 == vw->cntSubline || totalCurLen == 0) {
				vw->firstLine++;
				vw->cntSubline = 0;
			}
			else {
				vw->cntSubline++;
			}
		}
	}
	else if (shift > 0) {
		for (i = 0; i < move; i++) {
			if (vw->firstLine == 0 && vw->cntSubline == 0)
				return;
			else if (vw->cntSubline == 0) {
				vw->firstLine--;
				totalCurLen = ceil((double)(vw->data[vw->firstLine + 1] - vw->data[vw->firstLine]) / (double)vw->maxSymbCount);
				vw->cntSubline = MAX(0, totalCurLen - 1);
			}
			else {
				vw->cntSubline--;
			}
		}
	}
}

void VM_ShiftHorizontal(view_t* vw, int shift) {
	int move = abs(shift);
	if (shift < 0) { // вправо
		vw->horzScrollPos = MIN(vw->maxSymInLine - vw->maxSymbCount, vw->horzScrollPos + move);
	}
	else {
		vw->horzScrollPos = MAX(0, vw->horzScrollPos - move);
	}
}

// drawing text
void VM_DrawText(HWND hwnd, view_t* vw) {

	RECT ScreenRect;
	PAINTSTRUCT ps;
	HDC hdc;
	int x = 0; // сдвиг в каждой строке
	int iLine = vw->firstLine, drawnLine = 0;
	int tmpSubln = vw->cntSubline;
	char* toDrawStr = NULL;
	int toDrawLen = 0, lineLen = 0;
	if (vw->data == NULL)
		return;

	hdc = BeginPaint(hwnd, &ps);
	GetClientRect(hwnd, &ScreenRect);
	if (vw->mode == WRAP) {
		for (int i = 0; i < vw->heightWinInLines && iLine < vw->linesCount; i++)
		{
			lineLen = vw->data[iLine + 1] - vw->data[iLine];
			toDrawStr = vw->data[iLine] + (x + tmpSubln) * vw->maxSymbCount;
			toDrawLen = MIN(vw->maxSymbCount, lineLen - (x + tmpSubln) * vw->maxSymbCount);
			if (lineLen > (x + 1 + tmpSubln) * vw->maxSymbCount) {
				TextOut(hdc, ScreenRect.left, ScreenRect.top + drawnLine++ * FONT_SIZE, toDrawStr, toDrawLen);
				x++;
			}
			else {
				TextOut(hdc, ScreenRect.left, ScreenRect.top + drawnLine++ * FONT_SIZE, toDrawStr, toDrawLen);
				iLine++;
				x = 0;
				tmpSubln = 0; // обнуляем, тк строка уже полностью напечатана (ее длина меньше макс. элементов в строке)
			}
		}
	}
	else if (vw->mode == NOWRAP) {
		for (int i = 0; i < vw->heightWinInLines && iLine < vw->linesCount; i++)
		{
			lineLen = vw->data[iLine + 1] - vw->data[iLine];
			toDrawStr = vw->data[iLine] + vw->horzScrollPos;
			toDrawLen = lineLen - vw->horzScrollPos;
			if (toDrawLen > 0)
				toDrawLen = MIN(toDrawLen, vw->maxSymbCount);

			TextOut(hdc, ScreenRect.left, ScreenRect.top + drawnLine++ * FONT_SIZE, toDrawStr, toDrawLen);
			iLine++;
		}
	}

	EndPaint(hwnd, &ps);
}
