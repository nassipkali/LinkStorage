#include "links.h"
#include "memorymap.h"

Links::Links(const char* dbname)
{

}

size_t Links::GetLinkCount() {
    return LinkCount;
}

Link Links::Create() {
    LinkCount++;
    return Link(LinkCount + 1, LinkCount + 1);
}
