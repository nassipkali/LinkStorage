#ifndef LINKS_HPP
#define LINKS_HPP
#include "link.hpp"
#include "linksmemory.hpp"
#include <stdint.h>
#include <iostream>



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
    void InsertToSourceTree(link_t index);
    void InsertToSourceTree(Link* link);
    void InsertToTargetTree(link_t index);
    void InsertToTargetTree(Link* link);
	void Close();
	size_t GetAllocatedLinksCount();
    size_t GetReservedLinksCount();
    size_t GetFreeLinksCount();
	Link* GetLinkByIndex(link_t index);
	link_t GetIndexByLink(Link* link);
	size_t GetMemoryMapSize();
	size_t GetMemoryUse();
    link_t Search(link_t Source, link_t Target);
    Link* SearchLink(link_t Source, link_t Target);
 	template<typename T>
	Link* NumberToLink(T num);
    template<typename T>
    T LinkToNumber(Link* link);
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
        if(link->Source == 1) {
            num ^= 1 << (link->Target - 1);
            break;
        }
        else {
            num ^= 1 << (link->Target - 1);
            if(link->Source != 0) {
                link = this->GetLinkByIndex(link->Source);
            }
            else {
                return num;
            }
        }
    }
    return num;
}

#endif // LINKS_HPP
