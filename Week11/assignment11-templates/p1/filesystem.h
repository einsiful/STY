#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <inttypes.h>

#define FILE_NAME_LENGTH 8
#define BLOCK_SIZE 32
#define ROOT_DIRECTORY_BLOCK 0
#define HEADER_SIZE (sizeof(FileSystemHeader))

typedef uint16_t block_t;

// The file system image provided with this assignment is structured as follows:

// The file system starts with a header containing
// - the magic number ("StyF" -- the styrikerfi file system -- 0x46797453)
// - the size of the file system in blocks (not including the header)
// - the size of the root directory (in bytes(!))
// - the fat (array of uint16_t block numbers with fsBlocks entries)
typedef struct {
	uint32_t magic;				 // Stýrikerfy FAT file system, "StyF"
	uint32_t fsBlocks;			 // File system size in blocks (after header/FAT)
	uint32_t rootDirectorySize;				// Root directory size in bytes.
	block_t fat[];				 // The file allocation table (FAT), with fs_blocks entries
} FileSystemHeader;
#define STYFS_MAGIC 0x46797453

// Data structure to represent a file system in memory
// The header includes the cached full header from the file (fsBlocks, root dir size, FAT)
// fd contains the file descriptor (used for reading data blocks)
// headerSize the size of the header in bytes (including the complete FAT), calculated on initialization
typedef struct {
	FileSystemHeader *header;
	int headerSize;
	int fd;
} FileSystem;

// Type of entries in a directory
#define FTYPE_REGULAR 1
#define FTYPE_DIRECTORY 2
#define FTYPE_DELETED 255


// For each file, 16 bytes (i.e., half a block) of meta data are stored on disk in
// the root directory file using little endian byte order.
typedef struct {
	uint16_t type;			// FTYPE_REGULAR or FTYPE_DIRECTORY
	block_t firstBlock;		// Index of first data block.
	uint32_t length;		// File size in bytes
	char name[FILE_NAME_LENGTH];	// File name
} DirectoryEntry;

typedef struct {
	uint32_t currentFileOffset; // Index of the next byte to read within file. Starts at 0.
	block_t currentBlock;		// Index of the current block
	uint32_t length;			// Copy of the file's length
	FileSystem *fileSystem;	 // File system that owns the file
} OpenFileHandle;

FileSystem *initFileSystem(char *diskFile);

OpenFileHandle *openFile(FileSystem *fs, char *dir, char *name);
void closeFile(OpenFileHandle *handle);

int readFile(OpenFileHandle *handle, char *buffer, int length);

#endif
