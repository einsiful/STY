#!/usr/bin/env python3

#
# Put your solution into the three functions in this file
#

import numpy as np
import matplotlib.pyplot as plt

def get_page_list(filename):
    # Expected functionality: Read content of file (valgrind/lackey output), and then
    # - find all lines containing memory access:
    #   Line has I, L, M, S at the beginning (first two columns), then a space
    #   After that an address in hex notation
    #   Finally, a comma and an access size in byte
    # - construct an ordered list of memory pages accessed (based on the address)
    # - construct an set of memory pages that contain instructions (based on address in 'I' lines)
    page_access_list = []
    instruction_page_set = set()
    PAGE_SIZE = 4096

    def is_memory_access(line):
        OPERATIONS = ['I', 'L', 'S', 'M']
        
        is_operation = line[0] in OPERATIONS or line[1] in OPERATIONS
        followed_by_space = line[2] == ' '
        has_access_size = ',' in line
        
        return is_operation and followed_by_space and has_access_size
    
    with open(filename, 'r') as file:
        for line in file:
            if is_memory_access(line):
                columns = line.split()
                
                access_type = columns[0]
                virtual_address = int(columns[1], base = 16) # Convert the address from hex to int using base=16
                virtual_page_number = virtual_address // PAGE_SIZE

                page_access_list.append(virtual_page_number)

                if access_type == 'I':
                    instruction_page_set.add(virtual_page_number)

    return page_access_list, instruction_page_set

def plot_memory_access(page_access_list, png_file=None, instruction_page_set=None):
    # Create 2D array
    max_page_number = max(page_access_list)
    num_bins = 1000
    bin_size = len(page_access_list) // num_bins
    num_pages = max_page_number + 1
    access_array = np.zeros((num_bins, num_pages))

    # Normalize page numbers on contiguous numbers
    page_map = {page: i for i, page in enumerate(sorted(set(page_access_list)))}

    for i in range(num_bins):
        bin_start = i * bin_size
        bin_end = min((i + 1) * bin_size, len(page_access_list))
        bin_pages = [page_map[page] for page in page_access_list[bin_start:bin_end]]
        for page in bin_pages:
            access_array[i][page] = 1

    # Plot access array
    plt.imshow(access_array.T, cmap='binary', aspect='auto')
    plt.xlabel('Bins')
    plt.ylabel('Page Numbers')
    plt.title('Memory Access Patterns')
    plt.gca().invert_yaxis()
    plt.colorbar(label='Access')
    if png_file:
        plt.savefig(png_file)
    else:
        plt.show()

def export_page_trace(page_access_list, output_file):
    unique_page_access_list = [page_access_list[0]]
    
    for page in page_access_list[1:]:
        if page != unique_page_access_list[-1]:  # only unique pages
            unique_page_access_list.append(page)

    with open(output_file, 'w') as file:
        for page in unique_page_access_list:
            file.write(str(page) + '\n')
