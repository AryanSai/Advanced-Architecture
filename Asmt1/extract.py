def extract_lines_from_trace_file(file_path, num_lines, output_file_path):
    extracted_lines = []
    with open(file_path, 'r') as file:
        for i in range(num_lines):
            line = file.readline()
            if line:
                extracted_lines.append(line.rstrip('\n'))
            else:
                break
    
    with open(output_file_path, 'w') as output_file:
        for line in extracted_lines:
            output_file.write(line + '\n')

file_path = '/home/dmacs/Desktop/MTech/102P/Asmt1/gcc-10M.trace'
num_lines = 100000  
output_file_path = '/home/dmacs/Desktop/MTech/102P/Asmt1/data1'  

extract_lines_from_trace_file(file_path, num_lines, output_file_path)