#include <stddef.h>
#include "link.hpp"


Link* LinksMap(const char* filename, size_t BlockSize);

void LinksUnmap(Link* links, size_t size);
