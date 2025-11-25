#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#define CLIPBOARD_SIZE 4096

typedef struct {
    char data[CLIPBOARD_SIZE];
    int length;
} Clipboard;

void clipboard_init(Clipboard* cb);
void clipboard_copy(Clipboard* cb, const char* text, int len);
const char* clipboard_paste(Clipboard* cb);

#endif