#include "../linksmap.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>


Link* LinksMap::Map(const char* filename){
    try {
        FileDescriptor = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if(FileDescriptor == -1) {
            std::cout << "[LinksPlatform] Linux/linksmap.cpp: OpenFileErrorException, ERRNO: ";
            throw(errno);
        }

        struct stat st;
        if(fstat(FileDescriptor, &st) == -1) {
            std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileStatErrorException, ERRNO: ";
            throw(errno);
        }

        MapSize = st.st_size + BlockSize;

        if(ftruncate(FileDescriptor, MapSize) == -1) {
            std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileTruncateErrorException, ERRNO: ";
            throw(errno);
        }

        MappedLinks = mmap(NULL,MapSize, PROT_READ | PROT_WRITE, MAP_SHARED, FileDescriptor, 0);
        if(MappedLinks == MAP_FAILED) {
            std::cout << "[LinksPlatform] Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: ";
            throw(errno);
        }
        return (Link*)MappedLinks;
    }
    catch(int err) {
        std::cout << err << std::endl;
    }
    return nullptr;
}

void LinksMap::Unmap() {
    if(munmap(MappedLinks, MapSize) == -1) {
        std::cout << "[LinksPlatform] Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: " << errno;
    }
}

void LinksMap::ResizeFile(size_t size) {
    if(ftruncate(FileDescriptor, size) == -1) {
            std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileTruncateErrorException, ERRNO: ";
            throw(errno);
    }
}

void LinksMap::Close() {
    if(close(FileDescriptor) == -1) {
        std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << errno;
    }
}
