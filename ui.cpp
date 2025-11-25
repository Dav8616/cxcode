#include "editor.h"
#include "font.h"
#include <stdio.h>
#include <string.h>

void editor_draw(Editor* ed);

void draw_menubar(Editor* ed) {
    draw_filled_rect(0, 0, 320, 10, ed->theme.title_bar);
    
    const char* menus[] = {"File", "Edit", "View", "Help"};
    int x = 2;
    
    for (int i = 0; i < 4; i++) {
        Uint16 color = (ed->active_menu == i) ? ed->theme.menu_selected : ed->theme.text;
        draw_string(x, 1, menus[i], color);
        x += strlen(menus[i]) * 6 + 8;
    }
    
    // Filename
    draw_string(160, 1, ed->filename[0] ? ed->filename : "untitled.tns", ed->theme.text);
    if (ed->modified) draw_string(308, 1, "*", ed->theme.text);
}

void draw_dropdown_menu(Editor* ed) {
    if (ed->active_menu == MENU_NONE) return;
    
    const char* file_items[] = {"Save", "Save As", "Open", "New File", "Exit"};
    const char* edit_items[] = {"Undo", "Redo", "Copy Line", "Paste", "Preferences"};
    const char* view_items[] = {"Toggle Line Nums", "Load Theme", "Save Theme"};
    const char* help_items[] = {"Keybinds", "About"};
    
    const char** items = NULL;
    int count = 0;
    int x_pos = 2;
    
    switch (ed->active_menu) {
        case MENU_FILE:
            items = file_items;
            count = 5;
            x_pos = 2;
            break;
        case MENU_EDIT:
            items = edit_items;
            count = 5;
            x_pos = 34;
            break;
        case MENU_VIEW:
            items = view_items;
            count = 3;
            x_pos = 66;
            break;
        case MENU_HELP:
            items = help_items;
            count = 2;
            x_pos = 98;
            break;
        default:
            return;
    }
    
    int width = 100;
    int height = count * 12 + 4;
    
    draw_filled_rect(x_pos, 10, width, height, ed->theme.menu_border);
    draw_filled_rect(x_pos + 1, 11, width - 2, height - 2, ed->theme.menu_bg);
    
    for (int i = 0; i < count; i++) {
        if (i == ed->menu_selection) {
            draw_filled_rect(x_pos + 2, 12 + i * 12, width - 4, 10, ed->theme.menu_selected);
        }
        draw_string(x_pos + 4, 13 + i * 12, items[i], ed->theme.text);
    }
}

void draw_help_screen(Editor* ed) {
    draw_filled_rect(20, 30, 280, 180, ed->theme.menu_border);
    draw_filled_rect(22, 32, 276, 176, ed->theme.menu_bg);
    
    draw_string(120, 40, "CXCode Help", ed->theme.text);
    draw_filled_rect(30, 50, 260, 1, ed->theme.menu_border);
    
    if (ed->help_page == 0) {
        draw_string(30, 60, "KEYBINDS:", ed->theme.text);
        draw_string(30, 72, "Ctrl+O - File Menu", ed->theme.text);
        draw_string(30, 84, "Ctrl+S - Quick Save", ed->theme.text);
        draw_string(30, 96, "Ctrl+Z - Undo", ed->theme.text);
        draw_string(30, 108, "Ctrl+Y - Redo", ed->theme.text);
        draw_string(30, 120, "Ctrl+C - Copy Line", ed->theme.text);
        draw_string(30, 132, "Ctrl+V - Paste", ed->theme.text);
        draw_string(30, 144, "Arrow Keys - Navigate", ed->theme.text);
        draw_string(30, 156, "Tab - Insert 4 Spaces", ed->theme.text);
        draw_string(30, 168, "ESC - Close Menu/Exit", ed->theme.text);
    } else {
        draw_string(30, 60, "ABOUT:", ed->theme.text);
        draw_string(30, 72, "CXCode IDE v1.0", ed->theme.text);
        draw_string(30, 84, "A text editor for", ed->theme.text);
        draw_string(30, 96, "TI-Nspire CX", ed->theme.text);
        draw_string(30, 120, "Features:", ed->theme.text);
        draw_string(30, 132, "- Syntax highlighting", ed->theme.text);
        draw_string(30, 144, "- Custom themes", ed->theme.text);
        draw_string(30, 156, "- Undo/Redo", ed->theme.text);
        draw_string(30, 168, "- Clipboard support", ed->theme.text);
    }
    
    draw_string(60, 190, "Arrow Keys: Switch Page", ed->theme.text);
    draw_string(60, 200, "ESC: Close", ed->theme.text);
}

void draw_input_dialog(const char* title, Editor* ed) {
    draw_filled_rect(20, 80, 280, 70, ed->theme.menu_border);
    draw_filled_rect(22, 82, 276, 66, ed->theme.menu_bg);
    
    draw_string(30, 90, title, ed->theme.text);
    draw_filled_rect(30, 102, 260, 1, ed->theme.menu_border);
    
    draw_filled_rect(30, 110, 260, 12, ed->theme.title_bar);
    draw_string(32, 112, ed->input_buffer, ed->theme.text);
    
    int cursor_x = 32 + ed->input_cursor * 6;
    draw_filled_rect(cursor_x, 112, 2, 8, ed->theme.cursor);
    
    draw_string(30, 128, "Enter=OK ESC=Cancel", ed->theme.text);
    draw_string(30, 138, ".tns auto-appended", ed->theme.text);
}

void editor_draw(Editor* ed) {
    SDL_FillRect(screen, NULL, ed->theme.bg);
    
    if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
    
    // Menu bar
    draw_menubar(ed);
    
    // Draw visible lines
    int line_offset = ed->show_line_numbers ? 26 : 2;
    int visible_lines = (240 - 22) / 8;
    for (int i = 0; i < visible_lines && (ed->scroll_y + i) < ed->line_count; i++) {
        int line_idx = ed->scroll_y + i;
        int screen_y = 12 + i * 8;
        
        if (ed->show_line_numbers) {
            char line_num[16];
            snprintf(line_num, sizeof(line_num), "%3d", line_idx + 1);
            draw_string(2, screen_y, line_num, ed->theme.line_num);
        }
        
        draw_string(line_offset, screen_y, ed->lines[line_idx], ed->theme.text);
    }
    
    // Cursor
    int cursor_screen_y = 12 + (ed->cursor_y - ed->scroll_y) * 8;
    if (cursor_screen_y >= 12 && cursor_screen_y < 228) {
        int cursor_x = line_offset + ed->cursor_x * 6;
        draw_filled_rect(cursor_x, cursor_screen_y, 2, 8, ed->theme.cursor);
    }
    
    // Status bar
    draw_filled_rect(0, 228, 320, 12, ed->theme.status_bar);
    char status[64];
    if (ed->status_timeout > 0) {
        draw_string(2, 230, ed->status_msg, ed->theme.cursor);
        ed->status_timeout--;
    } else {
        snprintf(status, sizeof(status), "Ln%d Col%d", ed->cursor_y + 1, ed->cursor_x + 1);
        draw_string(2, 230, status, ed->theme.text);
    }
    
    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    
    // Draw overlays
    if (ed->active_menu != MENU_NONE && ed->mode == MODE_EDIT) {
        draw_dropdown_menu(ed);
    } else if (ed->mode == MODE_SAVE_DIALOG) {
        draw_input_dialog("Save As:", ed);
    } else if (ed->mode == MODE_OPEN_DIALOG) {
        draw_input_dialog("Open File:", ed);
    } else if (ed->mode == MODE_HELP) {
        draw_help_screen(ed);
    }

    mouse_draw(&ed->mouse);
    
    SDL_Flip(screen);
}

void draw_theme_editor(Editor* ed)
{
    draw_filled_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ed->theme.bg);
    
    for (int i = 0; i < THEME_FIELD_COUNT; i++)
    {
        int y = 20 + i * 12;

        int r,g,b;
        Uint16* col = theme_get_field(&ed->theme, i);
        rgb_from_565(*col, &r, &g, &b);

        char buf[64];
        snprintf(buf, sizeof(buf), "%s: R=%d G=%d B=%d",
                 theme_keys[i], r, g, b);

        draw_string(10, y, buf,
            (i == ed->theme_edit_index) ? ed->theme.menu_selected : ed->theme.text);
    }

    draw_string(10, SCREEN_HEIGHT - 14,
        "Tab = Chanel | Enter = Save | ESC = Back",
        ed->theme.status_bar);
}
