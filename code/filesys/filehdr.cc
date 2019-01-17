// filehdr.cc
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector,
//
//      Unlike in a real system, we do not keep track of file permissions,
//	ownership, last modification date, etc., in the file header.
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "filehdr.h"
#include "system.h"

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool FileHeader::Allocate(BitMap *freeMap, unsigned int fileSize, bool is_directory)
{
	isDirectory = is_directory;
	numBytes = fileSize;
	numSectors = divRoundUp(fileSize, SectorSize);

	// FIXME Not thread-safe (cannot assume the number of clear sectors won't change)
	if (freeMap->NumClear() < numSectors)
		return FALSE; // not enough space

	// Number of entries in the indirect table
	unsigned int numIndir = divRoundUp(numSectors, NumDirect);

	for (unsigned int i = 0; i < numIndir; i++) {
		// New entry in indirect table, allocate a segment for it
		indirectDataSectors[i] = freeMap->Find();
		ASSERT(indirectDataSectors[i] != -1)

		int directSectors[NumDirect] = {0};
		ASSERT(sizeof(directSectors) == SectorSize);

		// A table has max NumDirect segments, and doesn't have more segments than what is needed
		for (unsigned int j = 0; (j < NumDirect && i * NumDirect + j < numSectors); j++) {
			// New entry in direct table, allocate segment for it
			directSectors[j] = freeMap->Find();
			ASSERT(directSectors[j] != -1)
		}

		synchDisk->WriteSector(indirectDataSectors[i], (char *)directSectors);
	}

	return TRUE;
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void FileHeader::Deallocate(BitMap *freeMap)
{
	unsigned int numIndir = divRoundUp(numSectors, NumDirect);

	for (unsigned int i = 0; i < numIndir; i++) {

		int directSectors[NumDirect] = {0};
		ASSERT(sizeof(directSectors) == SectorSize);

		synchDisk->ReadSector(indirectDataSectors[i], (char *)directSectors);

		// A table has max NumDirect segments, and doesn't have more segments than what is needed
		for (unsigned int j = 0; (j < NumDirect && i * NumDirect + j < numSectors); j++) {
			ASSERT(freeMap->Test(directSectors[j]));
			// Remove data sector
			freeMap->Clear(directSectors[j]);
		}

		// Remove current segment table
		ASSERT(freeMap->Test(indirectDataSectors[i]));
		freeMap->Clear(indirectDataSectors[i]);
	}
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk.
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void FileHeader::FetchFrom(int sector)
{
	synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk.
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void FileHeader::WriteBack(int sector)
{
	synchDisk->WriteSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int FileHeader::ByteToSector(int offset)
{
	// Position in the file (unit: a sector)
	int sectorDirectNumber = offset / SectorSize;
	int sectorIndirectIndex = sectorDirectNumber / NumDirect;

	int sectors[NumDirect] = {0};
	ASSERT(sizeof(sectors) == SectorSize);

	// Fetch the direct segments table
	synchDisk->ReadSector(indirectDataSectors[sectorIndirectIndex], (char *)sectors);

	int sectorDirectIndex = sectorDirectNumber % NumDirect;

	return sectors[sectorDirectIndex];
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

unsigned int FileHeader::FileLength()
{
	return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void FileHeader::Print()
{
	unsigned int i, j, k;
	char *data = new char[SectorSize];

	printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
	for (i = 0; i < numSectors; i++)
		printf("%d ", indirectDataSectors[i]);
	printf("\nFile contents:\n");
	for (i = k = 0; i < numSectors; i++) {
		synchDisk->ReadSector(indirectDataSectors[i], data);
		for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
			if ('\040' <= data[j] && data[j] <= '\176') // isprint(data[j])
				printf("%c", data[j]);
			else
				printf("\\%x", (unsigned char)data[j]);
		}
		printf("\n");
	}
	delete[] data;
}

int FileHeader::IsDirectory()
{
	return isDirectory;
}

bool FileHeader::Extend(BitMap *freeMap, unsigned int newFileSize) {
	ASSERT(isDirectory == FALSE); // Folders are supposed to have a fixed size

	ASSERT(newFileSize > numBytes); // Can only increase file size
	ASSERT(NumIndirect >= divRoundUp(newFileSize, NumDirect)); // Enough room in the indirect table

	unsigned int newNumSectors = divRoundUp(newFileSize, SectorSize);
	unsigned int numExtraSectors = newNumSectors - numSectors;

	unsigned int numExtraIndirectSectors = divRoundUp(newNumSectors, NumDirect) - divRoundUp(numSectors, NumDirect);

	// Nothing to do here
	if (numExtraSectors == 0) {
		numBytes = newFileSize;
		return TRUE;
	}

	// FIXME Not thread-safe (cannot assume the number of clear sectors won't change)
	if (freeMap->NumClear() < numExtraSectors + numExtraIndirectSectors)
		return FALSE; // Not enough space for new sectors + indirect tables

	// Number of entries in the indirect table
	unsigned int newNumIndir = divRoundUp(newNumSectors, NumDirect);

	// There is an indirect sector that is not entirely full
	if (numSectors % NumDirect != 0) {
		unsigned incompleteIndirect = numSectors / NumDirect;

		int directSectors[NumDirect] = {0};
		ASSERT(sizeof(directSectors) == SectorSize);

		synchDisk->ReadSector(indirectDataSectors[incompleteIndirect], (char *)directSectors);

		for (unsigned int j = numSectors % NumDirect; (j < NumDirect && incompleteIndirect * NumDirect + j < numSectors); j++) {

			directSectors[j] = freeMap->Find();
			ASSERT(directSectors[j] != -1)
		}

		synchDisk->WriteSector(indirectDataSectors[incompleteIndirect], (char *)directSectors);
	}

	// All new segments already fit in the remaining place in the last indirect sector
	if (numExtraIndirectSectors == 0) {
		numBytes = newFileSize;
		numSectors = newNumSectors;
		return TRUE;
	}

	// Now the last allocated indirect sector is full, need to create new indirect entries

	unsigned int i = 0;

	// File was completely empty
	if (numSectors == 0) {
		i = 0;
	} else {
		i = 1 + numSectors / NumDirect;
	}

	for (; i < newNumIndir; i++) {
		// New entry in indirect table, allocate a segment for it
		indirectDataSectors[i] = freeMap->Find();
		ASSERT(indirectDataSectors[i] != -1)

		int directSectors[NumDirect] = {0};
		ASSERT(sizeof(directSectors) == SectorSize);

		// A table has max NumDirect segments, and doesn't have more segments than what is needed
		for (unsigned int j = 0; (j < NumDirect && i * NumDirect + j < newNumSectors); j++) {
			// New entry in direct table, allocate segment for it
			directSectors[j] = freeMap->Find();
			ASSERT(directSectors[j] != -1)
		}

		synchDisk->WriteSector(indirectDataSectors[i], (char *)directSectors);
	}

	numBytes = newFileSize;
	numSectors = newNumSectors;

	return TRUE;
}
