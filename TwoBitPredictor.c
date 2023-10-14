#include <stdio.h>
#include <string.h>

#define SIZE 5

int main()
{
    char str[SIZE];

    printf("Enter the bit pattern: \n");
    scanf("%s", str);
    printf("The entered bit pattern is: %s \n\n", str);

    char prediction_bit = '1';
    char hysteresis_bit = '1';
    int predictions = 0, mispredictions = 0;
    float mispredictionRate = 0.0;

    for (int i = 0; i < SIZE; i++)
    {
        char bit = str[i];
        if (bit == '1')
        {
            if (prediction_bit == '1' && hysteresis_bit == '1')
            {
                predictions++;
            }
            else if (prediction_bit == '1' && hysteresis_bit == '0')
            {
                hysteresis_bit = '1';
                predictions++;
            }
            else if (prediction_bit == '0' && hysteresis_bit == '1')
            {
                prediction_bit = '1';
                hysteresis_bit = '0';
                mispredictions++;
            }
            else
            {
                hysteresis_bit = '1';
                mispredictions++;
            }
        }
        else
        {
            if (prediction_bit == '1' && hysteresis_bit == '1')
            {
                hysteresis_bit = '0';
                mispredictions++;
            }
            else if (prediction_bit == '1' && hysteresis_bit == '0')
            {
                prediction_bit = '0';
                hysteresis_bit = '1';
                mispredictions++;
            }
            else if (prediction_bit == '0' && hysteresis_bit == '1')
            {
                hysteresis_bit = '0';
                predictions++;
            }
            else
            {
                predictions++;
            }
        }
    }
    mispredictionRate = (float)mispredictions / (mispredictions + predictions);

    printf("Total Predictions = %d \n", predictions);
    printf("Total MisPredictions = %d \n", mispredictions);
    printf("MisPrediction Rate = %f \n", mispredictionRate);
    printf("Accuracy Rate = %f \n", 1 - mispredictionRate);
    return 0;
}
