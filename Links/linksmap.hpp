#ifndef LINKSMAP_HPP
#define LINKSMAP_HPP

#include <stddef.h>
#include "link.hpp"


typedef int fd_t;

class LinksMap {
public:
	size_t BlockSize = 1024 * 1024 * 64; // 64MB
    	size_t LinksSize = 0;
	fd_t FileDescriptor;
	void* MappedLinks;
	Link* Map(const char* filename);
	void Unmap();
};

#endif
