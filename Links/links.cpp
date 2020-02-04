#include "links.hpp"
#include "linksmemory.hpp"

Links::Links(const char* dbname)
{
    Memory.Map(dbname);
}

Links::Links(const char* dbname, size_t BlockSize) {
    Memory.BlockSize = BlockSize;
    Memory.Map(dbname);
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

size_t Links::GetMemoryMapSize() {
    return Memory.MapSize;
}

size_t Links::GetMemoryUse() {
    return Memory.MemoryUse;
}


