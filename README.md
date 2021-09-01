# LinkStorage


```C++
#include <iostream>
#include "Links/linkstorage.hpp"

typedef size_t link_t;

int main()
{
    LinkStorage<size_t> links("db.links", "db.index");
    std::cout << "Hello World!" << std::endl;
    link_t link = links.Create(1, 1);
    std::cout << "Link Count: " << links.GetLinksCount() << std::endl;
    LinkData<link_t> linkData = links.GetLinkData(link);
    std::cout << "My first link! (" << linkData.Source<< " " << linkData.Target << ")\n";
    std::cout << "Link index" << link << std::endl;
    return 0;
}
```
