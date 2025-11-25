#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <libndls.h>

#define MOUSE_W 8
#define MOUSE_H 8

typedef struct {
    int x;
    int y;
    bool touching;
    bool click;
    int sensitivity;
    touchpad_report_t prev;
} MouseState;

extern const uint16_t mouse_cursor[MOUSE_W * MOUSE_H];

void mouse_init(MouseState* m);
void mouse_update(MouseState* m);
void mouse_draw(MouseState* m);

#endif
