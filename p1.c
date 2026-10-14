#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 100
#define MAX_LEN   200

char lines[MAX_LINES][MAX_LEN];
int count = 0;

char backup_lines[MAX_LINES][MAX_LEN];
int backup_count = 0;
int have_backup = 0;

void save_backup(void) {
    for (int i = 0; i < count; i++) {
        strcpy(backup_lines[i], lines[i]);
    }
    backup_count = count;
    have_backup = 1;
}

void undo(void) {
    if (!have_backup) {
        printf("Nothing to undo.\n");
        return;
    }
    for (int i = 0; i < backup_count; i++) {
        strcpy(lines[i], backup_lines[i]);
    }
    count = backup_count;
    have_backup = 0;
    printf("Undo successful.\n");
}

void insert_line(int pos, const char *text) {
    if (pos < 1 || pos > count + 1) {
        printf("Error: invalid line number (valid range: 1 to %d).\n", count + 1);
        return;
    }
    if (count >= MAX_LINES) {
        printf("Error: document full (maximum %d lines).\n", MAX_LINES);
        return;
    }
    save_backup();
    for (int i = count; i >= pos; i--) {
        strcpy(lines[i], lines[i - 1]);
    }
    strncpy(lines[pos - 1], text, MAX_LEN - 1);
    lines[pos - 1][MAX_LEN - 1] = '\0';
    count++;
    printf("Inserted at line %d.\n", pos);
}

void delete_line(int pos) {
    if (count == 0) {
        printf("Error: document is empty.\n");
        return;
    }
    if (pos < 1 || pos > count) {
        printf("Error: invalid line number (valid range: 1 to %d).\n", count);
        return;
    }
    save_backup();
    for (int i = pos - 1; i < count - 1; i++) {
        strcpy(lines[i], lines[i + 1]);
    }
    count--;
    printf("Deleted line %d.\n", pos);
}

void display(void) {
    if (count == 0) {
        printf("(empty document)\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        printf("%d: %s\n", i + 1, lines[i]);
    }
}

void save_file(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: could not open file '%s' for writing.\n", filename);
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s\n", lines[i]);
    }
    fclose(fp);
    printf("Saved to %s.\n", filename);
}

void load_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: could not open file '%s' for reading.\n", filename);
        return;
    }
    save_backup();
    count = 0;
    while (count < MAX_LINES && fgets(lines[count], MAX_LEN, fp) != NULL) {
        lines[count][strcspn(lines[count], "\r\n")] = '\0';
        count++;
    }
    fclose(fp);
    printf("Loaded from %s (%d lines).\n", filename, count);
}

void search(const char *word) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strstr(lines[i], word) != NULL) {
            printf("Found on line %d: %s\n", i + 1, lines[i]);
            found = 1;
        }
    }
    if (!found) {
        printf("Word '%s' not found.\n", word);
    }
}

void replace_on_line(int pos, const char *oldw, const char *neww) {
    if (pos < 1 || pos > count) {
        printf("Error: invalid line number (valid range: 1 to %d).\n", count);
        return;
    }
    char *found = strstr(lines[pos - 1], oldw);
    if (found == NULL) {
        printf("'%s' not found on line %d.\n", oldw, pos);
        return;
    }

    int before_len = (int)(found - lines[pos - 1]);
    int new_len = (int)strlen(neww);
    int after_len = (int)strlen(found + strlen(oldw));

    if (before_len + new_len + after_len >= MAX_LEN) {
        printf("Error: replacement exceeds maximum line length.\n");
        return;
    }

    save_backup();
    char result[MAX_LEN];
    strncpy(result, lines[pos - 1], before_len);
    result[before_len] = '\0';
    strcat(result, neww);
    strcat(result, found + strlen(oldw));

    strcpy(lines[pos - 1], result);
    printf("Replaced on line %d.\n", pos);
}

void replace_all(const char *oldw, const char *neww) {
    int matches = 0;
    for (int i = 0; i < count; i++) {
        if (strstr(lines[i], oldw) != NULL) {
            matches++;
        }
    }
    if (matches == 0) {
        printf("'%s' not found in document.\n", oldw);
        return;
    }

    save_backup();
    int changed = 0;
    for (int i = 0; i < count; i++) {
        char *found = strstr(lines[i], oldw);
        if (found == NULL) continue;

        int before_len = (int)(found - lines[i]);
        int new_len = (int)strlen(neww);
        int after_len = (int)strlen(found + strlen(oldw));

        if (before_len + new_len + after_len >= MAX_LEN) continue;

        char result[MAX_LEN];
        strncpy(result, lines[i], before_len);
        result[before_len] = '\0';
        strcat(result, neww);
        strcat(result, found + strlen(oldw));

        strcpy(lines[i], result);
        changed++;
    }
    printf("Replaced on %d line(s).\n", changed);
}

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
    char arg1[MAX_LEN], arg2[MAX_LEN], arg3[MAX_LEN];
    int lineno;

    printf("Simple Line Editor. Type 'h' for help.\n");

    while (1) {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;

        char *ptr = input;
        while (isspace((unsigned char)*ptr)) ptr++;
        if (*ptr == '\0') continue;

        char cmd = *ptr;

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
            int offset = 0;
            if (sscanf(ptr, "%*c %d%n", &lineno, &offset) == 1 && offset > 0) {
                char *text = ptr + offset;
                while (*text == ' ' || *text == '\t') text++;
                text[strcspn(text, "\r\n")] = '\0';
                if (*text == '\0') {
                    printf("Usage: i <line#> <text>\n");
                } else {
                    insert_line(lineno, text);
                }
            } else {
                printf("Usage: i <line#> <text>\n");
            }
        } else if (cmd == 'd') {
            if (sscanf(ptr, "%*c %d", &lineno) == 1) {
                delete_line(lineno);
            } else {
                printf("Usage: d <line#>\n");
            }
        } else if (cmd == 's') {
            if (sscanf(ptr, "%*c %s", arg1) == 1) {
                save_file(arg1);
            } else {
                printf("Usage: s <filename>\n");
            }
        } else if (cmd == 'o') {
            if (sscanf(ptr, "%*c %s", arg1) == 1) {
                load_file(arg1);
            } else {
                printf("Usage: o <filename>\n");
            }
        } else if (cmd == 'f') {
            if (sscanf(ptr, "%*c %s", arg1) == 1) {
                search(arg1);
            } else {
                printf("Usage: f <word>\n");
            }
        } else if (cmd == 'r') {
            if (sscanf(ptr, "%*c %d %s %s", &lineno, arg2, arg3) == 3) {
                replace_on_line(lineno, arg2, arg3);
            } else {
                printf("Usage: r <line#> <old> <new>\n");
            }
        } else if (cmd == 'R') {
            if (sscanf(ptr, "%*c %s %s", arg2, arg3) == 2) {
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