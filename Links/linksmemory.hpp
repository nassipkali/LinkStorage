#ifndef LINKSMEMORY_HPP
#define LINKSMEMORY_HPP

#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>


template <typename T>
class MemoryMappedFileArray {
    private:
        size_t size;
        size_t cap;
        size_t step = 4096;
        int fd;
        T* array;
        void PrepareCapForAdd();
    public:
        MemoryMappedFileArray(const char* filename);
        MemoryMappedFileArray(const char* filename, size_t page_count);
        ~MemoryMappedFileArray();
        void Add(T element);
        size_t GetSize();
        size_t GetCap();
        T& operator[](size_t index);
};



template <typename T>
MemoryMappedFileArray<T>::MemoryMappedFileArray(const char* filename) {
    this->fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(this->fd == -1) {
        std::cerr << "[MemoryMappedFileArray] linksmemory.hpp: OpenFileErrorException, ERRNO: " << errno << std::endl;
    }
    struct stat st;
    if(fstat(this->fd, &st) == -1) {
        std::cerr << "[MemoryMappedFileArray] linksmemory.hpp: FileStatErrorException, ERRNO: " << errno << std::endl;
    }
    this->size = st.st_size / sizeof(T);
    size_t need_cap = this->size * sizeof(T);
    this->cap = (need_cap / this->step + 1) * this->step;
    
    ftruncate(this->fd, this->cap);
    
    this->array = (T*)mmap(NULL, this->cap, PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0);
    if(this->array == MAP_FAILED) {
        std::cerr << "[MemoryMappedFileArray] linksmemory.hpp: MemoryMapErrorException, ERRNO: " << errno << std::endl;
    }
}

template <typename T>
MemoryMappedFileArray<T>::MemoryMappedFileArray(const char* filename, size_t page_count) {
    this->step = 4096 * page_count;
    this->fd = open(filename, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(this->fd == -1) {
        std::cerr << "[MemoryMappedFileArray] linksmemory.hpp: OpenFileErrorException, ERRNO: " << errno << std::endl;
    }
    struct stat st;
    if(fstat(this->fd, &st) == -1) {
        std::cerr << "[MemoryMappedFileArray] linksmemory.hpp: FileStatErrorException, ERRNO: " << errno << std::endl;
    }
    this->size = st.st_size / sizeof(T);
    size_t need_cap = this->size * sizeof(T);
    this->cap = (need_cap / this->step + 1) * this->step;
    
    ftruncate(this->fd, this->cap);
    
    this->array = (T*)mmap(NULL, this->cap, PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0);
    if(this->array == MAP_FAILED) {
        std::cerr << "[MemoryMappedFileArray] linksmemory.hpp: MemoryMapErrorException, ERRNO: " << errno << std::endl;
    }
}

template <typename T>
void MemoryMappedFileArray<T>::Add(T element) {
    this->PrepareCapForAdd();
    this->array[this->size] = element;
    this->size++;
}	

template <typename T>
void MemoryMappedFileArray<T>::PrepareCapForAdd() {
    size_t need_cap = (this->size + 1) * sizeof(LinkData<T>);
    if(need_cap >= this->cap) {
        size_t new_cap = this->cap + this->step;
        ftruncate(this->fd, new_cap);
        this->array = (T*)mremap(this->array, this->cap, new_cap, 0, 0);
        if(this->array == MAP_FAILED) {
            std::cerr << "[MemoryMappedFileArray] linksmemory.hpp: MemoryReMapErrorException, ERRNO: " << errno << std::endl;
        }
        this->cap = new_cap;
    }
}

template <typename T>
T& MemoryMappedFileArray<T>::operator[](size_t index) {
    return this->array[index];
}

template <typename T>
size_t MemoryMappedFileArray<T>::GetSize() {
    return this->size;
}

template <typename T>
size_t MemoryMappedFileArray<T>::GetCap() {
    return this->cap;
}

template <typename T>
MemoryMappedFileArray<T>::~MemoryMappedFileArray() {
    if(munmap(this->array, this->cap) == -1) {
        std::cerr << "[MemoryMappedFileArray] Linux/linksmap.cpp: MemoryMapErrorException, ERRNO: " << errno << std::endl;
    }
    ftruncate(this->fd, this->size * sizeof(T));
    if(close(this->fd) == -1) {
        std::cerr << "[MemoryMappedFileArray] Linux/linksmap.cpp: FileCloseErrorException, ERRNO: " << errno << std::endl;
    }
}



#endif
