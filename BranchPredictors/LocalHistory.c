#include <stdio.h>

#define HISTORY_SIZE 16

enum PredictorState
{
    StronglyTaken,
    WeaklyTaken,
    WeaklyNotTaken,
    StronglyNotTaken
};

// Define a structure to represent a single branch instruction.
typedef struct
{
    enum PredictorState state;
} BranchInstruction;

// Define a structure to hold the history of branch instructions.
typedef struct
{
    BranchInstruction history[HISTORY_SIZE];
    int index; // Index to keep track of the current position in the history
} TwoBitPredictor;

// Initialize the two-bit predictor.
void initTwoBitPredictor(TwoBitPredictor *predictor)
{
    for (int i = 0; i < HISTORY_SIZE; i++)
    {
        predictor->history[i].state = StronglyTaken; // Initialize all entries
    }
    predictor->index = 0; // Start at the beginning of the history
}

// Predict whether a branch should be taken or not taken based on the two-bit predictor.
int predictBranch(TwoBitPredictor *predictor)
{
    int index = predictor->index;
    BranchInstruction *current = &predictor->history[index];

    // Make a prediction based on the current state.
    int prediction = (current->state == StronglyTaken || current->state == WeaklyTaken) ? 1 : 0;

    // Update the index for the next prediction.
    predictor->index = (index + 1) % HISTORY_SIZE;

    return prediction;
}

// Update the two-bit predictor after a branch instruction.
void updateTwoBitPredictor(TwoBitPredictor *predictor, int taken)
{
    int index = predictor->index;
    BranchInstruction *current = &predictor->history[index];

    // Update the state of the predictor based on the actual outcome of the branch.
    if (taken)
    {
        if (current->state == StronglyTaken)
        {
            // Stay in the StronglyTaken state
        }
        else if (current->state == WeaklyTaken)
        {
            current->state = StronglyTaken;
        }
        else if (current->state == WeaklyNotTaken)
        {
            current->state = WeaklyTaken;
        }
        else
        {
            current->state = WeaklyNotTaken;
        }
    }
    else
    {
        if (current->state == StronglyNotTaken)
        {
            // Stay in the StronglyNotTaken state
        }
        else if (current->state == WeaklyNotTaken)
        {
            current->state = StronglyNotTaken;
        }
        else if (current->state == WeaklyTaken)
        {
            current->state = WeaklyNotTaken;
        }
        else
        {
            current->state = WeaklyTaken;
        }
    }
}

int main()
{
    TwoBitPredictor predictor;
    initTwoBitPredictor(&predictor);

    // Simulate a series of branch instructions
    int branchInstructions[] = {1, 0, 1, 1, 0, 0, 1, 1}; // 1 for taken, 0 for not taken

    for (int i = 0; i < sizeof(branchInstructions) / sizeof(branchInstructions[0]); i++)
    {
        int prediction = predictBranch(&predictor);
        int actualOutcome = branchInstructions[i];

        if (prediction == actualOutcome)
        {
            printf("Correct prediction: Predicted %d, Actual %d\n", prediction, actualOutcome);
        }
        else
        {
            printf("Incorrect prediction: Predicted %d, Actual %d\n", prediction, actualOutcome);
        }

        updateTwoBitPredictor(&predictor, actualOutcome);
    }
    return 0;
}
