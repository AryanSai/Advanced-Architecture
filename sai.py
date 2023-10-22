import random

# Define cache parameters
CACHE_LINE_SIZE = 16
CACHE_SIZE_L1 = 16
CACHE_SIZE_L2 = 64
ASSOCIATIVITY = 4

# Define memory address range
MEM_SIZE = 1024  # Example: 1 KB
MEMORY_ACCESS_COUNT = 100

# Create a list of memory addresses with varying access patterns
memory_accesses = []
for _ in range(MEMORY_ACCESS_COUNT):
    # Generate a random memory address within the memory range
    address = random.randint(0, MEM_SIZE - 1)

    # Randomly choose between read and write operations
    operation = random.choice(["R", "W"])

    # Add the operation to the list
    if operation == "R":
        memory_accesses.append((operation, address))
    else:
        # For write operations, generate random data (ASCII characters)
        data = random.choice("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
        memory_accesses.append((operation, address, data))

    # Add prefetch addresses (assuming a stride of 16 bytes for simplicity)
    for i in range(1, 4):
        prefetch_address = (address + i * CACHE_LINE_SIZE) % MEM_SIZE
        memory_accesses.append(("R", prefetch_address))

# Create a sample trace file
with open("sample_memory_trace.trace", "w") as trace_file:
    for entry in memory_accesses:
        if entry[0] == "R":
            trace_file.write("{} 0x{:04X}\n".format(entry[0], entry[1]))
        elif entry[0] == "W":
            trace_file.write("{} 0x{:04X} {}\n".format(entry[0], entry[1], entry[2]))

print("Sample trace file 'sample_memory_trace.trace' generated.")
