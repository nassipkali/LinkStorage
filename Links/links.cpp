#include "links.hpp"
#include "linksmap.hpp"

Links::Links(const char* dbname)
{
    links = lmap.Map(dbname);
    LinkCount = (lmap.LinksSize - lmap.BlockSize) / sizeof(Link);
}

size_t Links::GetLinkCount() {
    return LinkCount;
}

Link Links::Create() {
    LinkCount++;
    links[LinkCount].Target = LinkCount;
    links[LinkCount].Source = LinkCount;
    return links[LinkCount];
}

void Links::Close() {
    lmap.ResizeFile(LinkCount * sizeof(Link));
    lmap.Close();
    lmap.Unmap();
}
