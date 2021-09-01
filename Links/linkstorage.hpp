#ifndef LINKSTORAGE_HPP
#define LINKSTORAGE_HPP
#include "linkdata.hpp"
#include "linkindex.hpp"
#include "linksmemory.hpp"
#include <stack>
#include <iostream>
#include <vector>
template <typename T>
class LinkStorage
{
    private:
        MemoryMappedFileArray<LinkData<T>> data_array;
        MemoryMappedFileArray<LinkIndex<T>> index_array;
        T free_links;
        T last_free_link;
    public:
        LinkStorage(const char* data_file, const char* index_file);
        ~LinkStorage();
        T CreateLink(T source, T target);
        T CreateDot();
        void UpdateLink(T linkindex, T source, T target);
        bool Delete(T index);
        T SearchLink(T source, T target);
      	T GetSourcedCount(T link);
        T GetTargetedCount(T link);
        std::vector<T> GetSourced(T link);
        std::vector<T> GetTargeted(T link);
        T GetSourcedWithSource(T link, T source);
        T GetSourcedWithTarget(T link, T target);
        T GetTargetedWithSource(T link, T source);
        T GetTargetedWithTarget(T link, T target);
        T CreateSequence(std::vector<T> links);
        T SearchSequence(std::vector<T> links);
        LinkData<T> GetLinkData(T link);
        LinkIndex<T> GetLinkIndex(T link);
        T GetFreeLinksCount();
        size_t GetLinksCount();
};

template <typename T>
LinkStorage<T>::LinkStorage(const char* data_file, const char* index_file) : data_array(data_file), index_array(index_file) {
    // MetaData loads from 0 link
    if(this->data_array.GetSize() == 0) {
    	LinkData<T> zero_data = {0,0};
    	LinkIndex<T> zero_index = {0,0,0,0,0,0,0,0};
    	this->data_array.Add(zero_data);
    	this->index_array.Add(zero_index);
    }
    this->free_links = this->index_array[0].root_as_source; // Количество неиспользуемых/удаленных связей
    this->last_free_link = this->index_array[0].left_as_source; //Индекс последней удаленной связи
}



template <typename T>
T LinkStorage<T>::CreateLink(T source, T target) {
    if(source == 0 || target == 0) {
        std::cerr << "[LinkStorage] Can't create link with reference to zero" << std::endl;
        return 0;
    }
    
    T link;
    
    if(this->free_links > 0) {
        link = this->last_free_link;
        this->last_free_link = this->data_array[this->last_free_link].source;
        this->free_links--;
        this->data_array[link] = {source, target};
        this->index_array[link] = {0,0,0,1,0,0,0,1};
    }
    else {
    	link = data_array.GetSize();
    	LinkData<T> link_data = {source, target};
    	this->data_array.Add(link_data);
        LinkIndex<T> link_index = {0,0,0,1,0,0,0,1};
        this->index_array.Add(link_index);
    }
    
    return link;
}

template <typename T>
T LinkStorage<T>::CreateDot() {  
    T link;
    
    if(this->free_links > 0) {
        link = this->last_free_link;
        this->last_free_link = this->data_array[this->last_free_link].source;
        this->free_links--;
        this->data_array[link] = {link, link};
        this->index_array[link] = {0,0,0,1,0,0,0,1};
        
    }
    else {
    	link = data_array.GetSize();
    	LinkData<T> link_data = {link, link};
    	this->data_array.Add(link_data);
        LinkIndex<T> link_index = {0,0,0,1,0,0,0,1};
        this->index_array.Add(link_index);
    }
    
    return link;
}

template <typename T>
void LinkStorage<T>::UpdateLink (T link, T source, T target ) {
	if(source == 0 || target == 0) {
        std::cerr << "[LinkStorage] Can't create link with reference to zero" << std::endl;
        return;
    }
    this->data_array[link].source = source;
    this->data_array[link].target = target;
}

template <typename T>
bool LinkStorage<T>::Delete(T link) {
	if(this->free_links == 0) {
		this->last_free_link = link;
		this->free_links++;
	}
	else {
		this->data_array[this->last_free_link].target = link;
		this->data_array[link].source = this->last_free_link;
		this->last_free_link = link;
		this->free_links++;
	}
	return true;
}

template <typename T>
LinkData<T> LinkStorage<T>::GetLinkData(T link) {
    return this->data_array[link];
}

template <typename T>
LinkIndex<T> LinkStorage<T>::GetLinkIndex(T link) {
    return this->index_array[link];
}

template <typename T>
size_t LinkStorage<T>::GetLinksCount() {
	return data_array.GetSize() - 1;
}


template <typename T>
T LinkStorage<T>::GetFreeLinksCount() {
    return this->free_links;
}

template <typename T>
LinkStorage<T>::~LinkStorage() {
	this->index_array[0].root_as_source = this->free_links; // Количество неиспользуемых/удаленных связей
	this->index_array[0].left_as_source = this->last_free_link; //Индекс последней удаленной связи
}



#endif // LINKS_HPP
