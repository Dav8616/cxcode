#include "input.h"
#include <libndls.h>
#include <stdint.h>

// ------------------------------
// Key & click wrappers
// ------------------------------

bool input_key(t_key k)
{
    return isKeyPressed(k);   // macro adds the & for us
}

bool input_click()
{
    return isKeyPressed(KEY_NSPIRE_CLICK);
}

bool input_escape()
{
    return isKeyPressed(KEY_NSPIRE_ESC);
}

// ------------------------------
// Touchpad polling (raw only)
// mouse.cpp will turn this
// into a real cursor.
// ------------------------------

bool input_touchpad(touchpad_report_t* out)
{
    touchpad_scan(out);
    return out->contact;
}
