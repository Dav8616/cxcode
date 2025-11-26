#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

#include <stdint.h>
#include <stdbool.h> // Using 'bool' type for clearer flag semantics
#include <libndls.h> // For touchpad_report_t

#define MOUSE_W 8
#define MOUSE_H 8

typedef struct {
    int x;
    int y;
    bool touching;
    bool click;
    int sensitivity;
    // We group the hardware structure at the end, though placement doesn't matter much
    touchpad_report_t prev; 
} MouseState;

// Declares the mouse cursor array, which is defined in mouse.cpp
extern const uint16_t mouse_cursor[MOUSE_W * MOUSE_H]; 

void mouse_init(MouseState* m);
void mouse_update(MouseState* m);
void mouse_draw(MouseState* m);

#endif