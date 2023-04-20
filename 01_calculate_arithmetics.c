#include "01_calculate_arithmetics.h"

void calculator_lab1(char number_1[], char number_2[], char operator[], int to_hex, int to_int)
{
    double num_1 = atof(number_1);
    double num_2 = atof(number_2);
    if (to_int) {
        num_1 = floor(num_1);
        num_2 = floor(num_2);
    }
    double result = 0;
    switch(operator[0]) {
    case '+':
        result = num_1 + num_2;
        break;
    case '-':
        result = num_1 - num_2;
        break;
    case '*':
        result = num_1 * num_2;
        break;
    case '/':
        result = num_1 / num_2;
        break;
    default:
        printf("error: unknown operator;\n");
    }
    if (to_hex && to_int)
        printf("%X\n", (int)result);
    else if (to_hex && !to_int)
        printf("%.3a\n", result);
    else if (!to_hex && to_int)
        printf("%.f\n", result);
    else
        printf("%.3f\n", result);
}

int command_analyzer_1(char source_parameter[], char* commands[], int commands_quantity)
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

int calculate_arithmetics_tests(int argc, char* argv[]) {
    if (argc < 2) { // Проверка на наличие параметров.
        printf("%s", data[0]);
        return -1;
    }
    char* commands[] = {"-h", "-t", "-c", "-x", "-i"};
    int flag_hex = 0, flag_int = 0;
    const int commands_quantity = 5;
    for (int i = 1; i < argc; ++i) {
        int idx = command_analyzer_1(argv[i], commands, commands_quantity);
        switch (idx) {
        case help:
            printf("%s", data[1]);
            break;
        case table:
            printf("%s", data[2]);
            break;
        case calc:
            if (argc >= i + 3) {
                calculator_lab1(argv[i + 1], argv[i + 3], argv[i + 2], flag_hex, flag_int);
                i += 3;
            }
            break;
        case to_hex:
            flag_hex = 1;
            break;
        case to_int:
            flag_int = 1;
            break;
        default:
            printf("unknown parameter '%s'", argv[i]);
        }
    }
    return 0;
}
