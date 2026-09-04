#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 100
#define MAX_LINE_LEN 256

typedef struct {
    char *lines[MAX_LINES];
    int count;
} Document;

typedef enum { ACTION_NONE, ACTION_INSERT, ACTION_DELETE } ActionType;

typedef struct {
    ActionType type;
    int line_number;          
    char text[MAX_LINE_LEN];
} LastAction;


void init_document(Document *doc) {
    doc->count = 0;
}


void insert_line_ex(Document *doc, int n, const char *text, LastAction *last, int record_undo) {
    if (n < 0 || n > doc->count) {
        printf("Error: invalid line number %d\n", n);
        return;
    }
    if (doc->count >= MAX_LINES) {
        printf("Error: document full (max %d lines)\n", MAX_LINES);
        return;
    }

    
    for (int i = doc->count; i > n; i--) {
        doc->lines[i] = doc->lines[i - 1];
    }

    
    doc->lines[n] = malloc(strlen(text) + 1);
    if (doc->lines[n] == NULL) {
        printf("Error: memory allocation failed\n");
        return;
    }
    strcpy(doc->lines[n], text);
    doc->count++;

    if (record_undo && last != NULL) {
        last->type = ACTION_INSERT;
        last->line_number = n;
        last->text[0] = '\0'; 
    }
}

void delete_line_ex(Document *doc, int n, LastAction *last, int record_undo) {
    if (n < 0 || n >= doc->count) {
        printf("Error: invalid line number %d\n", n);
        return;
    }

    if (record_undo && last != NULL) {
        last->type = ACTION_DELETE;
        last->line_number = n;
        strncpy(last->text, doc->lines[n], MAX_LINE_LEN - 1);
        last->text[MAX_LINE_LEN - 1] = '\0';
    }

    free(doc->lines[n]);

    
    for (int i = n; i < doc->count - 1; i++) {
        doc->lines[i] = doc->lines[i + 1];
    }
    doc->count--;
}


void insert_line(Document *doc, int n, const char *text) {
    insert_line_ex(doc, n, text, NULL, 0);
}

void delete_line(Document *doc, int n) {
    delete_line_ex(doc, n, NULL, 0);
}

void undo_last(Document *doc, LastAction *last) {
    if (last->type == ACTION_NONE) {
        printf("Nothing to undo.\n");
        return;
    }
    if (last->type == ACTION_INSERT) {
        
        delete_line_ex(doc, last->line_number, NULL, 0);
        printf("Undid insert at line %d\n", last->line_number + 1);
    } else if (last->type == ACTION_DELETE) {
        
        insert_line_ex(doc, last->line_number, last->text, NULL, 0);
        printf("Undid delete at line %d\n", last->line_number + 1);
    }
    last->type = ACTION_NONE; 
}


void find_word(const Document *doc, const char *word) {
    int found = 0;
    for (int i = 0; i < doc->count; i++) {
        if (strstr(doc->lines[i], word) != NULL) {
            printf("Found on line %d: %s\n", i + 1, doc->lines[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("\"%s\" not found.\n", word);
    }
}

/* builds a new string with every occurrence of old_word replaced by new_word */
void replace_in_string(const char *line, const char *old_word, const char *new_word, char *result) {
    int result_pos = 0;
    int old_len = strlen(old_word);
    int i = 0;
    int line_len = strlen(line);

    while (i < line_len) {
        if (strncmp(&line[i], old_word, old_len) == 0 && old_len > 0) {
            strcpy(&result[result_pos], new_word);
            result_pos += strlen(new_word);
            i += old_len;
        } else {
            result[result_pos++] = line[i++];
        }
    }
    result[result_pos] = '\0';
}


void replace_word(Document *doc, int n, const char *old_word, const char *new_word) {
    char buffer[MAX_LINE_LEN * 2]; 

    if (n == -1) {
        for (int i = 0; i < doc->count; i++) {
            replace_in_string(doc->lines[i], old_word, new_word, buffer);
            free(doc->lines[i]);
            doc->lines[i] = malloc(strlen(buffer) + 1);
            strcpy(doc->lines[i], buffer);
        }
        printf("Replaced \"%s\" with \"%s\" across the document.\n", old_word, new_word);
        return;
    }

    if (n < 0 || n >= doc->count) {
        printf("Error: invalid line number %d\n", n + 1);
        return;
    }
    replace_in_string(doc->lines[n], old_word, new_word, buffer);
    free(doc->lines[n]);
    doc->lines[n] = malloc(strlen(buffer) + 1);
    strcpy(doc->lines[n], buffer);
    printf("Replaced on line %d.\n", n + 1);
}

void display_document(const Document *doc) {
    if (doc->count == 0) {
        printf("(empty document)\n");
        return;
    }
    for (int i = 0; i < doc->count; i++) {
        printf("%d: %s\n", i + 1, doc->lines[i]);
    }
}

void save_document(const Document *doc, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: could not open %s for writing\n", filename);
        return;
    }
    for (int i = 0; i < doc->count; i++) {
        fprintf(fp, "%s\n", doc->lines[i]);
    }
    fclose(fp);
    printf("Saved to %s\n", filename);
}

void load_document(Document *doc, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: could not open %s for reading\n", filename);
        return;
    }

    
    for (int i = 0; i < doc->count; i++) {
        free(doc->lines[i]);
    }
    doc->count = 0;

    char buffer[MAX_LINE_LEN];
    while (fgets(buffer, sizeof(buffer), fp) != NULL && doc->count < MAX_LINES) {
        buffer[strcspn(buffer, "\n")] = '\0'; /* strip trailing newline */
        doc->lines[doc->count] = malloc(strlen(buffer) + 1);
        strcpy(doc->lines[doc->count], buffer);
        doc->count++;
    }
    fclose(fp);
    printf("Loaded from %s\n", filename);
}

void free_document(Document *doc) {
    for (int i = 0; i < doc->count; i++) {
        free(doc->lines[i]);
    }
    doc->count = 0;
}



void print_help(void) {
    printf("Commands:\n");
    printf("  i <n> <text>   insert text at line n\n");
    printf("  d <n>          delete line n\n");
    printf("  p              print/display the document\n");
    printf("  s <filename>   save document to file\n");
    printf("  l <filename>   load document from file\n");
    printf("  f <word>       find lines containing word\n");
    printf("  r <n> <old> <new>   replace old with new on line n\n");
    printf("  r all <old> <new>   replace old with new across whole doc\n");
    printf("  u              undo last insert/delete\n");
    printf("  h              show this help\n");
    printf("  q              quit\n");
}

int main(void) {
    Document doc;
    init_document(&doc);

    LastAction last;
    last.type = ACTION_NONE;

    char input[512];
    print_help();

    while (1) {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0'; /* strip newline */

        if (strlen(input) == 0) continue;

        char command = input[0];

        if (command == 'q') {
            break;
        } else if (command == 'p') {
            display_document(&doc);
        } else if (command == 'h') {
            print_help();
        } else if (command == 'i') {
            int n;
            char text[MAX_LINE_LEN];
            /* input looks like: i 2 hello world */
            if (sscanf(input, "i %d %[^\n]", &n, text) == 2) {
                insert_line_ex(&doc, n - 1, text, &last, 1); /* user types 1-based line numbers */
            } else {
                printf("Usage: i <line_number> <text>\n");
            }
        } else if (command == 'd') {
            int n;
            if (sscanf(input, "d %d", &n) == 1) {
                delete_line_ex(&doc, n - 1, &last, 1);
            } else {
                printf("Usage: d <line_number>\n");
            }
        } else if (command == 'u') {
            undo_last(&doc, &last);
        } else if (command == 'f') {
            char word[MAX_LINE_LEN];
            if (sscanf(input, "f %[^\n]", word) == 1) {
                find_word(&doc, word);
            } else {
                printf("Usage: f <word>\n");
            }
        } else if (command == 'r') {
            char first[32], old_word[MAX_LINE_LEN], new_word[MAX_LINE_LEN];
            if (sscanf(input, "r %s %s %[^\n]", first, old_word, new_word) == 3) {
                if (strcmp(first, "all") == 0) {
                    replace_word(&doc, -1, old_word, new_word);
                } else {
                    int n = atoi(first);
                    replace_word(&doc, n - 1, old_word, new_word);
                }
            } else {
                printf("Usage: r <line_number|all> <old_word> <new_word>\n");
            }
        } else if (command == 's') {
            char filename[128];
            if (sscanf(input, "s %s", filename) == 1) {
                save_document(&doc, filename);
            } else {
                printf("Usage: s <filename>\n");
            }
        } else if (command == 'l') {
            char filename[128];
            if (sscanf(input, "l %s", filename) == 1) {
                load_document(&doc, filename);
            } else {
                printf("Usage: l <filename>\n");
            }
        } else {
            printf("Unknown command. Type 'h' for help.\n");
        }
    }

    free_document(&doc);
    return 0;
}