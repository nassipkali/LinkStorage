#ifndef LINKS_HPP
#define LINKS_HPP
#include "link.hpp"


class Links
{
private:
    link_t LinkCount = 0;
    size_t BlockSize = 1024 * 1024 * 64; // 64MB
    size_t BlockCount = 1;
    Link *links;
public:
    Links(const char* dbname);
    Link Create();
    size_t GetLinkCount();
};

#endif // LINKS_HPP
