#include "../linksmap.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>


Link* LinksMap::Map(const char* filename){
    try {
        void* links;
        FileDescriptor = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if(FileDescriptor == -1) {
            throw("Linux/linksmap.cpp: OpenFileErrorException");
        }
        links = mmap(NULL,LinksSize + BlockSize, PROT_READ | PROT_WRITE, MAP_SHARED, FileDescriptor, 0);
        if(links == MAP_FAILED) {
            throw("Linux/linksmap.cpp: MemoryMapErrorException");
        }
        return (Link*)links;
    }
    catch(char *str) {
        std::cout << str << std::endl;
    }
    return NULL;
}

void LinksMap::Unmap() {

}
