#include <stdio.h>

int main()
{
    printf("mgtu labs;\n");
    printf("Size of char is %u, short is %u, int is %u bytes, address %u bits;\n",
           sizeof(char), sizeof(short), sizeof(int), sizeof(char*) * 8);
    polynomial();
    return 0;
}






