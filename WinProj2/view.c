#include "view.h"

int VM_ViewInit(text_t tx, view_t* vw, int winH, int winW, HWND hwnd) {
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
	

	HDC hdc = GetDC(hwnd);
	HFONT textFont = CreateFont(FONT_SIZE, FONT_WIDTH, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, OUT_OUTLINE_PRECIS, PROOF_QUALITY, FF_MODERN, NULL);
	SelectObject(hdc, textFont); // font
	SetTextColor(hdc, RGB(0, 0, 0)); // color text
	SetBkMode(hdc, TRANSPARENT); // background

	if (tx.len == 0) {
		vw->data = NULL;
	}
	else {
		for (int i = 0; tx.text[i] != '\0'; i++) { // count the number of lines
			if (isChar == 0 && tx.text[i] != '\n') {
				vw->linesCount += lineReserve;
				lineReserve = 0;
				isChar = 1;
			}
			else if (isChar == 0 && tx.text[i] == '\n') { // empty line, only \n
				lineReserve++;
			}
			else if (isChar == 1 && tx.text[i] == '\n') {
				isChar = 0;
				vw->linesCount++;
			}
		}
		if (isChar == 1)
			vw->linesCount++;
	}
	return TRUE;
}

int VM_ParseText(text_t tx, view_t* vw) {
	int iLine = 0, iBegin = 0;
	vw->lineCountFormated = 0;
	if (tx.len != 0) {
		if ((vw->data = (line_t*)malloc(sizeof(line_t) * vw->linesCount)) == NULL)
			return FALSE;

		for (int i = 0; tx.text[i] != '\0' && iLine < vw->linesCount; i++) { // ��������� ������ ���������� �������� �� ������ ������
			if (tx.text[i] == '\n') {
				vw->data[iLine].line = &(tx.text[iBegin]);
				vw->data[iLine++].lineLen = i - iBegin;

				if (vw->maxSymInLine < i - iBegin) // ���������� �������� ������, ������� ���������� �����
					vw->maxSymInLine = i - iBegin;

				if (i - iBegin > vw->maxSymbCount) { // ������� ���������� ����� ���������� ������� ��� ������� � ��������
					vw->lineCountFormated += (i - iBegin) / vw->maxSymbCount;
					vw->lineCountFormated += (i - iBegin) % vw->maxSymbCount == 0 ? 0 : 1;
				}
				else
					vw->lineCountFormated++;

				if (tx.text[i + 1] != '\0')
					iBegin = i + 1;
			}
		}
	}
	return TRUE;
}

void VM_UpdateSize(view_t* vw, int winW, int winH) { // �������� ������ ����
	if (winH < 0 || winW < 0)
		return;

	vw->winSize.h = winH;
	vw->winSize.w = winW;
	vw->maxSymbCount = winW / FONT_WIDTH;
	vw->heightWinInLines = winH / FONT_SIZE;
}

int VM_CountTotalLinesInWin(view_t* vw) { // ���������� ���������� (� ������������ ������) ������������ ����� ������ � ��������
	int iLine = vw->firstLine, x = 0, tmpSubln = vw->cntSubline;
	for (int i = vw->firstLine; i < vw->heightWinInLines; i++) {
		if (vw->data[iLine].lineLen > (x + 1 + tmpSubln) * vw->maxSymbCount) {
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

void VM_ShiftVerticalFormated(view_t* vw, int shift) {  // ������  �����������
	int iLine = VM_CountTotalLinesInWin(vw);
	int move = abs(shift), dd, totalCurLen = ceil((double)vw->data[vw->firstLine].lineLen / (double)vw->maxSymbCount);
	if (move > 4) {
		dd = 0;
	}
	if (shift < 0 && iLine < vw->linesCount) { // ���� ������ ���� � ������� ��� ����
		if ((int)vw->data[vw->firstLine].lineLen - (vw->cntSubline) * vw->maxSymbCount  > (int)vw->maxSymbCount * move) {
			vw->cntSubline += move;
		}
		else {
			if (totalCurLen == 0) {
				vw->cntSubline = 0;
			}
			else
				vw->cntSubline = move - (totalCurLen - vw->cntSubline);
			vw->firstLine++;
			
		}
	}
	else if (shift > 0) { // ���� ������ �����
		if (vw->firstLine == 0 && vw->data[vw->firstLine].lineLen - move > vw->maxSymbCount) { // ���� ���� ���� �������� �� ������ ������
			if (vw->cntSubline != 0)
				vw->cntSubline -= move;
		}
		else if (vw->firstLine != 0) {
			int curLen;
			if (vw->cntSubline == 0) { // ���� ��� ������� ����� ���������� ����� ������ ������
				vw->firstLine--;
				curLen = vw->data[vw->firstLine].lineLen - vw->cntSubline * vw->maxSymbCount;
			}
			else // ���� ����� ������ ��� ���������� ��� �������, ����� ������� ������ ��� ��������� ����� ����� ������
				curLen = vw->data[vw->firstLine].lineLen - (vw->cntSubline - 1) * vw->maxSymbCount; 

			if (curLen > vw->maxSymbCount) {
				if (vw->cntSubline > 0) {
					vw->cntSubline--; // ��������� sntSubLine => ����������� ���������� ���������� ������ ������
				}
				else {
					// ��������� ����� ������, ������� ������� ����� � ���� ������������ � �������� �������� ������
					int tmp = curLen % vw->maxSymbCount == 0 ? 0 : 1; 
					vw->cntSubline = curLen / vw->maxSymbCount + tmp - 1;
				}
			}
			else { // ���������� ������ ������� ������� ��� �������, �������� "�����"
				vw->cntSubline = 0;
			}
		}
	}
}

// drawing text
void VM_DrawText(HWND hwnd, view_t *vw) {
	RECT ScreenRect;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	int x = 0; // ����� � ������ ������
	int iLine = vw->firstLine, drawnLine = 0;
	int tmpSubln = vw->cntSubline;

	GetClientRect(hwnd, &ScreenRect);
	for (int i = 0; i < vw->heightWinInLines && iLine < vw->linesCount; i++)
	{
		if (vw->data[iLine].lineLen > (x + 1 + tmpSubln) * vw->maxSymbCount) {
			TextOut(hdc, ScreenRect.left, ScreenRect.top + drawnLine++ * FONT_SIZE,
				vw->data[iLine].line + (x + tmpSubln) * vw->maxSymbCount,
				MIN(vw->maxSymbCount, vw->data[iLine].lineLen - (x + tmpSubln) * vw->maxSymbCount));
			x++;
		}
		else {
			TextOut(hdc, ScreenRect.left, ScreenRect.top + drawnLine++ * FONT_SIZE,
				vw->data[iLine].line + (x + tmpSubln) * vw->maxSymbCount,
				MIN(vw->maxSymbCount, vw->data[iLine].lineLen - (x + tmpSubln) * vw->maxSymbCount));
			iLine++;
			x = 0;
			tmpSubln = 0; // ��������, �� ������ ��� ��������� ���������� (�� ����� ������ ����. ��������� � ������)
		}

	}

	EndPaint(hwnd, &ps);
}