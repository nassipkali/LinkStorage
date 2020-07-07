#ifndef LINKS_HPP
#define LINKS_HPP
#include "link.hpp"
#include "linksmemory.hpp"


template <typename T>
class Links
{
    private:
        LinksMemory Memory;
        Link<T>* LinksArray;
        T AllocatedLinks;
        T ReservedLinks;
        T FreeLinks;
        T FirstFreeLink;
        T FirstAsSource;
        T FirstAsTarget;
        T LastFreeLink;
        T ReservedField;
        size_t BlockSize;
        Link<T>* InsertLinkToSourceTree(Link<T>* node);
        Link<T>* InsertLinkToTargetTree(Link<T>* node);
        void DeleteLinkFromSourceTree(Link<T>* node);
        void DeleteLinkFromTargetTree(Link<T>* node);
    public:
        Links(const char* dbname, size_t blocksize = 1024 * 1024 * 16);
        Link<T>* operator[] (const T index);
        Link<T>* CreateLink(T source, T target);
        void UpdateLink(Link<T>* link, T source, T target);
        void UpdateLink(T linkindex, T source, T target);
        void Delete(T index);
        void Delete(Link<T>* link);
        Link<T>* GetLinkByIndex(T index);
        T GetIndexByLink(Link<T>* link);
        Link<T>* SearchLinkBySource(T Source, T Target);
        Link<T>* SearchLinkByTarget(T Source, T Target);
        T GetAllocatedLinksCount();
        T GetReservedLinksCount();
        T GetFreeLinksCount();
        void Close();
};

template <typename T>
Links<T>::Links(const char* dbname, size_t blocksize) {
    LinksArray = (Link<T>*)Memory.Map(dbname, blocksize);
    // MetaData loads from 0 link
    AllocatedLinks = LinksArray[0].Source; //Количество используемых связей
    ReservedLinks = LinksArray[0].Target; //Количество выделенных связей
    FreeLinks = LinksArray[0].LeftAsSource; // Количество неиспользуемых/удаленных связей
    FirstFreeLink = LinksArray[0].RightAsSource; //Индекс первой удаленной связи
    FirstAsSource = LinksArray[0].SizeAsSource; //Корень дерева отсортированных по Source
    FirstAsTarget = LinksArray[0].LeftAsTarget; //Корень дерева отсортированных по Target
    LastFreeLink = LinksArray[0].RightAsTarget; //Индекс последней удаленной связи
    ReservedField = LinksArray[0].SizeAsTarget; //Поле не используется

    ReservedLinks = this->Memory.MapSize / sizeof(Link<T>);
    if(AllocatedLinks == 0) {
        AllocatedLinks = 1;
    }
}


template <typename T>
Link<T>* Links<T>::operator[] (const T index) {
    return &LinksArray[index];
}

template <typename T>
Link<T>* Links<T>::CreateLink(T source, T target) {
    Link<T>* link;
    if(FreeLinks > 0) {
        link = &LinksArray[LastFreeLink];
        LastFreeLink = LinksArray[LastFreeLink].Source;
        FreeLinks--;
    }
    else {
        link = &LinksArray[AllocatedLinks];
        AllocatedLinks++;
    }
    link->Source = source;
    link->Target = target;
    link->LeftAsSource = 0;
    link->LeftAsTarget = 0;
    link->RightAsSource = 0;
    link->RightAsTarget = 0;
    link->RootAsSource = 0;
    link->RootAsTarget = 0;
    link->SizeAsSource = 0;
    link->SizeAsTarget = 0;
    //InsertLinkToSourceTree(link);
    //InsertLinkToTargetTree(link);
    return link;
}


template <typename T>
void Links<T>::UpdateLink (Link<T>* link, T source, T target ) {
    link->Source = source;
    link->Target = target;
}

template <typename T>
void Links<T>::UpdateLink (T linkindex, T source, T target ) {
    LinksArray[linkindex].Source = source;
    LinksArray[linkindex].Target = target;
}

template <typename T>
void Links<T>::Delete(T index) {
    if(FreeLinks) {
        FreeLinks++;
        LinksArray[LastFreeLink].Target = index;
        LinksArray[index].Source = LastFreeLink;
        LastFreeLink = index;
    }
    else {
        FreeLinks++;
        FirstFreeLink = index;
        LastFreeLink = index;
    }
}

template <typename T>
void Links<T>::Delete(Link<T>* link) {
    if(FreeLinks) {
        FreeLinks++;
        T index = GetIndexByLink(link);
        LinksArray[LastFreeLink].Target = index;
        link->Source = LastFreeLink;
        LastFreeLink = index;
    }
    else {
        FreeLinks++;
        FirstFreeLink = GetIndexByLink(link);
        LastFreeLink = GetIndexByLink(link);
    }
}

template <typename T>
Link<T>* Links<T>::GetLinkByIndex(T index) {
    return &LinksArray[index];
}

template <typename T>
T Links<T>::GetIndexByLink(Link<T>* link) {
    return (link - LinksArray);
}


/* Here should be code of search methods  */

template <typename T>
T Links<T>::GetAllocatedLinksCount() {
    return AllocatedLinks;
}

template <typename T>
T Links<T>::GetReservedLinksCount() {
    return ReservedLinks;
}

template <typename T>
T Links<T>::GetFreeLinksCount() {
    return FreeLinks;
}


template <typename T>
void Links<T>::Close() {
    LinksArray[0].Source = AllocatedLinks; //Количество используемых связей
    LinksArray[0].Target = ReservedField; //Количество выделенных связей
    LinksArray[0].LeftAsSource = FreeLinks; // Количество неиспользуемых/удаленных связей
    LinksArray[0].RightAsSource = FirstFreeLink; //Индекс первой удаленной связи
    LinksArray[0].SizeAsSource = FirstAsSource; //Корень дерева отсортированных по Source
    LinksArray[0].LeftAsTarget = FirstAsTarget; //Корень дерева отсортированных по Target
    LinksArray[0].RightAsTarget = LastFreeLink; //Индекс последней удаленной связи
    LinksArray[0].SizeAsTarget = ReservedField; //Поле не используется
    Memory.ResizeFile(AllocatedLinks * sizeof(Link<T>));
    Memory.Close();
}



#endif // LINKS_HPP
