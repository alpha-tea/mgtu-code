#ifndef CALCULATE_ARITHMETICS
#define CALCULATE_ARITHMETICS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static char* data[] = { // Набор строк для вывода.

                        "This is laboratory work No. 1.\nTo use the help, run the program with the ""-h"" parameter.\n",

                        "Parameters:\n"
                        "\n<without>\t\tbriefly writes the purpose of the program and suggests how to gethelp"
                        "\n\n-h (--help)\t\t gives detailed help on the program, listing allpossible commands"
                        "\n\n-t (--table)\t\t gives in tabular form the sizes of all simple types of the language."
                        "\n\n-c (--calc)\t\t calculates a simple arithmetic\n"
                        "\t\t\t expression of the form <number operation number> (integers and real numbers)\n"
                        "\t\t\t Sub parameters:\n"
                        "\t\t\t   -x (--hex) response in 16-bit format\n"
                        "\t\t\t   -i (--int) the input numbers are integers (real by default)"
                        "\n\n<doen't match commands>\t issue a warning and an invitation to use the help\n",

                        "Type\t\t\t  Size\n\nchar\nsigned char\t\t 8 bits\nunsigned char\n\n"
                        "short\nshort int\nsigned short\nsigned short int\nunsigned short\nunsigned short int\t 16 bits\n"
                        "int\nsigned\nsigned int\nunsigned\nunsigned int\n\nlong\nlong int\nsigned long\nsigned long int"
                        "\t\t 32 bits\nunsigned long\nunsigned long int\nfloat\n\ndouble\nlong double\nlong long\nlong long int\n"
                        "signed long long\t 64 bits\nsigned long long int\nunsigned long long\nunsigned long long int\n\n",

                        "Error: unknown command.\nTo use the help, run the program with the ""-h"" parameter.\n"
                      };

enum idx_name_1 {help, table, calc, to_hex, to_int};
void calculator_lab1(char number_1[], char number_2[], char operator[], int to_hex, int to_int);
int command_analyzer_1(char source_parameter[], char* commands[], int commands_quantity);
int calculate_arithmetics_tests(int argc, char* argv[]);

#endif
