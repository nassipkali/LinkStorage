#include <stddef.h>

enum ReadFlags {
    PROT_READ = 0,
    PROT_WRITE = 1,
    PROT_EXEC = 2
};

void* MemoryMap(const char *filename, size_t length, int flags);
bool MemoryUnmap(void* addr, size_t length);
void* MemoryRemap(void* old_address, size_t old_size, size_t new_size, int flags);
