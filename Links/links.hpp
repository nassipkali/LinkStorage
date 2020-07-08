#ifndef LINKS_HPP
#define LINKS_HPP
#include "linkdata.hpp"
#include "linkindex.hpp"
#include "linksmemory.hpp"


template <typename T>
class Links
{
    private:
        LinksMemory DataMemory;
        LinksMemory IndexMemory;
        LinkData<T>* LinksDataArray;
        LinkIndex<T>* LinksIndexArray;
        T AllocatedLinks;
        T ReservedLinks;
        T FreeLinks;
        T FirstFreeLink;
        T LastFreeLink;
        size_t BlockSize;
        void LeftRotateBySourceTree(T pNode);
        void RightRotateBySourceTree(T qNode);
        void LeftRotateByTargetTree(T pNode);
        void RightRotateByTargetTree(T qNode);
        void MaintainBySourceTree(T node, bool flag);
        void MaintainByTargetTree(T node, bool flag);
        void InsertLinkToSourceTree(T node);
        void InsertLinkToTargetTree(T node);
        void DeleteLinkFromSourceTree(T node);
        void DeleteLinkFromTargetTree(T node);
    public:
        Links(const char* dataFile, const char* indexFile, size_t blocksize = 1024 * 1024 * 16);
        T CreateLink(T source, T target);
        LinkData<T> GetLinkData(T link);
        LinkIndex<T> GetLinkIndex(T link);
        void UpdateLink(T linkindex, T source, T target);
        void Delete(T index);
        T SearchLinkBySource(T Source, T Target);
        T SearchLinkByTarget(T Source, T Target);
        T GetAllocatedLinksCount();
        T GetReservedLinksCount();
        T GetFreeLinksCount();
        void Close();
};

template <typename T>
Links<T>::Links(const char* dataFile, const char* indexFile, size_t blocksize) {
    LinksDataArray = (LinkData<T>*)DataMemory.Map(dataFile, blocksize);
    LinksIndexArray = (LinkIndex<T>*)IndexMemory.Map(indexFile, blocksize*4);
    // MetaData loads from 0 link
    AllocatedLinks = LinksIndexArray[0].RootAsSource; //Количество используемых связей
    ReservedLinks = LinksIndexArray[0].LeftAsSource; //Количество выделенных связей
    FreeLinks = LinksIndexArray[0].RightAsSource; // Количество неиспользуемых/удаленных связей
    FirstFreeLink = LinksIndexArray[0].SizeAsSource; //Индекс первой удаленной связи
    LastFreeLink = LinksIndexArray[0].RootAsTarget; //Индекс последней удаленной связи

    ReservedLinks = this->DataMemory.MapSize / sizeof(LinkData<T>);
    if(AllocatedLinks == 0) {
        AllocatedLinks = 1;
    }
}



template <typename T>
T Links<T>::CreateLink(T source, T target) {
    T index;
    if(FreeLinks > 0) {
        index = LastFreeLink;
        LastFreeLink = LinksDataArray[LastFreeLink].Source;
        FreeLinks--;
    }
    else {
        index = AllocatedLinks;
        AllocatedLinks++;
    }
    LinksDataArray[index].Source = source;
    LinksDataArray[index].Target = target;
    LinksIndexArray[index].RootAsSource = 0;
    LinksIndexArray[index].LeftAsSource = 0;
    LinksIndexArray[index].RightAsSource = 0;
    LinksIndexArray[index].SizeAsSource = 0;
    LinksIndexArray[index].RootAsTarget = 0;
    LinksIndexArray[index].LeftAsTarget = 0;
    LinksIndexArray[index].RightAsTarget = 0;
    LinksIndexArray[index].SizeAsTarget = 0;
    //InsertLinkToSourceTree(link);
    //InsertLinkToTargetTree(link);
    return index;
}

template <typename T>
LinkData<T> Links<T>::GetLinkData(T link) {
    return LinksDataArray[link];
}

template <typename T>
LinkIndex<T> Links<T>::GetLinkIndex(T link) {
    return LinksIndexArray[link];
}


template <typename T>
void Links<T>::UpdateLink (T link, T source, T target ) {
    LinksDataArray[link].Source = source;
    LinksDataArray[link].Target = target;
}


template <typename T>
void Links<T>::Delete(T index) {
    if(FreeLinks) {
        FreeLinks++;
        LinksDataArray[LastFreeLink].Target = index;
        LinksDataArray[index].Source = LastFreeLink;
        LastFreeLink = index;
    }
    else {
        FreeLinks++;
        FirstFreeLink = index;
        LastFreeLink = index;
    }
}





/* Here should be code of search methods  */

template <typename T>
void Links<T>::LeftRotateBySourceTree(T pNode) {
    LinkIndex<T> *pNodePtr = &LinksIndexArray[pNode];
    LinkIndex<T> *qNodePtr = &LinksIndexArray[pNodePtr->RightAsSource];
    pNodePtr->RightAsSource = qNodePtr->LeftAsSource;
    qNodePtr->LeftAsSource = pNode;
    qNodePtr->SizeAsSource = pNodePtr->SizeAsSource;
    pNodePtr->SizeAsSource = LinksIndexArray[pNodePtr->LeftAsSource].SizeAsSource + LinksIndexArray[pNodePtr->RightAsSource].SizeAsSource + 1;
    LinkIndex<T> temp = *pNodePtr;
    *pNodePtr = *qNodePtr;
    *qNodePtr = temp;
}

template <typename T>
void Links<T>::RightRotateBySourceTree(T qNode) {
    LinkIndex<T> *qNodePtr = &LinksIndexArray[qNode];
    LinkIndex<T> *pNodePtr = &LinksIndexArray[qNodePtr->LeftAsSource];
    qNodePtr->LeftAsSource = pNodePtr->RightAsSource;
    pNodePtr->RightAsSource = qNode;
    pNodePtr->SizeAsSource = qNodePtr->SizeAsSource;
    qNodePtr->SizeAsSource = LinksIndexArray[qNodePtr->LeftAsSource].SizeAsSource + LinksIndexArray[pNodePtr->RightAsSource].SizeAsSource + 1;
    LinkIndex<T> temp = *qNodePtr;
    *qNodePtr = *pNodePtr;
    *pNodePtr = temp;
}

template <typename T>
void Links<T>::LeftRotateByTargetTree(T pNode) {
    LinkIndex<T> *pNodePtr = &LinksIndexArray[pNode];
    LinkIndex<T> *qNodePtr = &LinksIndexArray[pNodePtr->RightAsTarget];
    pNodePtr->RightAsTarget = qNodePtr->LeftAsTarget;
    qNodePtr->LeftAsTarget = pNode;
    qNodePtr->SizeAsTarget = pNodePtr->SizeAsTarget;
    pNodePtr->SizeAsTarget = LinksIndexArray[pNodePtr->LeftAsTarget].SizeAsTarget + LinksIndexArray[pNodePtr->RightAsTarget].SizeAsTarget + 1;
    LinkIndex<T> temp = *pNodePtr;
    *pNodePtr = *qNodePtr;
    *qNodePtr = temp;
}

template <typename T>
void Links<T>::RightRotateByTargetTree(T qNode) {
    LinkIndex<T> *qNodePtr = &LinksIndexArray[qNode];
    LinkIndex<T> *pNodePtr = &LinksIndexArray[qNodePtr->LeftAsTarget];
    qNodePtr->LeftAsTarget = pNodePtr->RightAsTarget;
    pNodePtr->RightAsTarget = qNode;
    pNodePtr->SizeAsTarget = qNodePtr->SizeAsTarget;
    qNodePtr->SizeAsTarget = LinksIndexArray[qNodePtr->LeftAsTarget].SizeAsTarget + LinksIndexArray[pNodePtr->RightAsTarget].SizeAsTarget + 1;
    LinkIndex<T> temp = *qNodePtr;
    *qNodePtr = *pNodePtr;
    *pNodePtr = temp;
}

template <typename T>
void Links<T>::MaintainBySourceTree(T node, bool flag) {
    LinkIndex<T>* nodePtr = &LinksIndexArray[node];
    LinkIndex<T>* nodeLeftPtr = &LinksIndexArray[nodePtr->LeftAsSource];
    LinkIndex<T>* nodeRightPtr = &LinksIndexArray[nodePtr->RightAsSource];
    if(flag) {
        if(nodeLeftPtr->SizeAsSource < LinksIndexArray[nodeRightPtr->LeftAsSource].SizeAsSource) {
            //case 1
            RightRotateBySourceTree(nodePtr->RightAsSource);
            LeftRotateBySourceTree(node);
        }
        else if(nodeLeftPtr->SizeAsSource < LinksIndexArray[nodeRightPtr->RightAsSource].SizeAsSource) {
            //case 2
            LeftRotateBySourceTree(node);
        }
        else {
            return;
        }
    }
    else {
        if(nodeRightPtr->SizeAsSource < LinksIndexArray[nodeLeftPtr->RightAsSource].SizeAsSource) {
            //case 1'
            LeftRotateBySourceTree(nodePtr->LeftAsSource);
            RightRotateBySourceTree(node);
        }
        else if(nodeRightPtr->SizeAsSource < LinksIndexArray[nodeLeftPtr->LeftAsSource].SizeAsSource) {
            RightRotateBySourceTree(node);
        }
        else {
            return;
        }
    }
    MaintainBySourceTree(nodePtr->LeftAsSource, false);
    MaintainBySourceTree(nodePtr->RightAsSource, true);
    MaintainBySourceTree(node, true);
    MaintainBySourceTree(node, false);
}

template <typename T>
void Links<T>::MaintainByTargetTree(T node, bool flag) {
    LinkIndex<T>* nodePtr = &LinksIndexArray[node];
    LinkIndex<T>* nodeLeftPtr = &LinksIndexArray[nodePtr->LeftAsTarget];
    LinkIndex<T>* nodeRightPtr = &LinksIndexArray[nodePtr->RightAsTarget];
    if(flag) {
        if(nodeLeftPtr->SizeAsTarget < LinksIndexArray[nodeRightPtr->LeftAsTarget].SizeAsTarget) {
            //case 1
            RightRotateByTargetTree(nodePtr->RightAsTarget);
            LeftRotateByTargetTree(node);
        }
        else if(nodeLeftPtr->SizeAsTarget < LinksIndexArray[nodeRightPtr->RightAsTarget].SizeAsTarget) {
            //case 2
            LeftRotateByTargetTree(node);
        }
        else {
            return;
        }
    }
    else {
        if(nodeRightPtr->SizeAsTarget < LinksIndexArray[nodeLeftPtr->RightAsTarget].SizeAsTarget) {
            //case 1'
            LeftRotateByTargetTree(nodePtr->LeftAsTarget);
            RightRotateByTargetTree(node);
        }
        else if(nodeRightPtr->SizeAsTarget < LinksIndexArray[nodeLeftPtr->LeftAsTarget].SizeAsTarget) {
            RightRotateByTargetTree(node);
        }
        else {
            return;
        }
    }
    MaintainByTargetTree(nodePtr->LeftAsTarget, false);
    MaintainByTargetTree(nodePtr->RightAsTarget, true);
    MaintainByTargetTree(node, true);
    MaintainByTargetTree(node, false);
}

/*END*/

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
    AllocatedLinks = LinksIndexArray[0].RootAsSource = AllocatedLinks; //Количество используемых связей
    ReservedLinks = LinksIndexArray[0].LeftAsSource = ReservedLinks; //Количество выделенных связей
    FreeLinks = LinksIndexArray[0].RightAsSource = FreeLinks; // Количество неиспользуемых/удаленных связей
    FirstFreeLink = LinksIndexArray[0].SizeAsSource = FirstFreeLink; //Индекс первой удаленной связи
    LastFreeLink = LinksIndexArray[0].RootAsTarget = LastFreeLink; //Индекс последней удаленной связи
    DataMemory.ResizeFile(AllocatedLinks * sizeof(LinkData<T>));
    DataMemory.Close();
    IndexMemory.ResizeFile(AllocatedLinks * sizeof(LinkIndex<T>));
    IndexMemory.Close();
}



#endif // LINKS_HPP
