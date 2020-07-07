# LinksPlatform rewriting to C++.
Original Idea from https://github.com/linksplatform

```C++
#include <iostream>
#include "Links/links.hpp"

typedef Link<size_t> Link_t;

int main()
{
    Links<size_t> links("db.links");
    std::cout << "Hello World!" << std::endl;
    Link_t *link = links.Create(1, 1);
    std::cout << "Link Count: " << links.GetLinkCount() << std::endl;
    std::cout << "My first link! (" << link->Source<< " " << link->Target << ")\n";
    std::cout << "Link index" << links.GetIndexByLink(link) << std::endl;
    links.Close();
    return 0;
}
```
