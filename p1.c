/* ============================================================
 * editor_full_simple.c — Line editor with ALL features from the
 * problem statement, written in the simplest style possible.
 *
 * Data structure: a FIXED-SIZE 2D array of characters.
 *   char lines[MAX_LINES][MAX_LEN];
 *   int count;   // how many lines are currently used
 *
 * No malloc, no pointers to worry about — just arrays and loops.
 *
 * Features:
 *   CORE:  insert, delete, display, save/load
 *   BONUS: search, find & replace, undo (one level), line/word count
 * ============================================================ */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 100
#define MAX_LEN   200

char lines[MAX_LINES][MAX_LEN];
int count = 0;   /* how many lines are currently in use */

/* ---- Undo support: keep ONE backup copy of the whole document ---- */
char backup_lines[MAX_LINES][MAX_LEN];
int backup_count = 0;
int have_backup = 0;   /* 1 if backup_lines is valid, 0 otherwise */

/* Copy the current document into the backup, before we change it. */
void save_backup(void) {
    for (int i = 0; i < count; i++) {
        strcpy(backup_lines[i], lines[i]);
    }
    backup_count = count;
    have_backup = 1;
}

/* Restore the document from the backup (undo). */
void undo(void) {
    if (!have_backup) {
        printf("Nothing to undo.\n");
        return;
    }
    for (int i = 0; i < backup_count; i++) {
        strcpy(lines[i], backup_lines[i]);
    }
    count = backup_count;
    have_backup = 0; /* only one level of undo */
    printf("Undo successful.\n");
}

/* ---- Insert text at line number `pos` (1-based) ---- */
void insert_line(int pos, char *text) {
    if (pos < 1 || pos > count + 1 || count >= MAX_LINES) {
        printf("Error: invalid line number.\n");
        return;
    }
    save_backup();
    /* shift everything from pos..count down by one, starting from the end */
    for (int i = count; i >= pos; i--) {
        strcpy(lines[i], lines[i - 1]);
    }
    strcpy(lines[pos - 1], text);
    count++;
    printf("Inserted at line %d.\n", pos);
}

/* ---- Delete line number `pos` (1-based) ---- */
void delete_line(int pos) {
    if (pos < 1 || pos > count) {
        printf("Error: invalid line number.\n");
        return;
    }
    save_backup();
    /* shift everything after pos up by one */
    for (int i = pos - 1; i < count - 1; i++) {
        strcpy(lines[i], lines[i + 1]);
    }
    count--;
    printf("Deleted line %d.\n", pos);
}

/* ---- Print the whole document ---- */
void display(void) {
    if (count == 0) {
        printf("(empty document)\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        printf("%d: %s\n", i + 1, lines[i]);
    }
}

/* ---- Save document to a file ---- */
void save_file(char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: could not open file.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s\n", lines[i]);
    }
    fclose(fp);
    printf("Saved to %s.\n", filename);
}

/* ---- Load document from a file ---- */
void load_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: could not open file.\n");
        return;
    }
    save_backup();
    count = 0;
    while (count < MAX_LINES && fgets(lines[count], MAX_LEN, fp) != NULL) {
        int len = strlen(lines[count]);
        if (len > 0 && lines[count][len - 1] == '\n') {
            lines[count][len - 1] = '\0';
        }
        count++;
    }
    fclose(fp);
    printf("Loaded from %s.\n", filename);
}

/* ---- Search for a word; print matching line numbers ---- */
void search(char *word) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strstr(lines[i], word) != NULL) {
            printf("Found on line %d\n", i + 1);
            found = 1;
        }
    }
    if (!found) printf("Not found.\n");
}

/* ---- Replace `oldw` with `neww` on ONE line (simple, single word) ---- */
void replace_on_line(int pos, char *oldw, char *neww) {
    if (pos < 1 || pos > count) {
        printf("Error: invalid line number.\n");
        return;
    }
    char *found = strstr(lines[pos - 1], oldw);
    if (found == NULL) {
        printf("'%s' not found on line %d.\n", oldw, pos);
        return;
    }
    save_backup();

    /* build the new line in a temporary buffer:
       [text before match] + [neww] + [text after match] */
    char result[MAX_LEN];
    int before_len = (int)(found - lines[pos - 1]);

    strncpy(result, lines[pos - 1], before_len);
    result[before_len] = '\0';
    strcat(result, neww);
    strcat(result, found + strlen(oldw));

    strcpy(lines[pos - 1], result);
    printf("Replaced on line %d.\n", pos);
}

/* ---- Replace `oldw` with `neww` across the WHOLE document ---- */
void replace_all(char *oldw, char *neww) {
    save_backup();
    int changed = 0;
    for (int i = 0; i < count; i++) {
        char *found = strstr(lines[i], oldw);
        if (found == NULL) continue;

        char result[MAX_LEN];
        int before_len = (int)(found - lines[i]);
        strncpy(result, lines[i], before_len);
        result[before_len] = '\0';
        strcat(result, neww);
        strcat(result, found + strlen(oldw));

        strcpy(lines[i], result);
        changed++;
    }
    printf("Replaced on %d line(s).\n", changed);
}

/* ---- Report line count / word count / character count ---- */
void show_count(void) {
    int words = 0;
    int chars = 0;
    for (int i = 0; i < count; i++) {
        chars += (int)strlen(lines[i]);
        int in_word = 0;
        for (int j = 0; lines[i][j] != '\0'; j++) {
            if (isspace((unsigned char)lines[i][j])) {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                words++;
            }
        }
    }
    printf("Lines: %d | Words: %d | Characters: %d\n", count, words, chars);
}

void print_help(void) {
    printf("Commands:\n");
    printf("  i <line#> <text>       insert a line\n");
    printf("  d <line#>              delete a line\n");
    printf("  p                      display document\n");
    printf("  s <filename>           save to file\n");
    printf("  o <filename>           open/load from file\n");
    printf("  f <word>               search for a word\n");
    printf("  r <line#> <old> <new>  replace on one line\n");
    printf("  R <old> <new>          replace across whole document\n");
    printf("  c                      show line/word/char count\n");
    printf("  u                      undo last change\n");
    printf("  h                      help\n");
    printf("  q                      quit\n");
}

int main(void) {
    char input[300];
    char cmd;
    char arg1[MAX_LEN], arg2[MAX_LEN], arg3[MAX_LEN];
    int lineno;

    printf("Simple Line Editor. Type 'h' for help.\n");

    while (1) {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        cmd = input[0];

        if (cmd == 'q') {
            printf("Bye.\n");
            break;

        } else if (cmd == 'h') {
            print_help();

        } else if (cmd == 'p') {
            display();

        } else if (cmd == 'c') {
            show_count();

        } else if (cmd == 'u') {
            undo();

        } else if (cmd == 'i') {
            /* format: i <number> <text...> */
            if (sscanf(input, "i %d", &lineno) == 1) {
                char *text = strchr(input + 2, ' ');
                if (text != NULL) {
                    text++; /* skip the space */
                    text[strcspn(text, "\n")] = '\0'; /* strip newline */
                    insert_line(lineno, text);
                } else {
                    printf("Usage: i <line#> <text>\n");
                }
            } else {
                printf("Usage: i <line#> <text>\n");
            }

        } else if (cmd == 'd') {
            if (sscanf(input, "d %d", &lineno) == 1) {
                delete_line(lineno);
            } else {
                printf("Usage: d <line#>\n");
            }

        } else if (cmd == 's') {
            if (sscanf(input, "s %s", arg1) == 1) {
                save_file(arg1);
            } else {
                printf("Usage: s <filename>\n");
            }

        } else if (cmd == 'o') {
            if (sscanf(input, "o %s", arg1) == 1) {
                load_file(arg1);
            } else {
                printf("Usage: o <filename>\n");
            }

        } else if (cmd == 'f') {
            if (sscanf(input, "f %s", arg1) == 1) {
                search(arg1);
            } else {
                printf("Usage: f <word>\n");
            }

        } else if (cmd == 'r') {
            /* format: r <line#> <old> <new> */
            if (sscanf(input, "r %d %s %s", &lineno, arg2, arg3) == 3) {
                replace_on_line(lineno, arg2, arg3);
            } else {
                printf("Usage: r <line#> <old> <new>\n");
            }

        } else if (cmd == 'R') {
            /* format: R <old> <new> */
            if (sscanf(input, "R %s %s", arg2, arg3) == 2) {
                replace_all(arg2, arg3);
            } else {
                printf("Usage: R <old> <new>\n");
            }

        } else {
            printf("Unknown command. Type 'h' for help.\n");
        }
    }

    return 0;
}