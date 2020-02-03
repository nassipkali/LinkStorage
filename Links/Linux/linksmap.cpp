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
        struct stat st;
        if(fstat(FileDescriptor, &st) == -1) {
            throw("Linux/linksmap.cpp: FileStatErrorException, ERRNO: ", errno);
        }
        LinksSize = st.st_size + BlockSize;
        if(FileDescriptor == -1) {
            throw("Linux/linksmap.cpp: OpenFileErrorException, ERRNO: ", errno);
        }
        MappedLinks = mmap(NULL,LinksSize, PROT_READ | PROT_WRITE, MAP_SHARED, FileDescriptor, 0);
        if(MappedLinks == MAP_FAILED) {
            throw("Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: ", errno);
        }
        return (Link*)MappedLinks;
    }
    catch(char *str) {
        std::cout << str << std::endl;
    }
    return nullptr;
}

void LinksMap::Unmap() {

}
