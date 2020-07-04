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
};




#endif // LINKS_HPP
