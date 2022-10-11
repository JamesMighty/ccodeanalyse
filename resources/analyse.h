#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>
#include <common.h>

#ifndef _CANALYSE_ANALYSE
#define _CANALYSE_ANALYSE
#define LINE_BUFFER_SIZE 1024

// Defines the c source code analysis method
// @param file: the file to analyse
// @param output_file: the file to write the uncommented code to
CCodeAnalysis analyse_c_code_file(FILE *file, FILE *output_file) {
  // analysis resources
  CCodeAnalysis analysis = {0L, 0L, 0L, 0L, 0}; // test
  CCodeAnalysisLoopFlags flags = {0, 0, 1, 0, 0, 0};
  BracketStack *bracket_stack = create_bracket_stack(LINE_BUFFER_SIZE);
  long char_line_index = 0L;

  // comment lookup resources
  char comment_declaration[] = "// ";
  long comment_start_len = 3;
  long starts_with_comment_index = 0;
  long commented_on_index = 0;
  long commented_on = -1;
  // function declaration lookup resources
  char type[50], name[100], args[LINE_BUFFER_SIZE-150-4];
  // last read line buffer
  char* line = (char *)malloc(LINE_BUFFER_SIZE);

  char ch, last_ch;
  while (1) {
    // read next character
    ch = fgetc(file);
    // increment characted count
    if (ch != EOF)
      analysis.characters++;

    // event: on line edge
    if (ch == '\n' || ch == '\r' || ch == EOF) {
      if (flags.is_on_line_edge && ch == '\r') {
        flags.is_on_line_edge = 0;
        continue;
      }
      // conlude line type and increment line count
      analysis.lines+=1;
      // could be CLRF
      if (ch == '\n')
        flags.is_on_line_edge = 1;

      // print last line
      #ifdef DEBUG
        printf("%ld. > %s%c",analysis.lines, line, LINE_END);
      #endif
      // write only first part of line if ends with comment
      long line_code_carret = commented_on>=0?commented_on:char_line_index;
      fwrite(line, sizeof(char), line_code_carret, output_file);
      fputs(LINE_END_STR, output_file);

      // resolve code lines
      if (commented_on == -1 && !flags.is_blank){
        analysis.code_lines++;
        // try to parse line as function declaration
        if (sscanf(line, "%s %s (%s)%*s", type, name, args) == 3){ // 3 or 4? 3 is better
          if (!strcmp(type, "int") || !strcmp(type, "void") || !strcmp(type, "double")){
            flags.is_function = 1;
            #ifdef DEBUG
              printf(" ^ this is a function declaration%c", LINE_END);
            #endif
          }else {
            #ifdef DEBUG
              printf(" ^ function but invalid type: %s%c", type, LINE_END);
            #endif
          }
        }
      }
      if (flags.is_function){
        analysis.functions++;
      }

      // clear line buffer
      memset( line, 0, LINE_BUFFER_SIZE );
      // reset loop flags
      flags.is_blank=1;
      flags.is_function=0;
      // reset in-line indexes
      starts_with_comment_index=0;
      commented_on_index = 0;
      commented_on = -1;
      char_line_index = 0;

      // event: on file end
      if (ch == EOF) {
        if (bracket_stack->index != 0){
          analysis.is_imbalanced = 1;
          #ifdef DEBUG
            printf("%s%c", "File contains imbalanced brackets.", LINE_END);
          #endif
        }
        break;
      }
      continue;
    }

    // track character-escaping
    if (ch == '\\' && last_ch != '\\'){
      flags.is_escaping = 1;
      #ifdef DEBUG
        printf("Character escaping.%c", LINE_END);
      #endif
    }
    

    // track opening of string and character block in case of comment declaration contained inside of a string
    if (ch == '"' && !flags.is_character_block_open && !flags.is_escaping){
      flags.is_string_block_open = !flags.is_string_block_open;
      #ifdef DEBUG
        printf("String block %s.%c", flags.is_string_block_open?"open":"closed", LINE_END);
      #endif
    }
    if (ch == '\'' && !flags.is_string_block_open && !flags.is_escaping){
      flags.is_character_block_open = !flags.is_character_block_open;
      #ifdef DEBUG
        printf("Character block %s.%c", flags.is_character_block_open?"open":"closed", LINE_END);
      #endif
    }

    // check if start of line is a comment line
    if (!flags.is_string_block_open && ch == comment_declaration[commented_on_index]){
      commented_on_index++;
      if (commented_on_index == comment_start_len-1) {
        commented_on = char_line_index-comment_start_len+2;
        #ifdef DEBUG
          printf("Comment start found at Col %ld.%c", commented_on, LINE_END);
        #endif
      }
    }
    else
      commented_on_index = 0;

    // check if line is blank
    if (ch != ' ' && ch != '\t')
      flags.is_blank = 0;
    
    // push bracket onto stack
    if (!flags.is_character_block_open && !flags.is_string_block_open && commented_on == -1){
      if (ch == '(' || ch == '[' || ch == '{'){
        if (bracket_stack->max_size > bracket_stack->index){
          bracket_stack->brackets[bracket_stack->index] = ch;
          bracket_stack->index++;
          #ifdef DEBUG
            printf("Bracket stack: added: '%c', len: %i,  dump: '%.*s'%c",
               ch, bracket_stack->index,
               bracket_stack->index, bracket_stack->brackets,
               LINE_END);
          #endif
        }else {
          #ifdef DEBUG
            printf("Error: Bracket stack overflow.%c", LINE_END);
          #endif
          break;
        }
      }else
      // little messy - could be done via hash table or but thats a play of time -_- too much work, too little gain
        switch(ch) {
          case ')':
            analysis.is_imbalanced = remove_from_bracket_stack(bracket_stack, '(', 0);
            break;
          case ']':
            // force removal because [] are only used for array indexing/declaration
            analysis.is_imbalanced = remove_from_bracket_stack(bracket_stack, '[', 1);
            break;
          case '}':
            analysis.is_imbalanced = remove_from_bracket_stack(bracket_stack, '{', 0);
            break;
        }
    }

    // end character-escaping
    if (last_ch == '\\' && flags.is_escaping){
      flags.is_escaping = 0;
      #ifdef DEBUG
        printf("Character '%c' escaped.%c", ch, LINE_END);
      #endif
    }
    // copy character to line buffer
    line[char_line_index] = ch;
    char_line_index++;
    last_ch = ch;
  } 
  free(line);
  free(bracket_stack->brackets);
  free(bracket_stack);
  return analysis;
}

#endif