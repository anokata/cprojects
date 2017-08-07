#pragma once

#include <glib.h>
#include <stdio.h>
#include "note.h"

const char *notebooks_path = "./nb/"; // ~/.notebooks/

typedef struct NotesBook {
    GList *notes;
    char *name;
} *NotesBook;

NotesBook nbook_new(char *name);
void nbook_free(NotesBook book);

void nbook_add_note(NotesBook book, Note note);
void nbook_print(NotesBook book);
