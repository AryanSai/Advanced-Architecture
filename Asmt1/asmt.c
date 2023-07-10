#define __STDC_FORMAT_MACROS
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

void simulate(FILE *inputFile, FILE *outputFile)
{
  // See the documentation to understand what these variables mean.
  int32_t microOpCount;
  uint64_t instructionAddress;
  int32_t sourceRegister1;
  int32_t sourceRegister2;
  int32_t destinationRegister;
  char conditionRegister;
  char TNnotBranch;
  char loadStore;
  int64_t immediate;
  uint64_t addressForMemoryOp;
  uint64_t fallthroughPC;
  uint64_t targetAddressTakenBranch;
  char macroOperation[12];
  char microOperation[23];

  int loadStoreCount = 0;
  int arithmeticCount = 0;
  int conditionalCount = 0;
  int takenCount = 0;
  int notTakenCount = 0;
  int fpAddCount = 0;
  int fpMulCount = 0;
  int fpDivCount = 0;
  int fpCount = 0;

  int unconditionalCount = 0;
  int other = 0;

  int64_t totalMicroops = 0;
  int64_t totalMacroops = 0;

  fprintf(outputFile, "Processing trace...\n\n");

  while (true)
  {
    int result = fscanf(inputFile,
                        "%" SCNi32
                        "%" SCNx64
                        "%" SCNi32
                        "%" SCNi32
                        "%" SCNi32
                        " %c"
                        " %c"
                        " %c"
                        "%" SCNi64
                        "%" SCNx64
                        "%" SCNx64
                        "%" SCNx64
                        "%11s"
                        "%22s",
                        &microOpCount,
                        &instructionAddress,
                        &sourceRegister1,
                        &sourceRegister2,
                        &destinationRegister,
                        &conditionRegister,
                        &TNnotBranch,
                        &loadStore,
                        &immediate,
                        &addressForMemoryOp,
                        &fallthroughPC,
                        &targetAddressTakenBranch,
                        macroOperation,
                        microOperation);

    // puts(microOperation); //puts for printing a string

    if (result == EOF)
    {
      break;
    }

    if (result != 14)
    {
      fprintf(stderr, "Error parsing trace at line %" PRIi64 "\n", totalMicroops);
      abort();
    }

    // For each micro-op
    totalMicroops++;

    // For each macro-op:
    if (microOpCount == 1)
    {
      totalMacroops++;
    }

    // counting the number of load and store instns
    if (loadStore == 'L' || loadStore == 'S')
    {
      loadStoreCount++;
    }
    else if (targetAddressTakenBranch != 0 && conditionRegister == 'R')
    {
      if (TNnotBranch == 'T')
      {
        takenCount++;
      }
      else if (TNnotBranch == 'N')
      {
        notTakenCount++;
      }
      conditionalCount++;
    }
    else if (targetAddressTakenBranch != 0 && conditionRegister == '-')
    {
      unconditionalCount++;
    }
    else if (strncmp(microOperation, "ADD", 3) == 0 || strncmp(microOperation, "AND", 3) == 0 || strncmp(microOperation, "MUL", 3) == 0 || strncmp(microOperation, "DIV", 3) == 0 || strcmp(microOperation, "OR") == 0 || strncmp(microOperation, "SUB", 3) == 0 || strcmp(microOperation, "XOR") == 0 || strncmp(microOperation, "SHR", 3) == 0 || strncmp(microOperation, "SHL", 3) == 0 || strncmp(microOperation, "NOT", 3) == 0)
    {
      arithmeticCount++;
    }
    else if (strcmp(microOperation, "FP_ADD") == 0)
    {
      fpAddCount++;
    }
    else if (strcmp(microOperation, "FP_MUL") == 0)
    {
      fpMulCount++;
    }
    else if (strcmp(microOperation, "FP_DIV") == 0)
    {
      fpDivCount++;
    }
    else if (strncmp(microOperation, "FP", 2) == 0)
    {
      fpCount++;
    }
    else
    {
      other++;
    }
  }

  fprintf(outputFile, "Processed %" PRIi64 " trace records.\n\n", totalMicroops);

  fprintf(outputFile, "Micro-ops: %" PRIi64 "\n", totalMicroops);
  fprintf(outputFile, "Macro-ops: %" PRIi64 "\n\n", totalMacroops);

  fprintf(outputFile, "Arithmetic Instructions: %d (%f %%)\n", arithmeticCount, (float)arithmeticCount * 100 / totalMicroops);
  fprintf(outputFile, "FP_ADD Instructions: %d (%f %%)\n", fpAddCount, (float)fpAddCount * 100 / totalMicroops);
  fprintf(outputFile, "FP_MUL Instructions: %d (%f %%)\n", fpMulCount, (float)fpMulCount * 100 / totalMicroops);
  fprintf(outputFile, "FP_DIV Instructions: %d (%f %%)\n", fpDivCount, (float)fpDivCount * 100 / totalMicroops);
  fprintf(outputFile, "Other Floating Point Instructions: %d (%f %%)\n", fpCount, (float)fpCount * 100 / totalMicroops);
  fprintf(outputFile, "Load and Store Instructions: %d (%f %%)\n", loadStoreCount, (float)loadStoreCount * 100 / totalMicroops);
  fprintf(outputFile, "Conditional Instructions: %d (%f %%)\n", conditionalCount, (float)conditionalCount * 100 / totalMicroops);
  fprintf(outputFile, "Conditional Branches Taken: %d (%f %%)\n", takenCount, (float)takenCount * 100 / totalMicroops);
  fprintf(outputFile, "Conditional Branches Not Taken: %d (%f %%)\n", notTakenCount, (float)notTakenCount * 100 / totalMicroops);
  fprintf(outputFile, "Unconditional Instructions: %d (%f %%)\n", unconditionalCount, (float)unconditionalCount * 100 / totalMicroops);
  fprintf(outputFile, "Other Instructions: %d (%f %%)\n\n", other, (float)other * 100 / totalMicroops);

  float avgCPI = (arithmeticCount + loadStoreCount * 4 + unconditionalCount * 2 + conditionalCount * 4 + fpAddCount * 3 + fpDivCount * 30 + fpMulCount * 3) / (arithmeticCount + loadStoreCount + unconditionalCount + conditionalCount + fpAddCount + fpDivCount + fpMulCount);

  fprintf(outputFile, "Average CPI: %f \n", avgCPI);
}

int main(int argc, char *argv[])
{
  FILE *inputFile = stdin;
  FILE *outputFile = stdout;

  if (argc >= 2)
  {
    inputFile = fopen(argv[1], "r");
    assert(inputFile != NULL);
  }
  if (argc >= 3)
  {
    outputFile = fopen(argv[2], "w");
    assert(outputFile != NULL);
  }

  simulate(inputFile, outputFile);
  return 0;
}
