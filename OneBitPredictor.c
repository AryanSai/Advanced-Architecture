#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define size 30

void main()
{
    char str[size];

    printf("Enter the bit pattern: \n");
    scanf("%s", str);
    printf("The entered bit pattern is: %s \n\n", str);
    
    // bool state = true;
    char state = '1';
    int predictions = 0, mispredictions = 0, length;
    float rate = 0.0;
    length = strlen(str);

    for (int i = 0; i < length; i++)
    {
        if (str[i] == state)
        {
            predictions++;
        }
        else if (str[i] == '0' && state == '1')
        {
            mispredictions++;
            state = '0';
        }
        else if (str[i] == '1' && state == '0')
        {
            mispredictions++;
            state = '1';
        }
    }
    rate = (float)mispredictions / length;
    printf("Total Predictions = %d \n", predictions);
    printf("Total MisPredictions = %d \n", mispredictions);
    printf("MisPrediction Rate = %f \n", rate);
}