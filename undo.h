#ifndef UNDO_H
#define UNDO_H

#include <stddef.h>

#define MAX_UNDO_STATES 50
#define MAX_LINES 500
#define MAX_LINE_LENGTH 128

typedef struct {
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_count;
    int cursor_x;
    int cursor_y;
} EditorState;

typedef struct {
    EditorState* states;   // dynamic
    int current;
    int count;
    int capacity;
} UndoStack;

void undo_init(UndoStack* undo);               // allocates states
void undo_free(UndoStack* undo);               // frees states
void undo_push(UndoStack* undo, EditorState* state);
EditorState* undo_pop(UndoStack* undo);
EditorState* redo_pop(UndoStack* undo);

#endif
