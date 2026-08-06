#ifndef TEXT_H
#define TEXT_H

#include "libraries.h"

FT_Library freetype;
FT_Face face;

int TTFLoadFont(char * path, void * from_memory, int size_from_memory);
void TTFUnloadFont();
void TTF_to_Bitmap(u8 chr, u8 * bitmap, short *w, short *h, short *y_correction);

#endif