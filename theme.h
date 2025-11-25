#ifndef THEME_H
#define THEME_H

#include <SDL/SDL.h>

typedef struct {
    Uint16 bg;
    Uint16 text;
    Uint16 line_num;
    Uint16 cursor;
    Uint16 title_bar;
    Uint16 status_bar;
    Uint16 menu_bg;
    Uint16 menu_border;
    Uint16 menu_selected;
    Uint16 keyword;
    Uint16 string;
    Uint16 comment;
    Uint16 number;
} Theme;

#define THEME_FIELD_COUNT 13   // or however many fields Theme actually has

extern const char* theme_keys[THEME_FIELD_COUNT];
Uint16* theme_get_field(Theme* t, int index);

void theme_init_default(Theme* theme);
int theme_load(Theme* theme, const char* filename);
int theme_save(Theme* theme, const char* filename);
Uint16 rgb_to_565(int r, int g, int b);
void rgb_from_565(Uint16 color, int* r, int* g, int* b);

#endif