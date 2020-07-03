#ifndef LINKSMAP_HPP
#define LINKSMAP_HPP

#include <stddef.h>
#include "link.hpp"
#include "FileDescriptor.h"



class LinksMemory {
    public:
        size_t MapSize = 0;
        fd_t FileDescriptor;
        void* MappedMemory;
        void* Map(const char* filename, size_t offset);
        void* Map(fd_t FileDescriptor, size_t offset);
        void Unmap();
        void* Remap(size_t size);
        void* ShrinkMap(size_t offset);
        void ResizeFile(size_t size);
        void Close();
};



#endif
