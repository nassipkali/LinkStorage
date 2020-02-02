#include "memorymap.h"

#if defined(unix) // Linux/MacOS
#include <unistd.h>
#include <sys/mman.h>
#elif defined(WIN32) // Windows 32/64bit
#include <windows.h>
#endif

void* MemoryMap(const char* filename) {
}
bool MemoryUnmap(void* addr, size_t length) {
}


