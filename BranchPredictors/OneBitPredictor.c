#include <stdio.h>
#include <string.h>

#define SIZE 30

int main()
{
    char str[SIZE];

    printf("Enter the bit pattern: \n");
    scanf("%s", str);
    printf("The entered bit pattern is: %s \n\n", str);

    char state = '1'; // Initialize to predict '1'
    int predictions = 0, mispredictions = 0;
    float mispredictionRate = 0.0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        char bit = str[i];

        if (bit == '1' && state == '0')
        {
            mispredictions++;
            state = '1';
        }
        else if (bit == '0' && state == '1')
        {
            mispredictions++;
            state = '0';
        }
        else
        {
            predictions++;
        }
    }

    mispredictionRate = (float)mispredictions / (mispredictions + predictions);

    printf("Total Predictions = %d \n", predictions);
    printf("Total MisPredictions = %d \n", mispredictions);
    printf("MisPrediction Rate = %f \n", mispredictionRate);
    printf("Accuracy Rate = %f \n", 1 - mispredictionRate);

    return 0;
}
