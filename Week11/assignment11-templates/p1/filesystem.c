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
    ssize_t bytesRead = read(fd, fs->header->fat, fatSize);
	if (bytesRead == -1 || (size_t)bytesRead != fatSize) {
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
static OpenFileHandle *_openFileAtBlock(FileSystem *fs, uint32_t blockIndex, uint32_t length)
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
    if (dir == NULL || name == NULL || dirEntry == NULL) {
        return -1;
    }

    // The size of each directory entry on disk.
    size_t entrySize = sizeof(DirectoryEntry);
    char buffer[entrySize];
    ssize_t bytesRead;  // Declare bytesRead here as ssize_t

    // Reset the directory read position.
    dir->currentFileOffset = 0;
    dir->currentBlock = ROOT_DIRECTORY_BLOCK;

    while (1) {
        bytesRead = readFile(dir, buffer, entrySize);
        if (bytesRead < 0) {
            // Handle read error
            return -1;
        }
        if ((size_t)bytesRead != entrySize) {
            // End of directory or not enough data, break from the loop
            break;
        }

        memcpy(dirEntry, buffer, entrySize);
        if (dirEntry->type != FTYPE_DELETED && strncmp(dirEntry->name, name, FILE_NAME_LENGTH) == 0) {
            return 0; // Entry found
        }
    }
    return -1; // Entry not found or end of directory reached without finding the entry
}

OpenFileHandle *openFile(FileSystem *fs, char *dir, char *name)
{
	(void)dir;

	if (fs == NULL || name == NULL) {
        return NULL;
    }

    // Find the directory entry for the file.
    DirectoryEntry entry;
    OpenFileHandle *rootDirHandle = _openFileAtBlock(fs, ROOT_DIRECTORY_BLOCK, fs->header->rootDirectorySize);
    if (rootDirHandle == NULL) {
        return NULL;
    }

    int found = _findDirectoryEntry(rootDirHandle, name, &entry);
    closeFile(rootDirHandle);  // Close root directory handle as we don't need it anymore.

    if (found != 0 || entry.type != FTYPE_REGULAR) {
        return NULL;
    }

    // Create a new file handle for the file.
    return _openFileAtBlock(fs, entry.firstBlock, entry.length);
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

char _readFileByte(OpenFileHandle *handle) {
    assert(handle != NULL);
    assert(_hasMoreBytes(handle));
    assert(handle->fileSystem != NULL);
    assert(handle->currentBlock < BLOCK_SIZE);

    return 0;
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
