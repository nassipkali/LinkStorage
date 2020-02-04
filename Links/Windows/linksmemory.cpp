#include "../linksmemory.hpp"
#include <windows.h>
#include <iostream>

void LinksMap::ResizeFile(size_t size) {
    try {
        if(!SetFilePointerEx(FileDescriptor.hFile, size, nullptr, FILE_BEGIN)) {
            std::cerr << "[LinksPlatform] Windows/linksmap.cpp: CreateFileErrorException. ERRNO:";
            throw(GetLastError());
        }
        if(!SetEndOfFile(FileDescriptor.hFile)) {
            std::cerr << "[LinksPlatform] Windows/linksmap.cpp: CreateFileErrorException. ERRNO:";
            throw(GetLastError());
        }
        catch(DWORD error)
    }
}

void LinksMap::Map(const char* filename){
    try {
        FileDescriptor.hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(hFile == INVALID_HANDLE_VALUE) {
            std::cerr << "[LinksPlatform] Windows/linksmap.cpp: CreateFileErrorException. ERRNO:";
            throw(GetLastError());
        }
        DWORD dwFileSize = GetFileSize(hFile, nullptr);
        if(dwFileSize == INVALID_FILE_SIZE) {
            CloseHandle(hFile);
            std::cerr << "[LinksPlatform] Windows/linkmap.cpp: GetFileSizeErrorException. ERRNO:";
            throw(GetLastError());
        }
        MemoryUse = dwFileSize;
        LinkCount = MemoryUse / sizeof(Link);
        MapSize = (MemoryUse / BlockSize + 1) * BlockSize;
        FileResize(MapSize);

        FileDescriptor.hMapping = CreateFileMapping(FileDescriptor.hFile, nullptr, PAGE_READWRITE, 0, (DWORD)MapSize, nullptr);
        if(FileDescriptor.hMapping == nullptr) {
            CloseHandle(hFile);
            std::cerr << "[LinksPlatform] Windows/linksmap.cpp: MapViewOfFileErrorException. ERRNO:";
            throw(GetLastError());
        }
        MappedLinks = MapViewOfFile(FileDescriptor.hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, (DWORD)MapSize);
        if(MappedLinks == nullptr) {
            CloseHandle(FileDescriptor.hMapping);
            CloseHandle(FileDescriptor.hFile);
            std::cerr << "[LinksPlatform] Windows/linksmap.cpp: MapViewOfFileErrorException. ERRNO:";
            throw(GetLastError());
        }
    }
    catch(DWORD err) {
        std::cerr << err << std::endl;
    }
    return nullptr;
}

void LinksMap::Unmap() {
    UnMapViewOfFile(MappedLinks);
}

void LinksMap::Remap() {
    UnMapViewOfFile(MappedLinks);
    CloseHandle(hMapping);
    ftruncate(FileDescriptor.hFile, MapSize + BlockSize);
    FileDescriptor.hMapping = CreateFileMapping(FileDescriptor.hFile, nullptr, PAGE_READWRITE, 0, (DWORD)MapSize, nullptr);
    try {
        if(FileDescriptor.hMapping == nullptr) {
            CloseHandle(hFile);
            std::cerr << "[LinksPlatform] Windows/linksmap.cpp: MapViewOfFileErrorException. ERRNO:";
            throw(GetLastError());
        }
        MappedLinks = MapViewOfFile(FileDescriptor.hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, (DWORD)MapSize);
        if(MappedLinks == nullptr) {
            CloseHandle(FileDescriptor.hMapping);
            CloseHandle(FileDescriptor.hFile);
            std::cerr << "[LinksPlatform] Windows/linksmap.cpp: MapViewOfFileErrorException. ERRNO:";
            throw(GetLastError());
        }
    }
    catch(DWORD err) {
        std::cerr << err << std::endl;
    }

}


void LinksMap::Close() {
    UnMap();
    ResizeFile(MemoryUse);
    if(CloseHandle(FileDescriptor.hMapping) == 0) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << GetLastError();
    }
    if(CloseHandle(FileDescriptor.hFile) == 0) {
        std::cerr << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << GetLastError();
    }
}

Link* LinksMemory::LinkAlloc(size_t count) {
    Link* addr = &MappedLinks[LinkCount];
    LinkCount += count;
    MemoryUse += sizeof(Link) * count;
    return addr;
}
