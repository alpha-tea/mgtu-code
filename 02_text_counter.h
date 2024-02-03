#ifndef TEXT_COUNTER
#define TEXT_COUNTER

#include <stdio.h>
#include <string.h>
#include <ctype.h>

enum idx_name_2 {word_count, english, all, ignore};
int file_length_counter(FILE* file);
void ascii_find(FILE* file, int only_letters, int ignore);
int word_counter(FILE* file);
int command_analyzer_2(char source_parameter[], char* commands[], int commands_quantity);
int text_counter_tests(int argc, char* argv[]);

#endif
