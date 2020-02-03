#include "../linksmap.hpp"
#include <windows.h>
#include <iostream>

void ftruncate( HANDLE hFile, size_t size) {
    try {
        if(!SetFilePointerEx(hFile, size, nullptr, FILE_BEGIN)) {
            std::cout << "[LinksPlatform] Windows/linksmap.cpp: CreateFileErrorException";
            throw(GetLastError());
        }
        if(!SetEndOfFile(hFile)) {
            std::cout << "[LinksPlatform] Windows/linksmap.cpp: CreateFileErrorException";
            throw(GetLastError());
        }
        catch(DWORD error)
    }
}

Link* LinksMap::Map(const char* filename){
    try {
        HANDLE hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(hFile == INVALID_HANDLE_VALUE) {
            std::cout << "[LinksPlatform] Windows/linksmap.cpp: CreateFileErrorException";
            throw(GetLastError());
        }
        DWORD dwFileSize = GetFileSize(hFile, nullptr);
        if(dwFileSize == INVALID_FILE_SIZE) {
            CloseHandle(hFile);
            std::cout << "[LinksPlatform] Windows/linkmap.cpp: GetFileSizeErrorException";
            throw(GetLastError());
        }

        MapSize = dwFileSize + BlockSize;
        ftruncate(hFile, MapSize);

        HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, (DWORD)MapSize, nullptr);
        if(hMapping == nullptr) {
            CloseHandle(hFile);
            std::cout << "[LinksPlatform] Windows/linksmap.cpp: MapViewOfFileErrorException";
            throw(GetLastError());
        }
        MappedLinks = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, (DWORD)MapSize);
        if(MappedLinks == nullptr) {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            std::cout << "[LinksPlatform] Windows/linksmap.cpp: MapViewOfFileErrorException";
            throw(GetLastError());
        }
        FileDescriptor = hFile;
        FileMapDescriptor = hMapping;
    }
    catch(DWORD err) {
        std::cout << str << std::endl;
    }
    return nullptr;
}

void LinksMap::Unmap() {
    UnMapViewOfFile(MappedLinks);
}

void LinksMap::ResizeFile(size_t size) {
    ftruncate(FileDescriptor, size)
}

void LinksMap::Close() {
    if(CloseHandle(MapFileDescriptor) == 0) {
        std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << GetLastError();
    }
    if(CloseHandle(FileDescriptor) == 0) {
        std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << GetLastError();
    }
}

