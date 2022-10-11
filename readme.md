# Canalyse

Analyses C source code.



## outputs:

* Count the total number of lines.
* Count the total number of characters.
* Count the number of lines of code (excluding lines with only comments or white space).
* Produce an output file that has the comments removed but that is otherwise identical to the input .c file.
* Counts the number of functions defined in the input .c file.
* Determines whether the C code contains any imbalanced brackets.
    This is defined as having an opening [, (, or { without a matching
    closing bracket, where closing brackets need to be in the same order as
    opening brackets.

> Creates file `<anasylised filename stem>.o` containing same source code but removed from comments.

---

## TODO:
- make `readme.md` more obtained about the functionality of this program
- test if makefile works properly 

---

### known bugs:
- blank lines at the end of the file are left uncounted when line end set to CLRF (needs investigation)
- possible last blank line added to `.o` output file