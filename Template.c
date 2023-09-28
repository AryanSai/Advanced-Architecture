#include <stdio.h>
#include <string.h>
void main()
{

    char str[6] = "Geek";
    char ch = 's';

    strncat(str, &ch, 1);

    printf("Appended String: %s\n", str);
}