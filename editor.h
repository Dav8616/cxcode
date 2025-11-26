#ifndef EDITOR_H
#define EDITOR_H

#include "theme.h"
#include "clipboard.h"
#include "undo.h"
#include "mouse.h"

#define MAX_LINES 500
#define MAX_LINE_LENGTH 128

typedef enum
{
    MODE_EDIT,
    MODE_MENU,
    MODE_SAVE_DIALOG,
    MODE_OPEN_DIALOG,
    MODE_HELP,
    MODE_THEME_EDITOR
} EditorMode;

typedef enum
{
    MENU_NONE = -1,
    MENU_FILE = 0,
    MENU_EDIT = 1,
    MENU_VIEW = 2,
    MENU_HELP = 3
} MenuType;

typedef struct
{
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_count;
    int cursor_x;
    int cursor_y;
    int scroll_y;
    int modified;
    char filename[128];
    EditorMode mode;
    MenuType active_menu;
    int menu_selection;
    char input_buffer[128];
    int input_cursor;
    char status_msg[64];
    int status_timeout;
    MouseState mouse;
    Theme theme;
    Clipboard clipboard;
    UndoStack undo;
    int help_page;
    int show_line_numbers;
    int theme_edit_index;   // which theme field (0–12)
    int theme_edit_channel; // 0 = R, 1 = G, 2 = B

} Editor;

void editor_init(Editor *ed);
void editor_draw(Editor *ed);
void editor_free(Editor *ed);
void editor_handle_key(Editor *ed, int key, int ctrl_pressed);
void insert_char(Editor *ed, char c);
void delete_char(Editor *ed);
void new_line(Editor *ed);
int save_file(Editor *ed);
void load_file(Editor *ed, const char *filename);
void new_file(Editor *ed);
void set_status(Editor *ed, const char *msg);
void save_undo_state(Editor *ed);
void perform_undo(Editor *ed);
void perform_redo(Editor *ed);
void copy_selection(Editor *ed);
void paste_clipboard(Editor *ed);
void clamp_cursor(Editor *ed);
void editor_handle_click(Editor *ed, int x, int y);

#endif
