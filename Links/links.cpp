#include "links.hpp"
#include "linksmemory.hpp"

Links::Links(const char* dbname)
{
    LinksArray = (Link*)Memory.Map(dbname);
    if(!LinksArray[1].Source && LinksArray[1].Target == 1) {
        Init();
    }
}

Links::Links(const char* dbname, size_t BlockSize) {
    Memory.BlockSize = BlockSize;
    LinksArray = (Link*)Memory.Map(dbname);
    if(LinksArray[1].Source && LinksArray[1].Target == 1) {
        Init();
    }
}

Link& Links::operator[] (const link_t index) {
    return LinksArray[index];
}

size_t Links::GetLinkCount() {
    return LinkCount;
}

Link* Links::Create() {
    LinkCount++;
    Link* link = &LinksArray[LinkCount];
    return link;
}

Link* Links::Create(link_t target) {
    LinkCount++;
    Link* link = &LinksArray[LinkCount];
    link->Source = LinkCount;
    link->Target = target;
    return link;
}

Link* Links::Create(link_t source, link_t target) {
    LinkCount++;
    Link* link = &LinksArray[LinkCount];
    link->Source = source;
    link->Target = target;
    return link;
}

Link* Links::GetLinkByIndex(link_t index) {
    return &LinksArray[index];
}

link_t Links::GetIndexByLink(Link* link) {
    return (link - LinksArray);
}

void Links::Close() {
    Memory.Close();
}

void Links::Init() {
    CreateNoRet(0, 0);
    CreateNoRet(0, 1);
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
    return LinkCount * sizeof(Link);
}

void Links::CreateNoRet() {
    LinkCount++;
}
void Links::CreateNoRet(link_t target) {
    LinkCount++;
    link_t index = LinkCount;
    LinksArray[index].Source = index;
    LinksArray[index].Target = target;
}
void Links::CreateNoRet(link_t source, link_t target) {
    LinkCount++;
    link_t index = LinkCount;
    LinksArray[index].Source = source;
    LinksArray[index].Target = target;
}




