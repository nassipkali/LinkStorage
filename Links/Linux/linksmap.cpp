#include "../linksmap.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>


Link* LinksMap(const char* filename, size_t LinksSize){
    try {
        int fd;
        void* links;
        if(open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR) == -1) {
            throw("Linux/linksmap.cpp: OpenFileErrorException");
        }
        links = mmap(NULL, LinksSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(links == MAP_FAILED) {
            throw("Linux/linksmap.cpp: MemoryMapErrorException");
        }
    }
    catch(char *str) {
        cout << str << endl;
    }
}

void LinksUnmap(Link* links, size_t LinksSize) {

}
