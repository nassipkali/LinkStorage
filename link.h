#ifndef LINK_H
#define LINK_H

#include <stddef.h>

typedef size_t link_t;


class Link
{
public:
    link_t Source;
    link_t Target;
    Link(link_t source, link_t target);
};

#endif // LINK_H
