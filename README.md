# LinksPlatform rewriting to C++.
Original Idea from https://github.com/linksplatform

```C++
#include <iostream>
#include "Links/links.hpp"

typedef size_t link_t;

int main()
{
    Links<size_t> links("db.links");
    std::cout << "Hello World!" << std::endl;
    link_t link = links.Create(1, 1);
    std::cout << "Link Count: " << links.GetLinkCount() << std::endl;
    LinkData<link_t> linkData = links.GetLinkData(link);
    std::cout << "My first link! (" << linkData.Source<< " " << linkData.Target << ")\n";
    std::cout << "Link index" << link << std::endl;
    links.Close();
    return 0;
}
```
