#include "command.h"
#include "notesbook.h"
#define UNUSED(x) ((void)(x))

void print_help(char **params) {
    UNUSED(params);
    printf("Params: %s\n", params[0]);
    printf("help is\n");
}

void print_list(char **params) {
    UNUSED(params);
    UNUSED(notebooks_path);

    nbook_list();
}

char *_get_bookname(char **params) {
    char *bookname = params[0];
    if (bookname == NULL) {
        printf("Need more parametrs\n");
        exit(6);
    }
    return bookname;
}

void print_nbook(char **params) {
    NotesBook book = nbook_load(_get_bookname(params));
    nbook_print(book);
    nbook_free(book);
}

void cmd_book(char **params) {
    char *bookname = _get_bookname(params);
    nbook_file_new(bookname);
}

void cmd_unbook(char **params) {
    char *bookname = _get_bookname(params);
    nbook_file_rm(bookname);
}

void cmd_note(char **params) {
    char *bookname = _get_bookname(params);
    char *notename = _get_bookname(params + 1);
    printf("Creating note %s in book %s \n", notename, bookname);
    // load book, with check
    // Read stdin for text
    // create note
    // add to notebook
    // save
}

Command Commands[] = {
    {"help", print_help},
    {"list", print_list},
    {"show", print_nbook},
    {"book", cmd_book},
    {"unbook", cmd_unbook},
    {"note", cmd_note},
};

static int commands_count = sizeof(Commands)/sizeof(Command);

Command cmd_get(char *name) {
    for (int i = 0; i < commands_count; i++) {
        if (strcmp(Commands[i].name, name) == 0) {
            return Commands[i];
        }
    }
    return Commands[0];
}


