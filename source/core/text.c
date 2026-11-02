#include "libraries.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "rsx_renderer.h"
#include "text.h"
#include "asimov_ttf_bin.h"

#define FONT_FIRST      32
#define FONT_LAST       255
#define FONT_CELL       64                       /* glyphs are rendered at 64x64 */
#define ATLAS_COLS      16
#define ATLAS_ROWS      (((FONT_LAST - FONT_FIRST + 1) + ATLAS_COLS - 1) / ATLAS_COLS)
#define FONT_PAD        1                        /* transparent gutter around each glyph */
#define CELL_STRIDE     (FONT_CELL + 2 * FONT_PAD)
#define ATLAS_W         (ATLAS_COLS * CELL_STRIDE)
#define ATLAS_H         (ATLAS_ROWS * CELL_STRIDE)

#define FONT_TEX_ID     (RSX_MAX_TEXTURES - 2)   /* 63 is the white texture */
#define FONT_SCREEN_W   916.0f                   /* canvas width, used by auto-center */

typedef struct
{
    float advance;      /* in 64px glyph units */
    short rows;         /* bitmap rows actually used */
    short y_corr;       /* offset from the top of the cell */
} FontGlyph;

static FontGlyph glyphs[256];
static int       text_ready = 0;

static float font_w = 32.0f, font_h = 32.0f;
static u32   font_color = 0xFFFFFFFF;
static int   font_autocenter = 0;

static FT_Library freetype;
static FT_Face    face;
static int        ttf_inited = 0;

static int TTFLoadFont(char *path, void *from_memory, int size_from_memory)
{
    if (!ttf_inited) FT_Init_FreeType(&freetype);
    ttf_inited = 1;

    if (path) { if (FT_New_Face(freetype, path, 0, &face)) return -1; }
    else      { if (FT_New_Memory_Face(freetype, from_memory, size_from_memory, 0, &face)) return -1; }

    return 0;
}

static void TTFUnloadFont(void)
{
    FT_Done_FreeType(freetype);     /* also releases the face */
    ttf_inited = 0;
}

static void TTF_to_Bitmap(u8 chr, u8 *bitmap, short *w, short *h, short *y_correction)
{
    FT_Set_Pixel_Sizes(face, (*w), (*h));
    FT_GlyphSlot slot = face->glyph;
    memset(bitmap, 0, (*w) * (*h));

    uint32_t unicode_cp = chr;
    int is_kazakh_qa = 0;

    if      (chr == 0xA1) unicode_cp = 0x04D8;
    else if (chr == 0xA2) unicode_cp = 0x04D9;
    else if (chr == 0xA3) unicode_cp = 0x0492;
    else if (chr == 0xA4) unicode_cp = 0x0493;

    else if (chr == 0xA5) { unicode_cp = 0x049A; is_kazakh_qa = 1; }
    else if (chr == 0xA6) { unicode_cp = 0x049B; is_kazakh_qa = 2; }

    else if (chr == 0xA7) unicode_cp = 0x04A2;
    else if (chr == 0xAA) unicode_cp = 0x04A3;

    else if (chr == 0xAB) unicode_cp = 0x04E8;
    else if (chr == 0xAC) unicode_cp = 0x04E9;
    else if (chr == 0xAD) unicode_cp = 0x04AE;
    else if (chr == 0xAE) unicode_cp = 0x04AF;
    else if (chr == 0xAF) unicode_cp = 0x04B0;
    else if (chr == 0xB0) unicode_cp = 0x04B1;
    else if (chr == 0xB1) unicode_cp = 0x04BA;
    else if (chr == 0xB2) unicode_cp = 0x04BB;
    else if (chr == 0xB8) unicode_cp = 0x0451;

    else if (chr >= 192 && chr <= 255) unicode_cp = (uint32_t)chr + 0x0350;
    else if (chr == 168) unicode_cp = 0x0401;
    else if (chr == 184) unicode_cp = 0x0451;

    if (FT_Load_Char(face, unicode_cp, FT_LOAD_RENDER)) { (*w) = 0; return; }

    int n, m, ww;
    int target_w = *w;
    int target_h = *h;

    *y_correction = target_h - 1 - slot->bitmap_top;
    ww = 0;

    for (n = 0; n < slot->bitmap.rows; n++)
    {
        for (m = 0; m < slot->bitmap.width; m++)
        {
            if (m >= target_w || n >= target_h) continue;
            bitmap[m] = (u8)slot->bitmap.buffer[ww + m];
        }
        bitmap += target_w;
        ww += slot->bitmap.width;
    }

    bitmap -= (target_w * slot->bitmap.rows);

    if (is_kazakh_qa > 0)
    {
        int base_row = slot->bitmap.rows - 1;
        int right_edge = slot->bitmap.width - 1;

        if (right_edge > target_w - 4) right_edge = target_w - 4;

        if (base_row > 0 && right_edge > 0)
        {
            bitmap[(base_row * target_w) + right_edge + 1] = 255;
            bitmap[(base_row * target_w) + right_edge + 2] = 255;
            if (base_row + 1 < target_h) bitmap[((base_row + 1) * target_w) + right_edge + 2] = 255;
        }
    }

    *w = ((slot->advance.x + 31) >> 6) + ((slot->bitmap_left < 0) ? -slot->bitmap_left : 0);

    if (is_kazakh_qa > 0) *w += 3;

    *h = slot->bitmap.rows;
}

int Text_Init(void)
{
    u32 *atlas;
    u8  *bitmap;
    int  c, ok;

    if (text_ready) return 1;

    atlas  = (u32*)calloc((size_t)ATLAS_W * ATLAS_H, sizeof(u32));
    bitmap = (u8*)malloc(FONT_CELL * FONT_CELL);
    if (!atlas || !bitmap) { free(atlas); free(bitmap); return 0; }

    if (TTFLoadFont(NULL, (void*)asimov_ttf_bin, asimov_ttf_bin_size) != 0)
    {
        printf("TEXT: failed to load TTF\n");
        free(atlas); free(bitmap);
        return 0;
    }

    memset(glyphs, 0, sizeof(glyphs));

    for (c = FONT_FIRST; c <= FONT_LAST; c++)
    {
        short w = FONT_CELL, h = FONT_CELL, yc = 0;
        int idx = c - FONT_FIRST;
        int cx = (idx % ATLAS_COLS) * CELL_STRIDE + FONT_PAD;
        int cy = (idx / ATLAS_COLS) * CELL_STRIDE + FONT_PAD;
        int x, y;

        TTF_to_Bitmap((u8)c, bitmap, &w, &h, &yc);
        if (w <= 0) continue;                       /* glyph missing in font */

        if (h > FONT_CELL) h = FONT_CELL;
        if (h < 0) h = 0;

        glyphs[c].advance = (float)w;
        glyphs[c].rows    = h;
        glyphs[c].y_corr  = yc;

        for (y = 0; y < h; y++)
            for (x = 0; x < FONT_CELL; x++)
                atlas[(size_t)(cy + y) * ATLAS_W + cx + x] = ((u32)bitmap[y * FONT_CELL + x] << 24) | 0x00FFFFFF;
    }

    TTFUnloadFont();
    free(bitmap);

    ok = RSX_LoadTexture(FONT_TEX_ID, atlas, ATLAS_W, ATLAS_H);
    free(atlas);

    if (!ok) { printf("TEXT: atlas upload failed\n"); return 0; }

    RSX_SetTextureFilter(FONT_TEX_ID, 1);
    text_ready = 1;
    printf("TEXT: atlas ready (%dx%d)\n", ATLAS_W, ATLAS_H);
    return 1;
}

void Text_Shutdown(void)
{
    RSX_UnloadTexture(FONT_TEX_ID);
    text_ready = 0;
}

void SetFontSize(float sx, float sy)          { font_w = sx; font_h = sy; }
void SetFontColor(u32 rgba, u32 bk_rgba)      { (void)bk_rgba; font_color = rgba; }
void SetFontAutoCenter(int on_off)            { font_autocenter = on_off; }

float WidthFromStr(const char *str)
{
    float w = 0.0f, sx = font_w / FONT_CELL;

    for (; *str; str++)
    {
        u8 c = (u8)*str;
        if (c < FONT_FIRST) continue;
        w += glyphs[c].advance * sx;
    }
    return w;
}

float DrawString(float x, float y, const char *str)
{
    float sx = font_w / FONT_CELL;
    float sy = font_h / FONT_CELL;
    float start_x;

    if (!text_ready || !str) return x;

    if (font_autocenter) x = (FONT_SCREEN_W - WidthFromStr(str)) * 0.5f;
    start_x = x;

    for (; *str; str++)
    {
        u8 c = (u8)*str;
        const FontGlyph *g;
        int idx;

        if (c == '\n') { x = start_x; y += font_h; continue; }
        if (c < FONT_FIRST) continue;

        g = &glyphs[c];
        idx = c - FONT_FIRST;

        if (g->rows > 0)
        {
            float u = (float)((idx % ATLAS_COLS) * CELL_STRIDE + FONT_PAD) / ATLAS_W;
            float v = (float)((idx / ATLAS_COLS) * CELL_STRIDE + FONT_PAD) / ATLAS_H;

            RSX_DrawSprite2D(FONT_TEX_ID, x, y + g->y_corr * sy, FONT_CELL * sx, g->rows * sy,
				u, v, (float)FONT_CELL / ATLAS_W, (float)g->rows / ATLAS_H, font_color);
        }
        x += g->advance * sx;
    }
    return x;
}

float DrawFormatString(float x, float y, const char *format, ...)
{
    char buffer[1024];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    return DrawString(x, y, buffer);
}

void DrawFXString(float x, float y, const char *format, ...)
{
    char buffer[1024];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    DrawString(x, y, buffer);
}

void utf8_to_cp1251(const char *src, char *dest, size_t max_len)
{
    size_t i = 0, j = 0;
    while (src[i] != '\0' && j < max_len - 1)
    {
        unsigned char c1 = (unsigned char)src[i];
        unsigned char c2 = (unsigned char)src[i + 1];

        if (c1 == 0xD3 && c2 == 0x98) { dest[j++] = (char)0xA1; i += 2; continue; } // Ә
        if (c1 == 0xD3 && c2 == 0x99) { dest[j++] = (char)0xA2; i += 2; continue; } // ә
        if (c1 == 0xD2 && c2 == 0x92) { dest[j++] = (char)0xA3; i += 2; continue; } // Ғ
        if (c1 == 0xD2 && c2 == 0x93) { dest[j++] = (char)0xA4; i += 2; continue; } // ғ

        if (c1 == 0xD2 && c2 == 0x9A) { dest[j++] = (char)0xA5; i += 2; continue; } // Қ
        if (c1 == 0xD2 && c2 == 0x9B) { dest[j++] = (char)0xA6; i += 2; continue; } // қ

        if (c1 == 0xD2 && c2 == 0xA2) { dest[j++] = (char)0xA7; i += 2; continue; } // Ң
        if (c1 == 0xD2 && c2 == 0xA3) { dest[j++] = (char)0xAA; i += 2; continue; } // ң
        if (c1 == 0xD3 && c2 == 0xA8) { dest[j++] = (char)0xAB; i += 2; continue; } // Ө
        if (c1 == 0xD3 && c2 == 0xA9) { dest[j++] = (char)0xAC; i += 2; continue; } // ө
        if (c1 == 0xD2 && c2 == 0xAE) { dest[j++] = (char)0xAD; i += 2; continue; } // Ұ
        if (c1 == 0xD2 && c2 == 0xAF) { dest[j++] = (char)0xAE; i += 2; continue; } // ұ
        if (c1 == 0xD2 && c2 == 0xB0) { dest[j++] = (char)0xAF; i += 2; continue; } // Ү
        if (c1 == 0xD2 && c2 == 0xB1) { dest[j++] = (char)0xB0; i += 2; continue; } // ү
        if (c1 == 0xD2 && c2 == 0xBA) { dest[j++] = (char)0xB1; i += 2; continue; } // Һ
        if (c1 == 0xD2 && c2 == 0xBB) { dest[j++] = (char)0xB2; i += 2; continue; } // һ
        if (c1 == 0xD0 && c2 == 0x86) { dest[j++] = (char)0x49; i += 2; continue; } // І
        if (c1 == 0xD1 && c2 == 0x96) { dest[j++] = (char)0x69; i += 2; continue; } // і

        if (c1 == 0xD0 && c2 == 0x81) { dest[j++] = (char)0xA8; i += 2; continue; } // Ё
        if (c1 == 0xD1 && c2 == 0x91) { dest[j++] = (char)0xB8; i += 2; continue; } // ё

        if (c1 == 0xD0 && c2 >= 0x90 && c2 <= 0xBF) { dest[j++] = (char)(c2 + 0x30); i += 2; continue; }
        if (c1 == 0xD1 && c2 >= 0x80 && c2 <= 0x8F) { dest[j++] = (char)(c2 + 0x70); i += 2; continue; }

        dest[j++] = src[i++];
    }
    dest[j] = '\0';
}

void DrawWrappedText(float x, float y, float line_h, int max_chars, const char *text)
{
    char utf8_buffer[1024];
    char cp1251_buffer[1024];

    snprintf(utf8_buffer, sizeof(utf8_buffer), "%s", text);
    utf8_to_cp1251(utf8_buffer, cp1251_buffer, sizeof(cp1251_buffer));

    char line[1024];
    char word[256];
    int line_len = 0;
    int word_len = 0;
    int i = 0;

    line[0] = '\0';
    word[0] = '\0';

    while (1)
    {
        char c = cp1251_buffer[i];

        if (c != ' ' && c != '\0' && c != '\n')
        {
            if (word_len < (int)sizeof(word) - 1)
            {
                word[word_len++] = c;
                word[word_len] = '\0';
            }
        }

        if (c == ' ' || c == '\0' || c == '\n')
        {
            if (word_len > 0)
            {
                int needed = word_len;
                if (line_len > 0) needed += 1;

                if (line_len + needed > max_chars)
                {
                    DrawFXString(x, y, "%s", line);
                    y += line_h;

                    strcpy(line, word);
                    line_len = word_len;
                }
                else
                {
                    if (line_len > 0)
                    {
                        strcat(line, " ");
                        line_len++;
                    }
                    strcat(line, word);
                    line_len += word_len;
                }

                word[0] = '\0';
                word_len = 0;
            }

            if (c == '\n')
            {
                DrawFXString(x, y, "%s", line);
                y += line_h;
                line[0] = '\0';
                line_len = 0;
            }

            if (c == '\0') break;
        }

        i++;
    }

    if (line_len > 0) DrawFXString(x, y, "%s", line);
}