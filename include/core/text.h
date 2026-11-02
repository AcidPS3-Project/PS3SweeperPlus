#ifndef TEXT_H
#define TEXT_H

#include <ppu-types.h>
#include <stddef.h>

/* Builds the glyph atlas (needs RSX already initialised). Returns 1 on success. */
int  Text_Init(void);
void Text_Shutdown(void);

/* libfont-style API */
void  SetFontSize(float sx, float sy);          /* text box size in canvas pixels */
void  SetFontColor(u32 rgba, u32 bk_rgba);      /* background colour is ignored   */
void  SetFontAutoCenter(int on_off);
float WidthFromStr(const char *str);
float DrawString(float x, float y, const char *str);
float DrawFormatString(float x, float y, const char *format, ...);

/* game helpers (unchanged behaviour) */
void DrawFXString(float x, float y, const char *format, ...);
void DrawWrappedText(float x, float y, float line_h, int max_chars, const char *text);
void utf8_to_cp1251(const char *src, char *dest, size_t max_len);

#endif
