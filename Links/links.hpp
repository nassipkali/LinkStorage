#ifndef LINKS_HPP
#define LINKS_HPP
#include "link.hpp"
#include "linksmap.hpp"

class Links
{
private:
    link_t LinkCount = 0;
    LinksMap lmap;
    Link *links;
public:
    Links(const char* dbname);
    Link Create();
    void Close();
    size_t GetLinkCount();
};

#endif // LINKS_HPP
