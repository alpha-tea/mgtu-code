#include "02_text_counter.h"

int file_length_counter(FILE* file) // Счётчик кол-ва символов в файле.
{
    int length = 0;
    while (fgetc(file) != EOF)
        ++length;
    return length;
}


void ascii_find(FILE* file, int only_letters, int ignore)
{
    const int length = file_length_counter(file);
    const int length_array = 256; //Кол-во символов в ascii
    double frequency = 0;
    int array_counter_ascii[length_array], symbol = 0;
    for (int i = 0; i < length_array; ++i) // обнуление всего массива
        array_counter_ascii[i] = 0;
    fseek(file, 0, SEEK_SET);
    while ((symbol = fgetc(file)) != EOF) {
        if (ignore == 1)
            symbol = tolower(symbol);
        ++array_counter_ascii[symbol];
    }
    printf("idx:\tsymbol:\t\tquantity:\n");
    for (int i = 0; i < length_array; ++i) {
        if (((i >= 'A' && i <= 'Z' && !ignore) || (i >= 'a' && i <= 'z')) || !only_letters) {
            printf("%d)\t'%c':\t%d\t", i, i, array_counter_ascii[i]);
            if (!array_counter_ascii[i])
                printf("NULL\n");
            else
                printf("1:%.lf\n", (double)(length) / (double)(array_counter_ascii[i]));
        }
    }
    printf("file length = %d\n", length);
}

int word_counter(FILE* file)
{
    int symbol = 0, is_word = 0, counter = 0;
    while ((symbol = fgetc(file)) != EOF) {
        if ((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z')) {
            if (is_word == 0) {
                is_word = 1;
                ++counter;
            }
        } else if (symbol == ' ' || symbol == '\t' || symbol == '\n')
            is_word = 0;
    }
    return counter;
}

int command_analyzer_2(char source_parameter[], char* commands[], int commands_quantity)
{
    if (commands_quantity < 1) {
        printf("error: invalid number of commands;\n");
        return -1;
    }
    int idx = 0, i = 0;
    for (i = 0; i < commands_quantity; ++i) {
        if (strcmp(source_parameter, commands[i]) == 0) {
            idx = i;
            break;
        }
    }
    if (i == commands_quantity)
        return -1;
    return idx;
}

int text_counter_tests(int argc, char* argv[])
{
    FILE*  file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("error: file '%s' not found;\n", argv[1]);
        return -1;
    }
    if (argc == 2) { // Только название файла
        ascii_find(file, 0, 0);
        return 0;
    } else if (argc < 2) {  // Нет параметров
        printf("There are no command-line switches;\n");
        return -1;
    }
    char* commands[] = {"-w", "-en", "-all", "-ir"};
    int flag_ignore = 0;
    const int commands_quantity = 5;
    for (int i = 1; i < argc; ++i) {
        int idx = command_analyzer_2(argv[i], commands, commands_quantity);
        switch (idx) {
        case word_count:
            printf("Words in file = %d;\n", word_counter(file));
            break;
        case english:
            ascii_find(file, 1, flag_ignore);
            break;
        case all:
            ascii_find(file, 0, 0);
            break;
        case ignore:
            flag_ignore = 1;
            break;
        default:
            ;
        }
    }
    return 0;
}
