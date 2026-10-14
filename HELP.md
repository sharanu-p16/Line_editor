# Line Editor — Command Help & Documentation

This document describes all commands supported by the Line Editor, their syntax, and usage examples.

---

## Command Reference

### 1. `i` — Insert a Line
Inserts a new line of text at the specified line number (1-based index). Existing lines at and below this position are shifted down.

- **Syntax:** `i <line_number> <text>`
- **Example:**
  ```text
  > i 1 Hello world!
  Inserted at line 1.
  > i 2 This is the second line.
  Inserted at line 2.
  > i 2 Inserted in between.
  Inserted at line 2.
  ```

---

### 2. `d` — Delete a Line
Deletes the line at the specified line number (1-based index). All subsequent lines are shifted up.

- **Syntax:** `d <line_number>`
- **Example:**
  ```text
  > d 2
  Deleted line 2.
  ```

---

### 3. `p` — Display (Print) Document
Prints the current contents of the document with line numbers.

- **Syntax:** `p`
- **Example:**
  ```text
  > p
  1: Hello world!
  2: This is the second line.
  ```

---

### 4. `s` — Save to File
Writes the in-memory document to a specified text file.

- **Syntax:** `s <filename>`
- **Example:**
  ```text
  > s notes.txt
  Saved to notes.txt.
  ```

---

### 5. `o` — Open / Load from File
Reads a text file from disk into memory, replacing the current document.

- **Syntax:** `o <filename>`
- **Example:**
  ```text
  > o notes.txt
  Loaded from notes.txt.
  ```

---

### 6. `f` — Find / Search
Searches the document for occurrences of a given word or phrase and reports the line numbers where it appears.

- **Syntax:** `f <word>`
- **Example:**
  ```text
  > f Hello
  Found on line 1
  ```

---

### 7. `r` — Replace Word on a Specific Line
Replaces the first occurrence of `old_word` with `new_word` on a specified line number.

- **Syntax:** `r <line_number> <old_word> <new_word>`
- **Example:**
  ```text
  > r 1 Hello Greetings
  Replaced on line 1.
  ```

---

### 8. `R` — Replace Across Entire Document
Replaces occurrences of `old_word` with `new_word` across all lines in the document.

- **Syntax:** `R <old_word> <new_word>`
- **Example:**
  ```text
  > R line sentence
  Replaced on 2 line(s).
  ```

---

### 9. `c` — Statistics (Line, Word, Character Count)
Calculates and displays document statistics: total lines, words, and characters.

- **Syntax:** `c`
- **Example:**
  ```text
  > c
  Lines: 3 | Words: 14 | Characters: 82
  ```

---

### 10. `u` — Undo Last Action
Reverts the document to its state before the most recent modification (`insert`, `delete`, `replace`, or `load`). Supports 1 level of undo.

- **Syntax:** `u`
- **Example:**
  ```text
  > u
  Undo successful.
  ```

---

### 11. `h` — Help
Prints a summary list of all available commands and syntax.

- **Syntax:** `h`

---

### 12. `q` — Quit
Exits the line editor program.

- **Syntax:** `q`
- **Example:**
  ```text
  > q
  Bye.
  ```
