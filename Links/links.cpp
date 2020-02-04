#include "links.hpp"
#include "linksmemory.hpp"

Links::Links(const char* dbname)
{
    Memory.Map(dbname);
    if(!Memory.LinkCount) {
        Init();
    }
}

Links::Links(const char* dbname, size_t BlockSize) {
    Memory.BlockSize = BlockSize;
    Memory.Map(dbname);
    if(!Memory.LinkCount) {
        Init();
    }
}

size_t Links::GetLinkCount() {
    return Memory.LinkCount;
}

Link* Links::Create() {
    Link* link = Memory.LinkAlloc(1);
    return link;
}

Link* Links::Create(link_t target) {
    Link* link = Memory.LinkAlloc(1);
    link->Source = GetIndexByLink(link);
    link->Target = target;
    return link;
}

Link* Links::Create(link_t source, link_t target) {
    Link* link = Memory.LinkAlloc(1);
    link->Source = source;
    link->Target = target;
    return link;
}

Link* Links::GetLinkByIndex(link_t index) {
    return &Memory.MappedLinks[index];
}

link_t Links::GetIndexByLink(Link* link) {
    return (link - Memory.MappedLinks);
}

void Links::Close() {
    Memory.Close();
}

void Links::Init() {
    Link* zero = Create(0, 0);
    Link* one = Create(0, 1);
    uint64_t ExpOfTwo = 1;
    for(int i = 1; i < 64; i++) {
        CreateNoRet(ExpOfTwo, ExpOfTwo);
        ExpOfTwo *= 2;
    }
    std::cout << "First Initialization." << std::endl;
}

size_t Links::GetMemoryMapSize() {
    return Memory.MapSize;
}

size_t Links::GetMemoryUse() {
    return Memory.MemoryUse;
}

void Links::CreateNoRet() {
    Memory.LinkAllocNoRet(1);
}
void Links::CreateNoRet(link_t target) {
    link_t index = Memory.LinkAllocIndex();
    Memory.MappedLinks[index].Source = index;
    Memory.MappedLinks[index].Target = target;
}
void Links::CreateNoRet(link_t source, link_t target) {
    link_t index = Memory.LinkAllocIndex();
    Memory.MappedLinks[index].Source = source;
    Memory.MappedLinks[index].Target = target;
}




