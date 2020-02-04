#ifndef LINKSMAP_HPP
#define LINKSMAP_HPP

#include <stddef.h>
#include "link.hpp"
#include "FileDescriptor.h"



class LinksMap {
public:
	size_t BlockSize = 1024 * 1024 * 64; // 64MB
    	size_t MapSize = 0;
	fd_t FileDescriptor;
	void* MappedLinks;
	Link* Map(const char* filename);
	void Unmap();
	void Remap();
	void ResizeFile(size_t size);
	void Close();
};



#endif
