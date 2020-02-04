#ifndef LINKS_HPP
#define LINKS_HPP
#include "link.hpp"
#include "linksmap.hpp"
#include <stdint.h>
#include <iostream>

class Links
{
private:
	link_t LinkCount = 1;
    	LinksMap lmap;
    	Link *links;
public:
	Links(const char* dbname);
	Link* Create();
	Link* Create(link_t source, link_t target);
	void Close();
	size_t GetLinkCount();
	Link* GetLinkByIndex(link_t index);
	link_t GetIndexByLink(Link* link);
};

#endif // LINKS_HPP
