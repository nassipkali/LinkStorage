#ifndef LINKS_H
#define LINKS_H
#include "link.h"


class Links
{
private:
    size_t LinkCount = 0;
    Link *links;
public:
    Links(const char* dbname);
    Link Create();
    size_t GetLinkCount();
};

#endif // LINKS_H
