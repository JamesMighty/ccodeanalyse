#include <stdlib.h>
#include <common.h>

#ifndef _CANALYSE_TYPES
#define _CANALYSE_TYPES

// defines c code file analysis properties
typedef struct _C_CODE_FILE_ANALYSIS {
  // total number of lines in the file
  long lines;
  // total number of characters in the file
  long characters;
  // total number of lines with code
  long code_lines;
  // total number of functions (lines with function declarations)
  long functions;
  // flag: true if contains one or more instances of imbalanced brackets i.e. (), [], {}
  int is_imbalanced:1;
} CCodeAnalysis;

// defjnes flags for inner function of method `analyse_c_code_file(...)`
typedef struct _C_CODE_FILE_ANALYSIS_LOOP_FLAGS {
  // flag: true if current line is a function declaration
  int is_function:1;
  // flag: true if current line is a blank
  int is_blank:1;
  // flag: true if last character was a newline character 
  //  - treat CRLF as one line ending as it is 2 characters long
  int is_on_line_edge:1;
  // flag: true if string block has been opened on current line and not yet closed
  int is_string_block_open:1;
  // flag: true if character block has been opened on current line and not yet closed
  int is_character_block_open:1;
  // flag: true if character-escaping block has been opened on current line and not yet closed
  int is_escaping:1;
} CCodeAnalysisLoopFlags;

// defines bracket stack type (same as basic char stack)
typedef struct _BRACKET_STACK {
  // stack of brackets
  char *brackets;
  // current index of the stack
  int index;
  // maximum size of the stack
  int max_size;
} BracketStack;

// Defines BracketStack type constructor
// @param max_size: maximum size of the stack (number of characters ~ brackets)
BracketStack *create_bracket_stack(int max_size) {
  BracketStack *stack = (BracketStack*)malloc(sizeof(BracketStack));
  stack->brackets = (char *)malloc(sizeof(char) * max_size);
  stack->index = 0;
  stack->max_size = max_size;
  return stack;
}

int remove_from_bracket_stack(BracketStack *stack, char opening_bracket, int force_remove) {
  int is_imbalanced = 0;
  if (stack->index > 0) {
    if (stack->brackets[stack->index-1] != opening_bracket){
      is_imbalanced = 1;
      #ifdef DEBUG
        printf("Error: imbalanced brackets: '%c' (last opened) != '%c' (current to remove) %c",
         stack->brackets[stack->index-1], opening_bracket, LINE_END);
      #endif
    }else {
      if (!force_remove)
        stack->index--;
      #ifdef DEBUG
        printf("Bracket stack: removed: '%c', len: %i,  dump: %.*s%c",
            opening_bracket, stack->index,
            stack->index, stack->brackets,
            LINE_END);
      #endif
    }
    if (force_remove)
      stack->index--;
  }
  return is_imbalanced;
}

#endif