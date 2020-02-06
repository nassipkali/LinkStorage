#include "links.hpp"
#include "linksmemory.hpp"

Links::Links(const char* dbname)
{
    MetaData = (size_t*)Memory.Map(dbname);
    LinkCount = MetaData[0];
    IndexTreeRoot = MetaData[1];
    DeletedList = MetaData[2];
    FreeLinkCount = MetaData[3];
    LinksArray = (Link*)(MetaData + 8);
    if(LinkCount == 0) {
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

Link* Links::operator[] (const link_t index) {
    return &LinksArray[index];
}

size_t Links::GetLinkCount() {
    return LinkCount;
}

Link* Links::Create() {
    LinkCount++;
    if(FreeLinkCount > 0) {
        Link* link = &LinksArray[DeletedList];
        DeletedList = LinksArray[DeletedList].Source;
    }
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

void Links::Delete(link_t index) {
    FreeLinkCount++;
    LinksArray[DeletedList].Target = index;
    LinksArray[index].Source = DeletedList;
    DeletedList = index;
}

void Links::Delete(Link* link) {
    FreeLinkCount++;
    link_t index = GetIndexByLink(link);
    LinksArray[DeletedList].Target = index;
    link->Source = DeletedList;
    DeletedList = index;
}

Link* Links::GetLinkByIndex(link_t index) {
    return &LinksArray[index];
}

link_t Links::GetIndexByLink(Link* link) {
    return (link - LinksArray);
}

void Links::Close() {
    MetaData[0] = LinkCount;
    MetaData[1] = NumberTreeRoot;
    MetaData[2] = IndexTreeRoot;
    MetaData[3] = DeletedList;
    MetaData[4] = FreeLinkCount;
    Memory.Close();
}

void Links::Init() {
    CreateNoRet(0, 1);
    uint64_t ExpOfTwo = 1;
    for(int i = 0; i < 64; i++) {
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




