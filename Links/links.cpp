#include "links.hpp"
#include "linksmap.hpp"

Links::Links(const char* dbname)
{
    links = LinksMap(dbname, BlockSize * BlockCount);
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
