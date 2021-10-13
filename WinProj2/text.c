#include "text.h"

int TextReader(text_t* text, const char* file) {
	FILE* F;

	if ((F = fopen(file, "r")) == NULL)
		return FALSE;

	fseek(F, 0, SEEK_END);
	int bufLen = ftell(F);
	fseek(F, 0, SEEK_SET);

	if ((text->text = (char*)malloc(sizeof(char) * bufLen + 1)) == NULL)
		return FALSE;

	text->len = fread(text->text, 1, bufLen, F);
	text->text[text->len] = '\0';

	fclose(F);
	return TRUE;
}
