import random

# Define the input and output file names
input_file = "12K.trace"
output_file = "shuffled_trace_file.trace"

# Read the lines from the input file
with open(input_file, 'r') as f:
    lines = f.readlines()

# Shuffle the lines
random.shuffle(lines)

# Write the shuffled lines to the output file
with open(output_file, 'w') as f:
    f.writelines(lines)

print("Lines shuffled and written to {}".format(output_file))
