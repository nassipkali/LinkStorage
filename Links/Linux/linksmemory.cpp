#include "../linksmemory.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>


void LinksMemory::ResizeFile(size_t size) {
    try {
        if(ftruncate(FileDescriptor, size) == -1) {
            std::cerr << "[LinksPlatform] Linux/linksmap.cpp: FileTruncateErrorException, ERRNO: ";
        }
    }
    catch(int err) {
        std::cerr << errno << std::endl;
    }
}


void* LinksMemory::Map(const char* filename){
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


        MapSize = (st.st_size / BlockSize + 1) * BlockSize;
        ResizeFile(MapSize);

        MappedMemory = mmap(nullptr, MapSize, PROT_READ | PROT_WRITE, MAP_SHARED, FileDescriptor, 0);
        if(MappedMemory == MAP_FAILED) {
            std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: ";
            throw(errno);
        }
        return MappedMemory;
    }
    catch(int err) {
        std::cerr << err << std::endl;
    }
    return MappedMemory;
}

void LinksMemory::Unmap() {
    if(munmap(MappedMemory, MapSize) == -1) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: " << errno << std::endl;
    }
}

void* LinksMemory::Remap() {
    MappedMemory = mremap(MappedMemory, MapSize, MapSize + BlockSize, 0, 0);
    if(MappedMemory == MAP_FAILED) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryReMapErrorException, ERRNO: " << errno << std::endl;
    }
    return MappedMemory;
}


void LinksMemory::Close() {
    Unmap();
    if(close(FileDescriptor) == -1) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << errno << std::endl;
    }
}


