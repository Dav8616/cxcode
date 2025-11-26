#include "editor.h"
#include "font.h"
#include "mouse.h"
#include <SDL/SDL.h>
#include <string.h>
#include <stdio.h>


// safer signature: pass buffer size so we don't overflow
void ensure_tns_extension(char *filename, size_t bufsize)
{
    if (!filename || bufsize == 0)
        return;
    size_t len = strlen(filename);
    if (len >= 4 && strcmp(&filename[len - 4], ".tns") == 0)
        return;

    // find last dot but don't scan past start or slash
    int dot = -1;
    for (int i = (int)len - 1; i >= 0; --i)
    {
        if (filename[i] == '.')
        {
            dot = i;
            break;
        }
        if (filename[i] == '/')
            break;
    }
    if (dot >= 0)
        filename[dot] = '\0';

    // append extension safely
    const char *ext = ".tns";
    if (strlen(filename) + strlen(ext) + 1 <= bufsize)
    {
        strcat(filename, ext);
    }
    else
    {
        // can't append, leave as-is (caller should handle)
    }
}

void editor_init(Editor *ed)
{
    memset(ed, 0, sizeof(Editor));
    ed->line_count = 1;
    strcpy(ed->lines[0], "// CXCode IDE");
    strcpy(ed->filename, "untitled.tns");
    ed->mode = MODE_EDIT;
    ed->active_menu = MENU_NONE;
    ed->show_line_numbers = 1;
    strcpy(ed->status_msg, "");
    ed->status_timeout = 0;
    ed->help_page = 0;

    theme_init_default(&ed->theme);
    theme_load(&ed->theme, "/documents/ndless/cxcode.theme.tns");

    clipboard_init(&ed->clipboard);
    undo_init(&ed->undo);
    mouse_init(&ed->mouse);

    save_undo_state(ed);
}

void set_status(Editor *ed, const char *msg)
{
    strncpy(ed->status_msg, msg, sizeof(ed->status_msg) - 1);
    ed->status_timeout = 60;
}

void save_undo_state(Editor *ed)
{
    EditorState state;
    memcpy(state.lines, ed->lines, sizeof(ed->lines));
    state.line_count = ed->line_count;
    state.cursor_x = ed->cursor_x;
    state.cursor_y = ed->cursor_y;
    undo_push(&ed->undo, &state);
}

void perform_undo(Editor *ed)
{
    EditorState *state = undo_pop(&ed->undo);
    if (state)
    {
        memcpy(ed->lines, state->lines, sizeof(ed->lines));
        ed->line_count = state->line_count;
        ed->cursor_x = state->cursor_x;
        ed->cursor_y = state->cursor_y;
        set_status(ed, "Undo");
    }
}

void perform_redo(Editor *ed)
{
    EditorState *state = redo_pop(&ed->undo);
    if (state)
    {
        memcpy(ed->lines, state->lines, sizeof(ed->lines));
        ed->line_count = state->line_count;
        ed->cursor_x = state->cursor_x;
        ed->cursor_y = state->cursor_y;
        set_status(ed, "Redo");
    }
}

void copy_selection(Editor *ed)
{
    // Copy current line for now (selection can be added later)
    clipboard_copy(&ed->clipboard, ed->lines[ed->cursor_y], strlen(ed->lines[ed->cursor_y]));
    set_status(ed, "Copied");
}

void paste_clipboard(Editor *ed)
{
    const char *text = clipboard_paste(&ed->clipboard);
    for (int i = 0; text[i]; i++)
    {
        insert_char(ed, text[i]);
    }
    set_status(ed, "Pasted");
}

int save_file(Editor *ed)
{
    if (!ed->filename[0])
    {
        strcpy(ed->filename, "/documents/code.tns");
    }

    ensure_tns_extension(ed->filename, sizeof(ed->filename));

    FILE *f = fopen(ed->filename, "wb");
    if (f == NULL)
    {
        set_status(ed, "Save failed!");
        return 0;
    }

    for (int i = 0; i < ed->line_count; i++)
    {
        int len = strlen(ed->lines[i]);
        if (len > 0)
        {
            fwrite(ed->lines[i], 1, len, f);
        }
        fwrite("\n", 1, 1, f);
    }

    fclose(f);
    ed->modified = 0;
    set_status(ed, "Saved!");
    return 1;
}

void load_file(Editor *ed, const char *filename)
{
    char path[256];
    if (!filename || !filename[0])
    {
        set_status(ed, "No file specified");
        return;
    }

    // copy safely
    strncpy(path, filename, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';

    ensure_tns_extension(path, sizeof(path));

    FILE *f = fopen(path, "rb");
    if (f)
    {
        ed->line_count = 0;
        while (fgets(ed->lines[ed->line_count], MAX_LINE_LENGTH, f) &&
               ed->line_count < MAX_LINES)
        {
            int len = strlen(ed->lines[ed->line_count]);
            if (len > 0 && ed->lines[ed->line_count][len - 1] == '\n')
            {
                ed->lines[ed->line_count][len - 1] = 0;
            }
            if (len > 1 && ed->lines[ed->line_count][len - 2] == '\r')
            {
                ed->lines[ed->line_count][len - 2] = 0;
            }
            ed->line_count++;
        }
        fclose(f);
        if (ed->line_count == 0)
        {
            ed->line_count = 1;
            ed->lines[0][0] = 0;
        }
        strncpy(ed->filename, path, sizeof(ed->filename) - 1);
        ed->filename[sizeof(ed->filename) - 1] = '\0';
        ed->cursor_x = 0;
        ed->cursor_y = 0;
        ed->scroll_y = 0;
        ed->modified = 0;
        set_status(ed, "Loaded!");
    }
    else
    {
        set_status(ed, "Load failed!");
    }
}

void new_file(Editor *ed)
{
    ed->line_count = 1;
    ed->lines[0][0] = 0;
    ed->cursor_x = 0;
    ed->cursor_y = 0;
    ed->scroll_y = 0;
    ed->modified = 0;
    strcpy(ed->filename, "untitled.tns");
}

void insert_char(Editor *ed, char c)
{
    int len = strlen(ed->lines[ed->cursor_y]);
    if (len >= MAX_LINE_LENGTH - 1)
        return;

    for (int i = len; i >= ed->cursor_x; i--)
    {
        ed->lines[ed->cursor_y][i + 1] = ed->lines[ed->cursor_y][i];
    }

    ed->lines[ed->cursor_y][ed->cursor_x] = c;
    ed->cursor_x++;
    ed->modified = 1;
}

void delete_char(Editor *ed)
{
    if (ed->cursor_x == 0)
    {
        if (ed->cursor_y == 0)
            return;

        int prev_len = strlen(ed->lines[ed->cursor_y - 1]);
        int curr_len = strlen(ed->lines[ed->cursor_y]);

        if (prev_len + curr_len < MAX_LINE_LENGTH)
        {
            for (int i = 0; i <= curr_len; i++)
            {
                ed->lines[ed->cursor_y - 1][prev_len + i] = ed->lines[ed->cursor_y][i];
            }

            for (int i = ed->cursor_y; i < ed->line_count - 1; i++)
            {
                strcpy(ed->lines[i], ed->lines[i + 1]);
            }
            ed->line_count--;
            ed->cursor_y--;
            ed->cursor_x = prev_len;
            ed->modified = 1;
        }
    }
    else
    {
        int len = strlen(ed->lines[ed->cursor_y]);
        for (int i = ed->cursor_x - 1; i < len; i++)
        {
            ed->lines[ed->cursor_y][i] = ed->lines[ed->cursor_y][i + 1];
        }
        ed->cursor_x--;
        ed->modified = 1;
    }
}

void new_line(Editor *ed)
{
    if (ed->line_count >= MAX_LINES - 1)
        return;

    for (int i = ed->line_count; i > ed->cursor_y; i--)
    {
        strcpy(ed->lines[i], ed->lines[i - 1]);
    }
    ed->line_count++;

    char *current = ed->lines[ed->cursor_y];
    strcpy(ed->lines[ed->cursor_y + 1], &current[ed->cursor_x]);
    current[ed->cursor_x] = 0;

    ed->cursor_y++;
    ed->cursor_x = 0;
    ed->modified = 1;
}

void editor_free(Editor *ed)
{
    if (!ed)
        return;
    undo_free(&ed->undo);
    // if clipboard or theme need freeing in future, do it here
}

void clamp_cursor(Editor *ed)
{
    if (ed->cursor_y < 0)
        ed->cursor_y = 0;
    if (ed->cursor_y >= ed->line_count)
        ed->cursor_y = ed->line_count - 1;

    int line_len = strlen(ed->lines[ed->cursor_y]);
    if (ed->cursor_x < 0)
        ed->cursor_x = 0;
    if (ed->cursor_x > line_len)
        ed->cursor_x = line_len;
}

void handle_menu_action(Editor *ed);
void handle_dialog_input(Editor *ed, int key, EditorMode dialog_mode);
void handle_theme_editor_input(Editor *ed, int key);

void editor_handle_key(Editor *ed, int key, int ctrl_pressed)
{
    // Handle help screen
    if (ed->mode == MODE_HELP)
    {
        if (key == SDLK_ESCAPE)
        {
            ed->mode = MODE_EDIT;
        }
        else if (key == SDLK_LEFT || key == SDLK_RIGHT)
        {
            ed->help_page = 1 - ed->help_page;
        }
        return;
    }

    // Handle dropdown menus
    if (ed->active_menu != MENU_NONE && ed->mode == MODE_EDIT)
    {
        if (key == SDLK_ESCAPE)
        {
            ed->active_menu = MENU_NONE;
        }
        else if (key == SDLK_UP || key == SDLK_DOWN)
        {
            int max = 0;
            switch (ed->active_menu)
            {
            case MENU_FILE:
                max = 5;
                break;
            case MENU_EDIT:
                max = 5;
                break;
            case MENU_VIEW:
                max = 3;
                break;
            case MENU_HELP:
                max = 2;
                break;
            default:
                max = 0;
            }
            if (key == SDLK_UP)
                ed->menu_selection = (ed->menu_selection - 1 + max) % max;
            else
                ed->menu_selection = (ed->menu_selection + 1) % max;
        }
        else if (key == SDLK_LEFT)
        {
            ed->active_menu = (MenuType)((ed->active_menu - 1 + 4) % 4);
            ed->menu_selection = 0;
        }
        else if (key == SDLK_RIGHT)
        {
            ed->active_menu = (MenuType)((ed->active_menu + 1) % 4);
            ed->menu_selection = 0;
        }
        else if (key == SDLK_RETURN)
        {
            handle_menu_action(ed);
        }
        return;
    }

    // Handle dialogs
    if (ed->mode == MODE_SAVE_DIALOG || ed->mode == MODE_OPEN_DIALOG)
    {
        handle_dialog_input(ed, key, ed->mode);
        return;
    }

    // Normal editing
    int len = strlen(ed->lines[ed->cursor_y]);

    if (ctrl_pressed && key == SDLK_s)
    {
        save_undo_state(ed);
        save_file(ed);
    }
    else if (ctrl_pressed && key == SDLK_z)
    {
        perform_undo(ed);
    }
    else if (ctrl_pressed && key == SDLK_y)
    {
        perform_redo(ed);
    }
    else if (ctrl_pressed && key == SDLK_c)
    {
        copy_selection(ed);
    }
    else if (ctrl_pressed && key == SDLK_v)
    {
        save_undo_state(ed);
        paste_clipboard(ed);
    }
    else if (key == SDLK_LEFT && ed->cursor_x > 0)
        ed->cursor_x--;
    else if (key == SDLK_RIGHT && ed->cursor_x < len)
        ed->cursor_x++;
    else if (key == SDLK_UP && ed->cursor_y > 0)
    {
        ed->cursor_y--;
        if (ed->cursor_x > (int)strlen(ed->lines[ed->cursor_y]))
            ed->cursor_x = strlen(ed->lines[ed->cursor_y]);
    }
    else if (key == SDLK_DOWN && ed->cursor_y < ed->line_count - 1)
    {
        ed->cursor_y++;
        if (ed->cursor_x > (int)strlen(ed->lines[ed->cursor_y]))
            ed->cursor_x = strlen(ed->lines[ed->cursor_y]);
    }
    else if (key == SDLK_RETURN)
    {
        save_undo_state(ed);
        new_line(ed);
    }
    else if (key == SDLK_BACKSPACE)
    {
        save_undo_state(ed);
        delete_char(ed);
    }
    else if (key == SDLK_TAB)
    {
        save_undo_state(ed);
        for (int i = 0; i < 4; i++)
            insert_char(ed, ' ');
    }
    else if (key >= 32 && key <= 126)
    {
        if (ed->undo.current == -1 || ed->undo.current == ed->undo.count - 1)
            save_undo_state(ed);
        insert_char(ed, (char)key);
    }

    // Auto-scroll
    int visible = (240 - 22) / 8;
    if (ed->cursor_y < ed->scroll_y)
        ed->scroll_y = ed->cursor_y;
    if (ed->cursor_y >= ed->scroll_y + visible)
        ed->scroll_y = ed->cursor_y - visible + 1;

    if (ed->mode == MODE_THEME_EDITOR)
    {
        handle_theme_editor_input(ed, key);
        return;
    }
}

void handle_menu_action(Editor *ed)
{
    switch (ed->active_menu)
    {
    case MENU_FILE:
        switch (ed->menu_selection)
        {
        case 0:
            new_file(ed);
            break;
        case 1:
            ed->mode = MODE_OPEN_DIALOG;
            ed->input_buffer[0] = 0;
            ed->input_cursor = 0;
            break;
        case 2:
            ed->mode = MODE_SAVE_DIALOG;
            ed->input_buffer[0] = 0;
            ed->input_cursor = 0;
            break;
        case 3:
            save_file(ed);
            break;
        case 4:
            set_status(ed, "Quit: Press ESC");
            break;
        }
        break;

    case MENU_EDIT:
        switch (ed->menu_selection)
        {
        case 0:
            perform_undo(ed);
            break;
        case 1:
            perform_redo(ed);
            break;
        case 2:
            copy_selection(ed);
            break;
        case 3:
            paste_clipboard(ed);
            break;
        case 4:
            delete_char(ed);
            break;
        }
        break;

    case MENU_VIEW:
        switch (ed->menu_selection)
        {
        case 0:
            // Toggle line numbers
            ed->show_line_numbers = !ed->show_line_numbers;
            break;

        case 1:
            // Open in-program theme editor
            ed->mode = MODE_THEME_EDITOR;
            ed->theme_edit_index = 0;   // which theme field (bg, text, etc.)
            ed->theme_edit_channel = 0; // 0 = R, 1 = G, 2 = B
            set_status(ed, "Theme Editor");
            break;

        case 2:
            set_status(ed, "View refreshed");
            break;
        }
        break;

    case MENU_HELP:
        switch (ed->menu_selection)
        {
        case 0:
            ed->mode = MODE_HELP;
            break;
        case 1:
            set_status(ed, "CXCode IDE");
            break;
        }
        break;

    default:
        break;
    }

    // close menu after action
    ed->active_menu = MENU_NONE;
}

void handle_dialog_input(Editor *ed, int key, EditorMode dialog_mode)
{
    int len = strlen(ed->input_buffer);

    if (key == SDLK_ESCAPE)
    {
        ed->mode = MODE_EDIT;
        return;
    }

    if (key == SDLK_BACKSPACE && len > 0)
    {
        ed->input_buffer[len - 1] = 0;
        ed->input_cursor--;
        return;
    }

    if (key == SDLK_RETURN)
    {
        if (dialog_mode == MODE_SAVE_DIALOG)
        {
            strncpy(ed->filename, ed->input_buffer, sizeof(ed->filename) - 1);
            save_file(ed);
        }
        else if (dialog_mode == MODE_OPEN_DIALOG)
        {
            load_file(ed, ed->input_buffer);
        }

        ed->mode = MODE_EDIT;
        return;
    }

    // normal text input
    if (key >= 32 && key <= 126)
    {
        if (len < (int)sizeof(ed->input_buffer) - 1)
        {
            ed->input_buffer[len] = (char)key;
            ed->input_buffer[len + 1] = 0;
            ed->input_cursor++;
        }
    }
}

void handle_theme_editor_input(Editor *ed, int key)
{
    Uint16 *color = theme_get_field(&ed->theme, ed->theme_edit_index);
    if (!color) return;

    int r = 0, g = 0, b = 0;
    rgb_from_565(*color, &r, &g, &b);

    switch (key)
    {
        case SDLK_UP:
            ed->theme_edit_index--;
            if (ed->theme_edit_index < 0) ed->theme_edit_index = THEME_FIELD_COUNT - 1;
            break;

        case SDLK_DOWN:
            ed->theme_edit_index++;
            if (ed->theme_edit_index >= (int)THEME_FIELD_COUNT) ed->theme_edit_index = 0;
            break;

        case SDLK_LEFT:
        {
            int *chan = (ed->theme_edit_channel == 0) ? &r : (ed->theme_edit_channel == 1) ? &g : &b;
            *chan -= 5;
            if (*chan < 0) *chan = 0;
            *color = rgb_to_565(r, g, b);
            break;
        }

        case SDLK_RIGHT:
        {
            int *chan = (ed->theme_edit_channel == 0) ? &r : (ed->theme_edit_channel == 1) ? &g : &b;
            *chan += 5;
            if (*chan > 255) *chan = 255;
            *color = rgb_to_565(r, g, b);
            break;
        }

        case SDLK_TAB:
            ed->theme_edit_channel++;
            if (ed->theme_edit_channel > 2) ed->theme_edit_channel = 0;
            break;

        case SDLK_RETURN:
            theme_save(&ed->theme, "/documents/ndless/cxcode.theme.tns");
            set_status(ed, "Theme saved");
            break;

        case SDLK_ESCAPE:
            ed->mode = MODE_EDIT;
            break;

        default:
            // ignore other keys
            break;
    }
}

// Handles mouse clicks for menu interaction and cursor placement
void editor_handle_click(Editor *ed, int x, int y)
{
    // --- Zone 1: Menu Bar (Top 12 pixels) ---
    if (y < 12)
    {
        int item_width = 40;
        MenuType new_menu = MENU_NONE;
        
        // Determine which header was clicked
        if (x < item_width) new_menu = MENU_FILE;
        else if (x < item_width * 2) new_menu = MENU_EDIT;
        else if (x < item_width * 3) new_menu = MENU_VIEW;
        else if (x < item_width * 4) new_menu = MENU_HELP;
        
        if (new_menu != MENU_NONE) {
            if (new_menu == ed->active_menu) {
                // Clicked the currently active header -> Close it
                ed->active_menu = MENU_NONE;
            } else {
                // Clicked a different or closed header -> Open/Switch to it
                ed->active_menu = new_menu;
                ed->menu_selection = 0; // Reset selection
            }
        } else {
            // Clicked outside a menu header range, close any open menu
            ed->active_menu = MENU_NONE;
        }
    }

    // --- Zone 2: Dropdown Menu Area (Starts at y=12) ---
    else if (ed->active_menu != MENU_NONE && ed->mode == MODE_EDIT)
    {
        // A menu is open, check if the click is inside the dropdown box
        int max_items = 0;
        int menu_x_start = 0;
        
        // Get the dimensions and start position of the active dropdown
        switch (ed->active_menu)
        {
            case MENU_FILE: max_items = 5; menu_x_start = 0; break;
            case MENU_EDIT: max_items = 5; menu_x_start = 40; break;
            case MENU_VIEW: max_items = 3; menu_x_start = 80; break;
            case MENU_HELP: max_items = 2; menu_x_start = 120; break;
            default: ed->active_menu = MENU_NONE; return;
        }

        // Dropdown menu properties (Assumed MAX_MENU_WIDTH is 100)
        int dropdown_height = max_items * 8; // FONT_HEIGHT is 8
        int dropdown_width = 100;

        if (y < 12 + dropdown_height && x >= menu_x_start && x < menu_x_start + dropdown_width)
        {
            // Click is inside the active dropdown area
            int relative_y = y - 12;
            int selection = relative_y / 8; 
            
            if (selection >= 0 && selection < max_items)
            {
                ed->menu_selection = selection;
                handle_menu_action(ed); // Execute the selected action
            }
        }
        
        // If execution didn't happen (i.e., clicked outside the valid item area
        // but still within the dropdown region), or if an action was executed, close the menu.
        ed->active_menu = MENU_NONE;
    }

    // --- Zone 3: Text Editing Area (No Menu Open) ---
    else if (ed->mode == MODE_EDIT) 
    {
        // Handle cursor placement
        int line = (y - 12) / 8 + ed->scroll_y;
        
        // Calculate the column offset based on whether line numbers are visible
        // (Assuming line numbers take up 24px + 2px padding = 26px)
        int start_x = ed->show_line_numbers ? 26 : 2;
        int col = (x - start_x) / 6; // FONT_WIDTH is 6

        if (line >= 0 && line < ed->line_count)
        {
            ed->cursor_y = line;
            ed->cursor_x = col;
            clamp_cursor(ed);
        }
    }
}