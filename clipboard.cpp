#include "clipboard.h"
#include <string.h>

void clipboard_init(Clipboard* cb) {
    cb->data[0] = '\0';
    cb->length = 0;
}

void clipboard_copy(Clipboard* cb, const char* text, int len) {
    if (len >= CLIPBOARD_SIZE) len = CLIPBOARD_SIZE - 1;
    memcpy(cb->data, text, len);
    cb->data[len] = '\0';
    cb->length = len;
}

const char* clipboard_paste(Clipboard* cb) {
    return cb->data;
}
