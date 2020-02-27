#ifndef LINKS_HPP
#define LINKS_HPP
#include "link.hpp"
#include "linksmemory.hpp"
#include <stdint.h>
#include <iostream>
#include <stack>



class Links
{
private:
    LinksMemory Memory;
    Link* LinksArray;
    link_t AllocatedLinks;
    link_t ReservedLinks;
    link_t FreeLinks;
    link_t FirstFreeLink;
    link_t FirstAsSource;
    link_t FirstAsTarget;
    link_t LastFreeLink;
    link_t ReservedField;
    size_t BlockSize = 1024 * 1024 * 16;
    void Init();
public:
	Links(const char* dbname, size_t blocksize = 1024 * 1024 * 16);
    Link* operator[] (const link_t index);
	void Create();
	void Create(link_t target);
	void Create(link_t source, link_t target);
	Link* CreateLink();
	Link* CreateLink(link_t target);
	Link* CreateLink(link_t source, link_t target);
	void Delete(link_t index);
	void Delete(Link* link);
    void DeleteSequence(link_t index);
    void DeleteSequence(Link* link);
    void UpdateLink(Link* link, link_t source, link_t target);
    void UpdateLink(link_t linkindex, link_t source, link_t target);
	void Close();
	size_t GetAllocatedLinksCount();
    size_t GetReservedLinksCount();
    size_t GetFreeLinksCount();
	Link* GetLinkByIndex(link_t index);
	link_t GetIndexByLink(Link* link);
	size_t GetMemoryMapSize();
	size_t GetMemoryUse();
    link_t BFactorBySource(Link* node);
    link_t BFactorByTarget(Link* node);
    void FixSizeBySource(Link* node);
    void FixSizeByTarget(Link* node);
    Link* LeftRotateBySource(Link* node);
    Link* RightRotateBySource(Link* node);
    Link* LeftRotateByTarget(Link* node);
    Link* RightRotateByTarget(Link* node);
    Link* BalanceBySource(Link* node);
    Link* BalanceByTarget(Link* node);
    Link* InsertLinkBySource(Link* node);
    Link* InsertLinkByTarget(Link* node);
    Link* SearchLinkBySource(link_t Source, link_t Target);
    Link* SearchLinkByTarget(link_t Source, link_t Target);
 	template<typename T>
	Link* NumberToLink(T num);
    template<typename T>
    T LinkToNumber(Link* link);
    template<typename T>
    Link* ArrayToSequence(T *array, size_t size);
    template<typename T>
    T* SequenceToArray(Link* link);
    template<typename T>
    T* SequenceToArray(link_t index);
};


template<typename T>
Link* Links::NumberToLink(T num) {
	Link* link = this->CreateLink(0, 0);
	T ExpOfTwo = 1;
	int SecondIter = 0;
	for(int i = 0; i < sizeof(T)*8; i++) {
		if(ExpOfTwo & num) {
			link->Target = i + 1;
			SecondIter = i + 1;
			ExpOfTwo = ExpOfTwo << 1;
			break;
		}
		ExpOfTwo = ExpOfTwo << 1;
	}
	for(int i = SecondIter; i < sizeof(T)*8; i++){
		if(ExpOfTwo & num) {
			link = this->CreateLink(GetIndexByLink(link), i + 1);
		}
		ExpOfTwo = ExpOfTwo << 1;
	}
	return link;
}

template <typename T>
T Links::LinkToNumber(Link* link) {
    T num = 0;
    for(int i = 0; i < 64; i++) {
        num ^= 1 << (link->Target - 1);
        if(link->Source != 0) {
            link = this->GetLinkByIndex(link->Source);
        }
        else {
            return num;
        }
    }
    return num;
}

template <typename T>
Link* Links::ArrayToSequence ( T* array, size_t size) {
    Link* rootLink = CreateLink(0, 0);
    Link* link = NumberToLink(array[0]);
    rootLink->Target = GetIndexByLink(link);
    for(int i = 1; i < size; i++) {
        link = NumberToLink(array[i]);
        rootLink = CreateLink(GetIndexByLink(rootLink), GetIndexByLink(link));
    }
    return rootLink;
}

template <typename T>
T* Links::SequenceToArray(Link* link) {
    std::stack <T> seqStack;
    while(1) {
        seqStack.push(LinkToNumber<T>(GetLinkByIndex(link->Target)));
        if(link->Source == 0) {
            break;
        }
        else {
            link = GetLinkByIndex(link->Source);
        }
    }
    auto size = seqStack.size();
    T* array = (T*)malloc(sizeof(T) * size);
    for(int i = 0; i < size; i++) {
            array[i] = seqStack.top();
            seqStack.pop();
    }
    return array;
}


#endif // LINKS_HPP
