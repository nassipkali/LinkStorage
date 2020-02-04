#include "../linksmemory.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>


void LinksMemory::ResizeFile(size_t size) {
    if(ftruncate(FileDescriptor, size) == -1) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: FileTruncateErrorException, ERRNO: " << errno << std::endl;
    }
}


void LinksMemory::Map(const char* filename){
    try {
        FileDescriptor = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if(FileDescriptor == -1) {
            std::cerr << "[LinksPlatform] Linux/linksmap.cpp: OpenFileErrorException, ERRNO: ";
            throw(errno);
        }
        struct stat st;
        if(fstat(FileDescriptor, &st) == -1) {
            std::cerr << "[LinksPlatform] Linux/linksmap.cpp: FileStatErrorException, ERRNO: ";
            throw(errno);
        }

        MemoryUse = st.st_size;
        LinkCount = MemoryUse / sizeof(Link);
        MapSize = (MemoryUse / BlockSize + 1) * BlockSize;
        ResizeFile(MapSize);

        MappedLinks = (Link*)mmap(nullptr, MapSize, PROT_READ | PROT_WRITE, MAP_SHARED, FileDescriptor, 0);
        if(MappedLinks == MAP_FAILED) {
            std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: ";
            throw(errno);
        }
    }
    catch(int err) {
        std::cerr << err << std::endl;
    }
}

void LinksMemory::Unmap() {
    if(munmap(MappedLinks, MapSize) == -1) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: " << errno << std::endl;
    }
}

void LinksMemory::Remap() {
    if(mremap(MappedLinks, MapSize, MapSize + BlockSize, 0, 0) == MAP_FAILED) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryReMapErrorException, ERRNO: " << errno << std::endl;
    }
}


void LinksMemory::Close() {
    ResizeFile(MemoryUse);
    Unmap();
    if(close(FileDescriptor) == -1) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << errno << std::endl;
    }
}

Link* LinksMemory::LinkAlloc(size_t count) {
    Link* addr = &MappedLinks[LinkCount];
    LinkCount += count;
    MemoryUse += sizeof(Link) * count;
    return addr;
}

void LinksMemory::LinkAllocNoRet(size_t count) {
    LinkCount += count;
    MemoryUse += sizeof(Link) * count;
}

link_t LinksMemory::LinkAllocIndex() {
    LinkCount++;
    MemoryUse += sizeof(Link);
    return LinkCount - 1;
}


