#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

void input_update();

int input_touch_x();
int input_touch_y();

bool input_touch_pressed();   // Finger on pad
bool input_touch_click();     // Physical click

#endif
