#include "../linksmap.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

Link* LinksMap(const char* filename, size_t BlockSize){
    int fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    return (Link*)mmap(NULL, BlockSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}


