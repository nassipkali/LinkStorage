#ifndef LINKS_HPP
#define LINKS_HPP
#include "link.hpp"
#include "linksmemory.hpp"
#include <stdint.h>
#include <iostream>

class Links
{
private:
    	LinksMemory Memory;
public:
	Links(const char* dbname);
	Links(const char* dbname, size_t BlockSize);
	Link* Create();
	Link* Create(link_t target);
	Link* Create(link_t source, link_t target);
	void Close();
	size_t GetLinkCount();
	Link* GetLinkByIndex(link_t index);
	link_t GetIndexByLink(Link* link);
	size_t GetMemoryMapSize();
	size_t GetMemoryUse();
};

#endif // LINKS_HPP
