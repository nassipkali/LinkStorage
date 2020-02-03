#include "links.hpp"
#include "linksmap.hpp"

Links::Links(const char* dbname)
{
    links = lmap.Map(dbname);
    LinkCount = (lmap.MapSize - lmap.BlockSize) / sizeof(Link);
}

size_t Links::GetLinkCount() {
    return LinkCount;
}

Link* Links::Create() {
    LinkCount++;
    links[LinkCount].Target = LinkCount;
    links[LinkCount].Source = LinkCount;
    return &links[LinkCount];
}

Link* Links::GetLinkByIndex(link_t index) {
    return &links[index];
}

link_t Links::GetIndexByLink(Link* link) {
    return link - links;
}

void Links::Close() {
    lmap.Unmap();
    lmap.ResizeFile(LinkCount * sizeof(Link));
    lmap.Close();
}
