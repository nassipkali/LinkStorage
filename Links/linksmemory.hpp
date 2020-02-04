#ifndef LINKSMAP_HPP
#define LINKSMAP_HPP

#include <stddef.h>
#include "link.hpp"
#include "FileDescriptor.h"



class LinksMemory {
public:
	size_t BlockSize = 1024 * 1024 * 16; // 16MB
    	size_t MapSize = 0;
    	size_t MemoryUse = 0;
    	size_t LinkCount = 0;
	fd_t FileDescriptor;
	Link* MappedLinks;
	void Map(const char* filename);
	void Unmap();
	void Remap();
	void ResizeFile(size_t size);
	void Close();
	Link* LinkAlloc(size_t size);
};



#endif
