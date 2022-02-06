# LinkStorage

```C++
#include <fmt/core.h>
#include "Links/linkstorage.hpp"

auto main() -> int {
    auto links = LinkStorage<size_t>("db.links", "db.index");
    auto link = links.Create(1, 1);
    auto linkData = links.GetLinkData(link);
    fmt::println("Links Count: {}", links.GetLinksCount());
    fmt::println("My first link! ({}: {} -> {})", 
            link, linkData.Source, linkData.Target);
}
```
