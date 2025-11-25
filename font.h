#ifndef FONT_H
#define FONT_H

#include <SDL/SDL.h>

extern const unsigned char font_data[95][8];

void draw_pixel(int x, int y, Uint16 color);
void draw_char(int x, int y, char c, Uint16 color);
void draw_string(int x, int y, const char* str, Uint16 color);
void draw_filled_rect(int x, int y, int w, int h, Uint16 color);

extern SDL_Surface* screen;

#endif