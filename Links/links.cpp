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
        Init();
    }
}

Link* Links::operator[] (const link_t index) {
    return &LinksArray[index];
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


Link* Links::CreateLink() {
    Link* link;
    if(FreeLinks > 0) {
        link = &LinksArray[LastFreeLink];
        LastFreeLink = LinksArray[LastFreeLink].Source;
    }
    else {
        link = &LinksArray[AllocatedLinks];
        AllocatedLinks++;
    }
    return link;
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
    return link;
}

Link* Links::CreateLink(link_t source, link_t target) {
    Link* link;
    if(FreeLinks > 0) {
        link = &LinksArray[LastFreeLink];
        LastFreeLink = LinksArray[LastFreeLink].Source;
    }
    else {
        link = &LinksArray[AllocatedLinks];
        AllocatedLinks++;
    }
    link->Source = source;
    link->Target = target;
    return link;
}

void Links::UpdateLink (Link* link, link_t source, link_t target ) {
    link->Source = source;
    link->Target = target;
}

void Links::UpdateLink (link_t linkindex, link_t source, link_t target ) {
    LinksArray[linkindex].Source = source;
    LinksArray[linkindex].Target = target;
}


void Links::Delete(link_t index) {
    if(!FreeLinks) {
        FreeLinks++;
        FirstFreeLink = index;
        LastFreeLink = index;
    }
    else {
        FreeLinks++;
        LinksArray[LastFreeLink].Target = index;
        LinksArray[index].Source = LastFreeLink;
        LastFreeLink = index;
    }
}

void Links::Delete(Link* link) {
    if(!FreeLinks) {
        FreeLinks++;
        FirstFreeLink = GetIndexByLink(link);
        LastFreeLink = GetIndexByLink(link);
    }
    else {
        FreeLinks++;
        link_t index = GetIndexByLink(link);
        LinksArray[LastFreeLink].Target = index;
        link->Source = LastFreeLink;
        LastFreeLink = index;
    }
}

void Links::DeleteSequence ( link_t index ) {
    while(1) {
        if(this->LinksArray[index].Source == 0) {
            Delete(index);
            break;
        }
        link_t nextindex = LinksArray[index].Source;
        this->Delete(index);
        index = nextindex;
    }
}


void Links::DeleteSequence(Link* link) {
    while(1) {
        if(link->Source == 0) {
            Delete(link);
            break;
        }
        Link* nextlink = GetLinkByIndex(link->Source);
        this->Delete(link);
        link = nextlink;
    }
}


Link* Links::GetLinkByIndex(link_t index) {
    if(!index) {
        return nullptr;
    }
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

void Links::Init() {
    AllocatedLinks = 1;
    //uint64_t ExpOfTwo = 1;
    for(int i = 0; i < 64; i++) {
        this->Create(0, 0);
        //ExpOfTwo *= 2;
    }
    std::cout << "First Initialization." << std::endl;
}

size_t Links::GetMemoryMapSize() {
    return ReservedLinks * sizeof(Link);
}

size_t Links::GetMemoryUse() {
    return AllocatedLinks * sizeof(Link);
}

void Links::Create() {
    AllocatedLinks++;
}
void Links::Create(link_t target) {
    AllocatedLinks++;
    link_t index = AllocatedLinks;
    LinksArray[index].Source = index;
    LinksArray[index].Target = target;
}
void Links::Create(link_t source, link_t target) {
    AllocatedLinks++;
    link_t index = AllocatedLinks;
    LinksArray[index].Source = source;
    LinksArray[index].Target = target;
}




