#include "openfile.h"
#include "system.h"
#include "synch.h"
#include <string.h>

#ifdef FILESYS

int fd_index = 0;
std::unordered_map<int, OpenFile *> openFileList;
static Lock open_files_mutex("open files mutex");

int do_Create(char *filename, bool isDir)
{
	 return fileSystem->Create(filename, 0, isDir) ? 1 : 0;
}

int do_Open(char *filename)
{
	open_files_mutex.Acquire();
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
	open_files_mutex.Release();
	return fd_index;
}

int do_Read(int fd, char *into, int numBytes)
{
	open_files_mutex.Acquire();
	for (auto it : openFileList) {
		if (it.first == fd) { // fd found in the open files list
			int ret = it.second->Read(into, numBytes);
			open_files_mutex.Release();
			return ret;
		}
	}
	open_files_mutex.Release();
	return 0;
}

int do_Write(int fd, char *from, int numBytes)
{
	open_files_mutex.Acquire();
	for (auto it : openFileList) {
		if (it.first == fd) { // fd found in the open files list
			int ret = it.second->Write(from, numBytes);
			open_files_mutex.Release();
			return ret;
		}
	}
	open_files_mutex.Release();
	return 0;
}

int do_Close(int fd)
{
	open_files_mutex.Acquire();
	for (auto it : openFileList) {
		if (it.first == fd) { // fd found in the open files list
			openFileList.erase(fd);
			open_files_mutex.Release();
			return 1;
		}
	}
	open_files_mutex.Release();
	return 0;
}

int do_Rm(char *filename)
{
	open_files_mutex.Acquire();
	for (auto it : openFileList) {
		if (it.first == fd) { // fd found in the open files list
			printf("Unable to delete file %s: file is currently opened\n", (char *)filename);
			openFileList.erase(fd);
			open_files_mutex.Release();
			return -1;
		}
	}
	int ret = fileSystem->Remove(filename);
	open_files_mutex.Release();
	return ret;
}

#endif
