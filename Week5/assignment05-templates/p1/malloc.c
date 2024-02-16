#include "malloc.h"

#include <stdio.h>
#include <assert.h>

/*
 * This is the heap you should use.
 * The heap does not grow.
 */
uint8_t __attribute__ ((aligned(HEADER_SIZE))) _heapData[HEAP_SIZE];

/*
 * This should point to the first free block in memory.
 */
Block *_firstFreeBlock;

/*
 * Initializes the memory block. You don't need to change this.
 */
void initAllocator()
{
	_firstFreeBlock = (Block*)&_heapData[0];
	_firstFreeBlock->next = NULL;
	_firstFreeBlock->size = HEAP_SIZE;
}

/*
 * Gets the next block that should start after the current one.
 */
static Block *_getNextBlockBySize(const Block *current)
{
	static const Block *end = (Block*)&_heapData[HEAP_SIZE];
	Block *next = (Block*)&current->data[current->size - HEADER_SIZE];

	assert(next <= end);
	return (next == end) ? NULL : next;
}

/*
 * Dumps the allocator. You should not need to modify this.
 */
void dumpAllocator()
{
	Block *current;
	/* Note: This sample code prints addresses relative to the beginning of the heap */

	/* Part a: all blocks, using size, and starting at the beginning of the heap */
	printf("All blocks:\n");
	current = (Block*)&_heapData[0];
	while (current) {
		printf("  Block starting at %" PRIuPTR ", size %" PRIu64 " (%s)\n",
			((uintptr_t)(void*)current - (uintptr_t)(void*)&_heapData[0]),
			current->size,
			current->magic == ALLOCATED_BLOCK_MAGIC ? "in use" : "free" );

		current = _getNextBlockBySize(current);
	}

	/* Part b: free blocks, using next pointer, and starting at _firstFreeBlock */
	printf("Free block list:\n");
	current = _firstFreeBlock;
	while (current) {
		printf("  Free block starting at %" PRIuPTR ", size %" PRIu64 "\n",
			((uintptr_t)(void*)current - (uintptr_t)(void*)&_heapData[0]),
			current->size);

		current = current->next;
	}
}

/*
 * Round the integer up to the block header size (16 Bytes).

Each block starts with a header (16 bytes), which describes the block. The header con- tains a size field as well as either a magic number (for allocated blocks) or a next pointer to the next free block (for free blocks). The data region – the region which is returned to the user – follows the header. Its length depends on the requested size, but is always rounded up to a multiple of 16 bytes.
A heap allocator must be able to quickly satisfy new memory requests. It is therefore important to quickly identify free blocks. Your heap will keep all free blocks in a linked list, connected by the next pointers and choose the first block that is large enough (first fit). When a block is allocated, it is removed from the free-list, and when it is released, it is inserted into the free-list. The free-list is ordered by block address All blocks (used and free blocks) can be iterated by going from one block to the next using the size field. The heap allocator splits/merges blocks as appropriate (see Figure 2).
a. To ease the design, the heap allocator manages memory only at the granularity of 16bytes. Write a function that rounds a given integer up to the next larger multiple of 16 (e.g., 6 → 16, 16 → 16, 17 → 32, etc.).
          uint64_t roundUp(uint64_t n);
 */
uint64_t roundUp(uint64_t n)
{
	if (n % 16 == 0) {
		return n;
	}
	return n + 16 - (n % 16);
}

/*
 * Internal helper function for making a new allocation
 * (not needed for any tests, but a suggested structure to simplify your life)
 * This function should basically do the last two items of the assignment description
 * of part b (my_malloc).
 * - block is the (currently free) block that you want to use for your new allocation
 * - new_size is the total size for the new allocation (size requested in the
 *   my_malloc call plus header size)
 * - update_next is a pointer to the next pointer pointing to block. This is
 *   the pointer you need to update for removing block from the free-list/
 *   replace block with a new free block starting somewhere within block
 */
static void * __attribute__ ((unused)) allocate_block(Block **update_next, Block *block, uint64_t new_size) {
	// Check if after allocating new_size, there's enough space for another block's header
    if (block->size - new_size >= HEADER_SIZE) {
        // We can split the block
        Block *new_block = (Block*)((uint8_t*)block + new_size);
        new_block->size = block->size - new_size;
        new_block->next = block->next; // Correctly update the next pointer
        *update_next = new_block; // Update the free list pointer to point to the new block
        block->size = new_size; // Update the size of the current block to reflect the allocation
    } else {
        // Not enough space to split the block; adjust the free list pointer directly
        *update_next = block->next;
    }
    // Return a pointer to the usable memory area, just after the header
    return (void*)((uint8_t*)block + HEADER_SIZE);
}


void *my_malloc(uint64_t size)
{
	uint64_t totalSize = roundUp(size + HEADER_SIZE); // Include the header in the total size

    Block *current = _firstFreeBlock;
    Block **prevNextPointer = &_firstFreeBlock;
    while (current != NULL) {
        if (current->size >= totalSize) {
            // Delegate to allocate_block to handle allocation and potential splitting
            return allocate_block(prevNextPointer, current, totalSize);
        }
        prevNextPointer = &current->next;
        current = current->next;
    }

    return NULL;
}

/*
 * Internal helper function to merge two blocks.
 * (not needed for any tests, but a suggested structure to simplify your life)
 * Suggested functionality (to be used in my_free):
 * if block2 follows directly after block1, then merge these two (free) blocks
 * i.e. update block1 such that it becomes one larger free block
 */
static void __attribute__ ((unused)) merge_blocks(Block *block1, Block *block2)
{
	block1->size += block2->size;
	block1->next = block2->next;
}


void my_free(void *address)
{
	Block *block = (Block*)((uint8_t*)address - HEADER_SIZE);
	Block *current = _firstFreeBlock;
	Block **update_next = &_firstFreeBlock;
	while (current) {
		if (current > block) {
			*update_next = block;
			block->next = current;
			if (block->size + (uint8_t*)block == (uint8_t*)current) {
				// We can merge the two blocks
				merge_blocks(block, current);
			}
			return;
		}
		update_next = &current->next;
		current = current->next;
	}
	*update_next = block;
	block->next = NULL;
	if (block->size + (uint8_t*)block == (uint8_t*)&_heapData[HEAP_SIZE]) {
		// We can merge the two blocks
		merge_blocks(block, (Block*)&_heapData[HEAP_SIZE]);
	}
	return;

	
}


