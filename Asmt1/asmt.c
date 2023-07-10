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
    else
    { // arithmetic and others
      other++;
    }

    if (strcmp(microOperation, "ADD") == 0 || strcmp(microOperation, "ADD_IMM") == 0 || strcmp(microOperation, "SUB") == 0 || strcmp(microOperation, "SUB_IMM") == 0 || strcmp(microOperation, "SHL") == 0 || strcmp(microOperation, "SHR") == 0 || strcmp(microOperation, "SHL_IMM") == 0 || strcmp(microOperation, "SHR_IMM") == 0)
    {
      arithmeticCount++;
    }

    if(strncmp(microOperation,"FP",2)){
      fpCount++;
    }

    // if(strcmp(microOperation,"JMP_REG")==0 || strcmp(microOperation,"JMP_IMM")==0 ){
    //   conditionalCount1++;
    // }
  }

  fprintf(outputFile, "Processed %" PRIi64 " trace records.\n\n", totalMicroops);

  fprintf(outputFile, "Micro-ops: %" PRIi64 "\n", totalMicroops);
  fprintf(outputFile, "Macro-ops: %" PRIi64 "\n\n", totalMacroops);

  fprintf(outputFile, "Arithmetic Instructions: %d \n", arithmeticCount);
  fprintf(outputFile, "Floating Point Arithmetic Instructions: %d \n", fpCount);
  fprintf(outputFile, "Load and Store Instructions: %d \n", loadStoreCount);
  fprintf(outputFile, "Conditional Branches Taken: %d \n", takenCount);
  fprintf(outputFile, "Conditional Branches Not Taken: %d \n", notTakenCount);
  fprintf(outputFile, "Unconditional Instructions: %d \n", unconditionalCount);
  fprintf(outputFile, "Other Instructions: %d \n\n", other-arithmeticCount);

  fprintf(outputFile, "Arithmetic Instructions CPI: %d \n", arithmeticCount);
  fprintf(outputFile, "Floating Point Arithmetic Instructions: %d \n", fpCount);
  fprintf(outputFile, "Load and Store Instructions CPI: %d \n", loadStoreCount * 2);
  fprintf(outputFile, "Conditional Branches Taken CPI: %d \n", takenCount * 4);
  fprintf(outputFile, "Conditional Branches Not Taken CPI: %d \n", notTakenCount * 4);
  fprintf(outputFile, "Unconditional Instructions CPI: %d \n", unconditionalCount * 2);
  fprintf(outputFile, "Other Instructions CPI: %d \n", other-arithmeticCount);
  float avgCPI = ((arithmeticCount) + (loadStoreCount * 2) + (unconditionalCount * 2) + (other-arithmeticCount) + (takenCount * 4) + notTakenCount * 4) / (loadStoreCount + takenCount + notTakenCount + unconditionalCount + other);
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
