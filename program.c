#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <analyse.h>
#include <common.h>

#define ERR_ARGS_COUNT "Error: Wrong number of arguments.%c"
#define ERR_ARGS_COUNT_CODE 0xE10
#define ERR_FILE_OPEN "Error: File could not be opened.%c"
#define ERR_FILE_OPEN_CODE 0xE11

void print_analysis(CCodeAnalysis analysis, const char * output_filename)
{
  printf("Lines in total: %ld%c", analysis.lines, LINE_END);
  printf("Characters: %ld%c", analysis.characters, LINE_END);
  printf("Lines of code: %ld%c", analysis.code_lines, LINE_END);
  printf("Number of function declarations: %ld%c", analysis.functions, LINE_END);
  printf("Is bracket-wise imbalanced: %s%c", analysis.is_imbalanced?"TRUE":"FALSE", LINE_END);
  printf("Output file: %s%c", output_filename, LINE_END);
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf(ERR_ARGS_COUNT, LINE_END);
    return ERR_ARGS_COUNT_CODE;
  }

  FILE * analysed_file = fopen(argv[1],"r");
  if (analysed_file == NULL) {
    printf(ERR_FILE_OPEN, LINE_END);
    return ERR_FILE_OPEN_CODE;
  }

  char * output_filename = (char *) argv[1];
  output_filename[strlen(output_filename)-1] = 'o';
  FILE * output_file = fopen(output_filename, "w");

  if (output_file == NULL) {
    printf(ERR_FILE_OPEN, LINE_END);
    return ERR_FILE_OPEN_CODE;
  }

  CCodeAnalysis analysis = analyse_c_code_file(analysed_file, output_file);
  fclose(analysed_file);
  fclose(output_file);
  print_analysis(analysis, output_filename);
  
  return 0;
}
