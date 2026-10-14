# Simple Command-Line Line Editor in C

A lightweight, terminal-based line editor implemented in C for **Activity 7: Build a Simple Line Editor in C (Studio Course | 3rd Semester)**.

---

## Team Information
- **Team Members:**
  - Sharanabasappagouda
  - VijayKumar
  - Vinayak P

---

## Data Structure Choice & Justification
- **Data Structure:** Fixed-size 2D character array (`char lines[MAX_LINES][MAX_LEN]`) with an integer line counter (`count`).
- **Justification:**
  - Contiguous memory allocation avoids dynamic memory overhead and eliminates risks of memory leaks or pointer corruption (`malloc`/`free`).
  - High cache locality makes line iteration, shifting, and searching fast and predictable.
  - Straightforward implementation allows robust bounds checking for line limits.

---

## Implemented Features

### Core Features
1. **Insert a line (`i`):** Inserts a line at a specific 1-based index, shifting subsequent lines down.
2. **Delete a line (`d`):** Removes a line at a specific index, shifting subsequent lines up.
3. **Display the document (`p`):** Prints all lines with line numbers.
4. **Save / Load a file (`s` / `o`):** Writes the in-memory document to a `.txt` file and reads from disk.

### Bonus Features
1. **Search (`f`):** Finds and displays all line numbers containing a specific word or phrase.
2. **Find & Replace (`r` / `R`):** Replaces a word on a single line (`r`) or across the entire document (`R`).
3. **Undo (`u`):** One-level rollback to recover from accidental insert, delete, or replacement actions.
4. **Document Statistics (`c`):** Reports total line count, word count, and character count.

---

## How to Compile and Run

### Compilation
Compile cleanly using `gcc` with standard flags:
```bash
gcc -Wall -Wextra -std=c11 p1.c -o editor
```

### Running the Editor
Run the executable from your terminal:

**Windows (PowerShell / Command Prompt):**
```bash
.\editor.exe
```

**Linux / macOS:**
```bash
./editor
```

---

## Documentation
For complete command syntax and examples, see [HELP.md](HELP.md).
