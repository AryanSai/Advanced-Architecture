#include <stdio.h>
#include <stdint.h>

// No. of entries in the predictor table
#define NUM_ENTRIES 256

// Bitfield for branch prediction states
typedef struct
{
    unsigned int state : 2;
} PredictorEntry;

// Initialize the predictor table
void initializePredictor(PredictorEntry predictorTable[NUM_ENTRIES])
{
    for (int i = 0; i < NUM_ENTRIES; i++)
    {
        predictorTable[i].state = 0; // Initializing to StronglyNotTaken
    }
}

// Predict the outcome of a branch
unsigned int predictBranch(PredictorEntry predictorTable[NUM_ENTRIES], uint8_t lastByteOfPC)
{
    return predictorTable[lastByteOfPC].state;
}

// Update the predictor table based on the actual branch outcome
void updatePredictor(PredictorEntry predictorTable[NUM_ENTRIES], uint8_t lastByteOfPC, int branchOutcome)
{
    unsigned int currentState = predictorTable[lastByteOfPC].state;

    // Update the state based on the branch outcome
    if (branchOutcome)
    {
        if (currentState < 3)
        {
            currentState++;
        }
    }
    else
    {
        if (currentState > 0)
        {
            currentState--;
        }
    }

    // Update the predictor table with the new state
    printf("%d",currentState);
    predictorTable[lastByteOfPC].state = currentState;
}

int main()
{
    PredictorEntry predictorTable[NUM_ENTRIES];
    uint8_t lastByteOfPC = 0xFF;
    initializePredictor(predictorTable);

    // Simulate a branch with a given last byte of PC and outcome
    int branchOutcome = 1; // 1 for taken, 0 for not taken
    unsigned int prediction = predictBranch(predictorTable, lastByteOfPC);

    // Update the predictor table with the actual outcome
    updatePredictor(predictorTable, lastByteOfPC, branchOutcome);

    // Now, you can use the 'prediction' variable for speculative execution

    return 0;
}