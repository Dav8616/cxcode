#include "theme.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* theme_keys[THEME_FIELD_COUNT] =
{
    "Background",
    "Text",
    "Line Numbers",
    "Cursor",
    "Title Bar",
    "Status Bar",
    "Menu BG",
    "Menu Border",
    "Menu Selected",
    "Keyword",
    "String",
    "Comment",
    "Number"
};

Uint16* theme_get_field(Theme* t, int index)
{
    switch (index)
    {
        case 0: return &t->bg;
        case 1: return &t->text;
        case 2: return &t->line_num;
        case 3: return &t->cursor;
        case 4: return &t->title_bar;
        case 5: return &t->status_bar;
        case 6: return &t->menu_bg;
        case 7: return &t->menu_border;
        case 8: return &t->menu_selected;
        case 9: return &t->keyword;
        case 10: return &t->string;
        case 11: return &t->comment;
        case 12: return &t->number;
    }

    return NULL;
}


Uint16 rgb_to_565(int r, int g, int b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void rgb_from_565(Uint16 color, int* r, int* g, int* b) {
    *r = (color >> 11) << 3;
    *g = ((color >> 5) & 0x3F) << 2;
    *b = (color & 0x1F) << 3;
}

void theme_init_default(Theme* theme) {
    theme->bg = rgb_to_565(0, 0, 0);           // Black
    theme->text = rgb_to_565(255, 255, 255);   // White
    theme->line_num = rgb_to_565(128, 128, 128); // Gray
    theme->cursor = rgb_to_565(255, 255, 0);   // Yellow
    theme->title_bar = rgb_to_565(32, 32, 32); // Dark gray
    theme->status_bar = rgb_to_565(32, 32, 32);
    theme->menu_bg = rgb_to_565(24, 24, 24);
    theme->menu_border = rgb_to_565(64, 64, 64);
    theme->menu_selected = rgb_to_565(0, 100, 200);
    theme->keyword = rgb_to_565(86, 156, 214);   // Blue
    theme->string = rgb_to_565(206, 145, 120);   // Orange
    theme->comment = rgb_to_565(106, 153, 85);   // Green
    theme->number = rgb_to_565(181, 206, 168);   // Light green
}

int theme_load(Theme* theme, const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        
        char key[64];
        int r, g, b;
        if (sscanf(line, "%63[^=]=%d,%d,%d", key, &r, &g, &b) == 4) {
            // Trim whitespace from key
            char* k = key;
            while (*k == ' ' || *k == '\t') k++;
            char* end = k + strlen(k) - 1;
            while (end > k && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) 
                *end-- = '\0';
            
            Uint16 color = rgb_to_565(r, g, b);
            
            if (strcmp(k, "bg") == 0) theme->bg = color;
            else if (strcmp(k, "text") == 0) theme->text = color;
            else if (strcmp(k, "line_num") == 0) theme->line_num = color;
            else if (strcmp(k, "cursor") == 0) theme->cursor = color;
            else if (strcmp(k, "title_bar") == 0) theme->title_bar = color;
            else if (strcmp(k, "status_bar") == 0) theme->status_bar = color;
            else if (strcmp(k, "menu_bg") == 0) theme->menu_bg = color;
            else if (strcmp(k, "menu_border") == 0) theme->menu_border = color;
            else if (strcmp(k, "menu_selected") == 0) theme->menu_selected = color;
            else if (strcmp(k, "keyword") == 0) theme->keyword = color;
            else if (strcmp(k, "string") == 0) theme->string = color;
            else if (strcmp(k, "comment") == 0) theme->comment = color;
            else if (strcmp(k, "number") == 0) theme->number = color;
        }
    }
    
    fclose(f);
    return 1;
}

int theme_save(Theme* theme, const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) return 0;
    
    fprintf(f, "# CXCode Theme File\n");
    fprintf(f, "# Format: key=R,G,B (0-255)\n\n");
    
    int r, g, b;
    
    rgb_from_565(theme->bg, &r, &g, &b);
    fprintf(f, "bg=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->text, &r, &g, &b);
    fprintf(f, "text=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->line_num, &r, &g, &b);
    fprintf(f, "line_num=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->cursor, &r, &g, &b);
    fprintf(f, "cursor=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->title_bar, &r, &g, &b);
    fprintf(f, "title_bar=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->status_bar, &r, &g, &b);
    fprintf(f, "status_bar=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->menu_bg, &r, &g, &b);
    fprintf(f, "menu_bg=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->menu_border, &r, &g, &b);
    fprintf(f, "menu_border=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->menu_selected, &r, &g, &b);
    fprintf(f, "menu_selected=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->keyword, &r, &g, &b);
    fprintf(f, "keyword=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->string, &r, &g, &b);
    fprintf(f, "string=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->comment, &r, &g, &b);
    fprintf(f, "comment=%d,%d,%d\n", r, g, b);
    
    rgb_from_565(theme->number, &r, &g, &b);
    fprintf(f, "number=%d,%d,%d\n", r, g, b);
    
    fclose(f);
    return 1;
}