#include "../linksmap.hpp"
#include <windows.h>
#include <iostream>


Link* LinksMap::Map(const char* filename){
    try {
        HANDLE hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(hFile == INVALID_HANDLE_VALUE) {
            throw("Windows/linksmap.cpp: CreateFileErrorException");
        }
        DWORD dwFileSize = GetFileSize(hFile, nullptr);
        if(dwFileSize == INVALID_FILE_SIZE) {
            CloseHandle(hFile);
            throw("Windows/linkmap.cpp: GetFileSizeErrorException");
        }
        LinksSize = dwFileSize + BlockSize;
        HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, (DWORD)LinksSize, NULL);
        if(hMapping == nullptr) {
            CloseHandle(hFile);
            throw("Windows/linksmap.cpp: CreateFileMappingErrorException");
        }
        MappedLinks = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, (DWORD)LinksSize);
        if(MappedLinks == nullptr) {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            throw("Windows/linksmap.cpp: MapViewOfFileErrorException");
        }
        FileDescriptor = hFile;
        FileMapDescriptor = hMapping;
    }
    catch(char *str) {
        std::cout << str << std::endl;
    }
    return nullptr;
}


