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

        if(ftruncate(FileDescriptor, BlockSize) == -1) {
            std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileTruncateErrorException, ERRNO: ";
            throw(errno);
        }
        LinksSize = st.st_size + BlockSize;

        MappedLinks = mmap(NULL,LinksSize, PROT_READ | PROT_WRITE, MAP_SHARED, FileDescriptor, 0);
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

}
