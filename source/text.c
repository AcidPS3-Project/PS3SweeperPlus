#include "text.h"
#include "importer.h"

int TTFLoadFont(char * path, void * from_memory, int size_from_memory)
{
    if(!ttf_inited) FT_Init_FreeType(&freetype);
    ttf_inited = 1;

    if(path) {if(FT_New_Face(freetype, path, 0, &face)) return -1;}
		else {if(FT_New_Memory_Face(freetype, from_memory, size_from_memory, 0, &face)) return -1;}

    return 0;
}

void TTFUnloadFont()
{
	FT_Done_FreeType(freetype);
	ttf_inited = 0;
}

void TTF_to_Bitmap(u8 chr, u8 * bitmap, short *w, short *h, short *y_correction)
{
    FT_Set_Pixel_Sizes(face, (*w), (*h));
    FT_GlyphSlot slot = face->glyph;
    memset(bitmap, 0, (*w) * (*h));
    if(FT_Load_Char(face, (char) chr, FT_LOAD_RENDER )) {(*w) = 0; return;}
    int n, m, ww;

    *y_correction = (*h) - 1 - slot->bitmap_top;
    ww = 0;

    for(n = 0; n < slot->bitmap.rows; n++)
	{
        for (m = 0; m < slot->bitmap.width; m++)
		{
            if(m >= (*w) || n >= (*h)) continue;
            bitmap[m] = (u8) slot->bitmap.buffer[ww + m];
        }
		bitmap += *w;
		ww += slot->bitmap.width;
    }

    *w = ((slot->advance.x + 31) >> 6) + ((slot->bitmap_left < 0) ? -slot->bitmap_left : 0);
    *h = slot->bitmap.rows;
}

void DrawWrappedText(float x, float y, float line_h, int max_chars, const char *text)
{
    char line[512];
    char word[128];
    int line_len = 0;
    int word_len = 0;
    int i = 0;

    line[0] = '\0';
    word[0] = '\0';

    while(1)
    {
        char c = text[i];

        // build current word
        if(c != ' ' && c != '\0' && c != '\n')
        {
            if(word_len < sizeof(word) - 1)
            {
                word[word_len++] = c;
                word[word_len] = '\0';
            }
        }

        // if word ended / newline / end of string
        if(c == ' ' || c == '\0' || c == '\n')
        {
            if(word_len > 0)
            {
                int needed = word_len;
                if(line_len > 0) needed += 1; // space

                if(line_len + needed > max_chars)
                {
                    DrawFormatString(x, y, "%s", line);
                    y += line_h;

                    strcpy(line, word);
                    line_len = word_len;
                }
                else
                {
                    if(line_len > 0)
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

            if(c == '\n')
            {
                DrawFormatString(x, y, "%s", line);
                y += line_h;
                line[0] = '\0';
                line_len = 0;
            }

            if(c == '\0') break;
        }

        i++;
    }

    if(line_len > 0) DrawFormatString(x, y, "%s", line);
}