#include "undo.h"
#include <string.h>
#include <stdlib.h>

void undo_init(UndoStack* undo) {
    if (!undo) return;
    undo->capacity = MAX_UNDO_STATES;
    undo->states = (EditorState*)malloc(sizeof(EditorState) * undo->capacity);
    if (!undo->states) {
        undo->capacity = 0;
    }
    undo->current = -1;
    undo->count = 0;
}

void undo_free(UndoStack* undo) {
    if (!undo) return;
    if (undo->states) free(undo->states);
    undo->states = NULL;
    undo->capacity = 0;
    undo->current = -1;
    undo->count = 0;
}

void undo_push(UndoStack* undo, EditorState* state) {
    if (!undo || !state) return;
    if (undo->capacity == 0 || !undo->states) return;

    // Remove any redo states
    undo->count = undo->current + 1;

    // If at capacity, shift left one element (drop oldest)
    if (undo->count >= undo->capacity) {
        memmove(&undo->states[0], &undo->states[1], sizeof(EditorState) * (undo->capacity - 1));
        undo->count = undo->capacity - 1;
        undo->current = undo->count - 1;
    }

    undo->current++;
    if (undo->current >= undo->capacity) undo->current = undo->capacity - 1;
    memcpy(&undo->states[undo->current], state, sizeof(EditorState));
    undo->count = undo->current + 1;
}

EditorState* undo_pop(UndoStack* undo) {
    if (!undo || !undo->states) return NULL;
    if (undo->current > 0) {
        undo->current--;
        return &undo->states[undo->current];
    }
    return NULL;
}

EditorState* redo_pop(UndoStack* undo) {
    if (!undo || !undo->states) return NULL;
    if (undo->current < undo->count - 1) {
        undo->current++;
        return &undo->states[undo->current];
    }
    return NULL;
}
