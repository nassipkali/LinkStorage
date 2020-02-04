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
        FileDescriptor.hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
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
        ftruncate(FileDescriptor.hFile, MapSize);

        FileDescriptor.hMapping = CreateFileMapping(FileDescriptor.hFile, nullptr, PAGE_READWRITE, 0, (DWORD)MapSize, nullptr);
        if(FileDescriptor.hMapping == nullptr) {
            CloseHandle(hFile);
            std::cout << "[LinksPlatform] Windows/linksmap.cpp: MapViewOfFileErrorException";
            throw(GetLastError());
        }
        MappedLinks = MapViewOfFile(FileDescriptor.hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, (DWORD)MapSize);
        if(MappedLinks == nullptr) {
            CloseHandle(FileDescriptor.hMapping);
            CloseHandle(FileDescriptor.hFile);
            std::cout << "[LinksPlatform] Windows/linksmap.cpp: MapViewOfFileErrorException";
            throw(GetLastError());
        }
        return (Link*)MappedLinks;
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
    ftruncate(FileDescriptor.hFile, size)
}

void LinksMap::Close() {
    if(CloseHandle(FileDescriptor.hMapping) == 0) {
        std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << GetLastError();
    }
    if(CloseHandle(FileDescriptor.hFile) == 0) {
        std::cout << "[LinksPlatform] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << GetLastError();
    }
}

