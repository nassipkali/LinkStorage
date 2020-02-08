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


void* LinksMemory::Map(const char* filename, size_t offset = 0){
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


        this->MapSize = st.st_size + offset;
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
    if(munmap(this->MappedMemory, this->MapSize) == -1) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: " << errno << std::endl;
    }
}

void* LinksMemory::Remap(size_t size) {
    this->MappedMemory = mremap(this->MappedMemory, this->MapSize, size, 0, 0);
    if(this->MappedMemory == MAP_FAILED) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryReMapErrorException, ERRNO: " << errno << std::endl;
    }
    this->MapSize = size;
    return this->MappedMemory;
}

void * LinksMemory::ShrinkMap(size_t offset) {
    this->MappedMemory = mremap(this->MappedMemory, this->MapSize, this->MapSize + offset, 0, 0);
    if(this->MappedMemory == MAP_FAILED) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: MemoryReMapErrorException, ERRNO: " << errno << std::endl;
    }
    this->MapSize += offset;
    return this->MappedMemory;
}


void LinksMemory::Close() {
    if(MappedMemory != nullptr) {
        this->Unmap();
    }
    if(close(FileDescriptor) == -1) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << errno << std::endl;
    }
}


