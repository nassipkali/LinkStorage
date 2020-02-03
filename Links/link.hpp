#ifndef LINK_HPP
#define LINK_HPP
#include <stddef.h>

typedef size_t link_t;


class Link
{
public:
    link_t Source;
    link_t Target;
    Link(link_t source, link_t target);
};
#endif
