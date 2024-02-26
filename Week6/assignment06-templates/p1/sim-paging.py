#!/usr/bin/env python3

#
# Put your solution into the three functions in this file
#

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

    # TODO: Implement (remove this comment when you implemented something)
    
    return page_access_list, instruction_page_set


def plot_memory_access(page_access_list, png_file=None, instruction_page_set=None):

    # TODO: Implement (remove this comment before submission if you implemented somthing)

    return


def export_page_trace(page_access_list, output_file):

    # TODO: Implement (remove this comment before submission if you implemented somthing)

    return
