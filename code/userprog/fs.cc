#include "openfile.h"
#include "system.h"
#include <string.h>

#ifdef FILESYS

int fd_index = 0;
std::unordered_map<int, OpenFile *> openFileList;

int do_Create(char *filename, bool isDir)
{
	 return fileSystem->Create(filename, 0, isDir) ? 1 : 0;
}

int do_Open(char *filename)
{
	if (openFileList.size() >= 10) {
		printf("Unable to open file %s: too many files currently open\n", (char *)filename);
		return -1;
	}

	OpenFile *f = fileSystem->Open(filename);
	if (f == nullptr) {
		printf("Unable to open file %s: file not found\n", filename);
		return -1;
	}

	openFileList.insert(std::make_pair(++fd_index, f));
	return fd_index;
}

int do_Read(int fd, char *into, int numBytes)
{
	for (auto it : openFileList) {
		if (it.first == fd) { // fd found in the open files list
			return it.second->Read(into, numBytes);
		}
	}
	return 0;
}

int do_Write(int fd, char *from, int numBytes)
{
	for (auto it : openFileList) {
		if (it.first == fd) { // fd found in the open files list
			return it.second->Write(from, numBytes);
		}
	}
	return 0;
}

int do_Close(int fd)
{
	for (auto it : openFileList) {
		if (it.first == fd) { // fd found in the open files list
			openFileList.erase(fd);
			return 1;
		}
	}
	return 0;
}

int do_Rm(char *filename)
{
		return fileSystem->Remove(filename);
}

#endif
