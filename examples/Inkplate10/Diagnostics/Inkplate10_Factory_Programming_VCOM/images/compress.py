import re
import os

def compress_rle(data):
    compressed = []
    i = 0
    while i < len(data):
        value = data[i]
        count = 1
        while i + count < len(data) and data[i + count] == value and count < 255:
            count += 1
        compressed.append(value)
        compressed.append(count)
        i += count
    return compressed

def extract_array_from_cpp(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    
    array_match = re.search(r'{([^}]*)}', content)
    if not array_match:
        raise ValueError("Array not found in the given C++ file")
    
    array_data = array_match.group(1).split(',')
    array_data = [int(x.strip(), 16) for x in array_data if x.strip()]
    return array_data

def create_cpp_array(data, variable_name='compressed_data'):
    array_content = ', '.join(f'0x{value:02x}' for value in data)
    cpp_array = f"const uint8_t {variable_name}[] PROGMEM = {{{array_content}}};"
    return cpp_array

def main():
    input_file = 'slide02_cropped.h'
    output_file = 'compressed_data.h'

    # Check if input file exists
    if not os.path.isfile(input_file):
        print(f"Error: The file '{input_file}' does not exist.")
        return

    # Extract the array from the input C++ file
    try:
        uncompressed_data = extract_array_from_cpp(input_file)
    except Exception as e:
        print(f"Error while extracting array from {input_file}: {e}")
        return

    # Compress the data using RLE
    compressed_data = compress_rle(uncompressed_data)

    # Create the compressed array in C++ format
    compressed_cpp_array = create_cpp_array(compressed_data)

    # Write the compressed array to the output C++ file
    try:
        with open(output_file, 'w') as file:
            file.write(compressed_cpp_array)
        print(f"Compressed data has been written to {output_file}")
    except Exception as e:
        print(f"Error while writing to {output_file}: {e}")

if __name__ == "__main__":
    main()
