#include "links.hpp"
#include "linksmemory.hpp"


Links::Links(const char* dbname, size_t blocksize) {
    LinksArray = (Link*)Memory.Map(dbname, blocksize);
    // MetaData loads from 0 link
    AllocatedLinks = LinksArray[0].Source; //Количество используемых связей 
    ReservedLinks = LinksArray[0].Target; //Количество выделенных связей
    FreeLinks = LinksArray[0].LeftAsSource; // Количество неиспользуемых/удаленных связей
    FirstFreeLink = LinksArray[0].RightAsSource; //Индекс первой удаленной связи
    FirstAsSource = LinksArray[0].SizeAsSource; //Корень дерева отсортированных по Source
    FirstAsTarget = LinksArray[0].LeftAsTarget; //Корень дерева отсортированных по Target
    LastFreeLink = LinksArray[0].RightAsTarget; //Индекс последней удаленной связи
    ReservedField = LinksArray[0].SizeAsTarget; //Поле не используется
    
    ReservedLinks = this->Memory.MapSize / sizeof(Link);
    if(AllocatedLinks == 0) {
        AllocatedLinks = 1;
    }
}



size_t Links::GetAllocatedLinksCount() {
    return AllocatedLinks;
}

size_t Links::GetReservedLinksCount() {
    return ReservedLinks;
}

size_t Links::GetFreeLinksCount() {
    return FreeLinks;
}




Link* Links::CreateLink(link_t target) {
    Link* link;
    if(FreeLinks > 0) {
        link = &LinksArray[LastFreeLink];
        LastFreeLink = LinksArray[LastFreeLink].Source;
    }
    else {
        link = &LinksArray[AllocatedLinks];
        AllocatedLinks++;
    }
    link->Source = AllocatedLinks;
    link->Target = target;
    link->LeftAsSource = 0;
    link->LeftAsTarget = 0;
    link->RightAsSource = 0;
    link->RightAsTarget = 0;
    link->RootAsSource = 0;
    link->RootAsTarget = 0;
    link->SizeAsSource = 0;
    link->SizeAsTarget = 0;
    InsertLinkBySource(link);
    InsertLinkByTarget(link);
    return link;
}









Link* Links::GetLinkByIndex(link_t index) {
    return &LinksArray[index];
}

link_t Links::GetIndexByLink(Link* link) {
    return (link - LinksArray);
}

void Links::Close() {
    LinksArray[0].Source = AllocatedLinks; //Количество используемых связей 
    LinksArray[0].Target = ReservedField; //Количество выделенных связей
    LinksArray[0].LeftAsSource = FreeLinks; // Количество неиспользуемых/удаленных связей
    LinksArray[0].RightAsSource = FirstFreeLink; //Индекс первой удаленной связи
    LinksArray[0].SizeAsSource = FirstAsSource; //Корень дерева отсортированных по Source
    LinksArray[0].LeftAsTarget = FirstAsTarget; //Корень дерева отсортированных по Target
    LinksArray[0].RightAsTarget = LastFreeLink; //Индекс последней удаленной связи
    LinksArray[0].SizeAsTarget = ReservedField; //Поле не используется
    Memory.ResizeFile(AllocatedLinks * sizeof(Link));
    Memory.Close();
}



void Links::Create(link_t target) {
    AllocatedLinks++;
    link_t index = AllocatedLinks;
    LinksArray[index].Source = index;
    LinksArray[index].Target = target;
    LinksArray[index].LeftAsSource = 0;
    LinksArray[index].LeftAsTarget = 0;
    LinksArray[index].RightAsSource = 0;
    LinksArray[index].RightAsTarget = 0;
    LinksArray[index].RootAsSource = 0;
    LinksArray[index].RootAsTarget = 0;
    LinksArray[index].SizeAsSource = 0;
    LinksArray[index].SizeAsTarget = 0;
    InsertLinkToSourceTree(GetLinkByIndex(index));
    InsertLinkToTargetTree(GetLinkByIndex(index));
}
void Links::Create(link_t source, link_t target) {
    AllocatedLinks++;
    link_t index = AllocatedLinks;
    LinksArray[index].Source = source;
    LinksArray[index].Target = target;
    LinksArray[index].LeftAsSource = 0;
    LinksArray[index].LeftAsTarget = 0;
    LinksArray[index].RightAsSource = 0;
    LinksArray[index].RightAsTarget = 0;
    LinksArray[index].RootAsSource = 0;
    LinksArray[index].RootAsTarget = 0;
    LinksArray[index].SizeAsSource = 0;
    LinksArray[index].SizeAsTarget = 0;
    InsertLinkToSourceTree(GetLinkByIndex(index));
    InsertLinkToTargetTree(GetLinkByIndex(index));
}

