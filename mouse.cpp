#include "mouse.h"
#include <SDL/SDL.h>
#include "editor.h"
extern Editor ed;
extern SDL_Surface* screen; 
void clamp_cursor(Editor* ed);


// Simple arrow cursor
const uint16_t mouse_cursor[MOUSE_W * MOUSE_H] = {
    0xFFFF,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0xFFFF,
    0xFFFF,
    0,
    0,
    0,
    0,
    0,
    0,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0,
    0,
    0,
    0,
    0,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0,
    0,
    0,
    0,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0,
    0,
    0,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0,
    0,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
};

void mouse_init(MouseState *m)
{
    m->x = 160;
    m->y = 120;
    m->touching = false;
    m->click = false;
    m->sensitivity = 4;
}

void mouse_update(MouseState *m)
{
    touchpad_report_t info;
    touchpad_scan(&info);

    if (info.contact)
    {
        if (!m->touching)
            m->prev = info;

        int dx = info.x - m->prev.x;
        int dy = info.y - m->prev.y;

        m->x += dx / (9 - m->sensitivity);
        m->y -= dy / (9 - m->sensitivity);

        if (m->x < 0)
            m->x = 0;
        if (m->y < 0)
            m->y = 0;
        if (m->x > 320 - MOUSE_W)
            m->x = 320 - MOUSE_W;
        if (m->y > 240 - MOUSE_H)
            m->y = 240 - MOUSE_H;

        m->prev = info;
        m->touching = true;
    }
    else
    {
        m->touching = false;
    }

    m->click = isKeyPressed(KEY_NSPIRE_CLICK);
    if (ed.mouse.click)
    {
        int line = (ed.mouse.y - 12) / 8 + ed.scroll_y;
        int col = (ed.mouse.x - (ed.show_line_numbers ? 26 : 2)) / 6;

        if (line >= 0 && line < ed.line_count)
        {
            ed.cursor_y = line;
            ed.cursor_x = col;
            clamp_cursor(&ed);
        }
    }
}

void mouse_draw(MouseState *m)
{
    uint16_t *pix = (uint16_t *)screen->pixels;

    for (int y = 0; y < MOUSE_H; y++)
    {
        for (int x = 0; x < MOUSE_W; x++)
        {
            uint16_t col = mouse_cursor[y * MOUSE_W + x];
            if (col)
                pix[(m->y + y) * 320 + (m->x + x)] = col;
        }
    }
}
