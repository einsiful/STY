/* 
 * Group number (on canvas): 2
 * Student 1 name: Einar Arni Bjarnason 
 * Student 2 name: Hlynur Isak Vilmundarson
 */


#include "filesystem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

FileSystem *initFileSystem(char *diskFile) {
    int fd = open(diskFile, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open disk image");
        return NULL;
    }

    // Read the header of the file system, without the FAT
    FileSystemHeader header;
    if (read(fd, &header, sizeof(header.magic) + sizeof(header.fsBlocks) + sizeof(header.rootDirectorySize)) 
        != sizeof(header.magic) + sizeof(header.fsBlocks) + sizeof(header.rootDirectorySize)) {
        perror("Failed to read header");
        close(fd);
        return NULL;
    }

    // Calculate the size of the FAT
    size_t fatSize = header.fsBlocks * sizeof(block_t);

    // Allocate memory for the FileSystem structure
    FileSystem *fs = malloc(sizeof(FileSystem));
    if (!fs) {
        perror("Failed to allocate memory for FileSystem");
        close(fd);
        return NULL;
    }

    // Allocate memory for the FileSystemHeader structure (including FAT)
    fs->header = malloc(HEADER_SIZE + fatSize);
    if (!fs->header) {
        perror("Failed to allocate memory for FileSystemHeader");
        free(fs);
        close(fd);
        return NULL;
    }

    // Copy the previously read header into the newly allocated space
    memcpy(fs->header, &header, sizeof(header.magic) + sizeof(header.fsBlocks) + sizeof(header.rootDirectorySize));

    // Read the FAT into memory after the FileSystemHeader
    if (read(fd, fs->header->fat, fatSize) != fatSize) {
        perror("Failed to read FAT");
        free(fs->header);
        free(fs);
        close(fd);
        return NULL;
    }

    // Store the file descriptor and calculate the header size
    fs->fd = fd;
    fs->headerSize = HEADER_SIZE + fatSize;

    // Return the FileSystem structure
    return fs;
}

// Create a file handle representing a file that is part of a specific file system (fs),
// starting at block (blockIndex) in that file system, and a file length (length) in bytes
// You can make use of this function in your code....
static OpenFileHandle *_openFileAtBlock(FileSystem *fs, uint32_t blockIndex,
	uint32_t length)
{
	assert(fs != NULL);
	assert(blockIndex < fs->header->fsBlocks);

	OpenFileHandle *handle = malloc(sizeof(OpenFileHandle));
	if (handle == NULL) {
		return NULL;
	}

	handle->fileSystem		= fs;
	handle->currentBlock	  = blockIndex;
	handle->currentFileOffset = 0;
	handle->length			= length;

	return handle;
}

static int _hasMoreBytes(OpenFileHandle *handle)
{
	return (handle->currentFileOffset < handle->length);

}

#define INVALID_BLOCK_INDEX (uint32_t)-1


int _findDirectoryEntry(OpenFileHandle *dir, char *name, DirectoryEntry *dirEntry)
{
	// Assuming DirectoryEntry is properly packed and does not require specific alignment.
    DirectoryEntry tempEntry;

    // Set the directory file handle to the start of the directory.
    dir->currentFileOffset = 0;
    dir->currentBlock = dir->fileSystem->header->rootDirectoryBlock;

    while (_hasMoreBytes(dir)) {
        // Read the next directory entry.
        if (readFile(dir, (char*)&tempEntry, sizeof(DirectoryEntry)) != sizeof(DirectoryEntry)) {
            return -1; // Error reading directory entry.
        }

        // Check if this is the entry we are looking for.
        if (tempEntry.fileType != 255 && strncmp(tempEntry.filename, name, FILENAME_SIZE) == 0) {
            // Found the entry, copy it to dirEntry.
            memcpy(dirEntry, &tempEntry, sizeof(DirectoryEntry));
            return 0;
        }
    }

    // Reached the end of the directory without finding the entry.
    return -1;
}

OpenFileHandle *openFile(FileSystem *fs, char *dir, char *name)
{
	if (fs == NULL || name == NULL) {
        return NULL;
    }

    // Open the root directory file handle.
    OpenFileHandle *root = _openFileAtBlock(fs, 0, fs->header->rootDirectorySize);
    if (root == NULL) {
        return NULL;
    }

    // Try to find the file in the root directory.
    DirectoryEntry entry;
    if (_findDirectoryEntry(root, name, &entry) == 0) {
        if (entry.fileType == FTYPE_REGULAR) {
            // Close the root directory handle before opening the file.
            closeFile(root);
            // Found the file; open and return a new file handle.
            return _openFileAtBlock(fs, entry.firstBlock, entry.fileSize);
        }
    }

    // Did not find the file or the entry is not a regular file.
    closeFile(root);
    return NULL;
}

void closeFile(OpenFileHandle *handle)
{
	if (handle == NULL) {
		return;
	}

	// Since we opened the file system with read access only, we do not have
	// any pending modifications that might need to be written back to the file
	// prior closing. Hence, we can just free the handle and call it a day.
	free(handle);
}

char _readFileByte(OpenFileHandle *handle)
{
	if (!_hasMoreBytes(handle)) {
        // Handle the error as appropriate in your assignment, perhaps set errno.
        return -1;
    }

    // Calculate the offset within the current block.
    uint32_t blockOffset = handle->currentFileOffset % BLOCK_SIZE;
    
    // If we are at the beginning of a block, we might need to update currentBlock.
    if (blockOffset == 0 && handle->currentFileOffset > 0) {
        // Find the next block in the chain from the FAT.
        handle->currentBlock = handle->fileSystem->fat[handle->currentBlock];
    }

    // Read the byte from the block.
    char byte;
    int blockStartAddress = handle->currentBlock * BLOCK_SIZE;
    lseek(handle->fileSystem->fd, blockStartAddress + blockOffset, SEEK_SET);
    read(handle->fileSystem->fd, &byte, 1);

    // Update the file offset.
    handle->currentFileOffset++;

    return byte;
}

int readFile(OpenFileHandle *handle, char *buffer, int length)
{
	if ((handle == NULL) || (buffer == NULL)) {
		return -1;
	}

	int n = 0;
	while ((n < length) && _hasMoreBytes(handle)) {
		buffer[n] = _readFileByte(handle);

		++n;
	}

	return n;
}
