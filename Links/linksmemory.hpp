#ifndef LINKSMAP_HPP
#define LINKSMAP_HPP

#include <stddef.h>
#include "link.hpp"
#include "FileDescriptor.h"



class LinksMemory {
public:
	size_t BlockSize = 1024 * 1024 * 16; // 16MB
    size_t MapSize = 0;
	fd_t FileDescriptor;
    void* MappedMemory;
    void* Map(const char* filename);
    void* Map(fd_t FileDescriptor);
	void Unmap();
    void* Remap();
    void ShrinkMap();
	void ResizeFile(size_t size);
    void Close();
};



#endif
