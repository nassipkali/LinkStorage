#ifndef LINKS_HPP
#define LINKS_HPP
#include "linkdata.hpp"
#include "linkindex.hpp"
#include "linksmemory.hpp"
#include <stack>
#include <iostream>
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
        void LeftRotateBySourceTree(T rootNode, T pNode);
        void RightRotateBySourceTree(T rootNode, T qNode);
        void LeftRotateByTargetTree(T rootNode, T pNode);
        void RightRotateByTargetTree(T rootNode, T qNode);
        void MaintainBySourceTree(T rootNode, T node, bool flag);
        void MaintainByTargetTree(T rootNode, T node, bool flag);
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
        bool Delete(T index);
        bool CascadeDelete(T index);
        T SearchLinkBySource(T source, T target);
        T SearchLinkByTarget(T source, T target);
        T SearchLink(T source, T target);
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
    if(source == 0 || target == 0) {
        std::cerr << "[LinksPlatform] Can't create link with reference to zero" << std::endl;
        return 0;
    }
    T link;
    if(FreeLinks > 0) {
        link = LastFreeLink;
        LastFreeLink = LinksDataArray[LastFreeLink].Source;
        FreeLinks--;
    }
    else {
        link = AllocatedLinks;
        AllocatedLinks++;
    }
    LinksDataArray[link].Source = source;
    LinksDataArray[link].Target = target;
    LinksIndexArray[link].RootAsSource = 0;
    LinksIndexArray[link].LeftAsSource = 0;
    LinksIndexArray[link].RightAsSource = 0;
    LinksIndexArray[link].SizeAsSource = 1;
    LinksIndexArray[link].RootAsTarget = 0;
    LinksIndexArray[link].LeftAsTarget = 0;
    LinksIndexArray[link].RightAsTarget = 0;
    LinksIndexArray[link].SizeAsTarget = 1;
    InsertLinkToSourceTree(link);
    InsertLinkToTargetTree(link);
    return link;
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
bool Links<T>::Delete(T link) {
    LinkIndex<T>* index = &LinksIndexArray[link];
    T rootAsSource = index->RootAsSource;
    T rootAsTarget = index->RootAsTarget;
    if(rootAsSource == 0 &&
       (rootAsTarget == 0 ||
        (LinksIndexArray[rootAsTarget].RootAsTarget == link && LinksIndexArray[rootAsTarget].SizeAsTarget == 1))) {
        if(FreeLinks) {
            FreeLinks++;
            LinksDataArray[LastFreeLink].Target = link;
            LinksDataArray[link].Source = LastFreeLink;
            LastFreeLink = link;
            return true;
        }
        else {
            FreeLinks++;
            FirstFreeLink = link;
            LastFreeLink = link;
            return true;
        }
    }
    else if(rootAsTarget == 0 &&
            (rootAsSource == 0 ||
             (LinksIndexArray[rootAsSource].RootAsSource == link && LinksIndexArray[rootAsSource].SizeAsSource == 1))) {
        if(FreeLinks) {
            FreeLinks++;
            LinksDataArray[LastFreeLink].Target = link;
            LinksDataArray[link].Source = LastFreeLink;
            LastFreeLink = link;
            return true;
        }
        else {
            FreeLinks++;
            FirstFreeLink = link;
            LastFreeLink = link;
            return true;
        }
    }
    else {
        return false;
    }
}






/* Here should be code of search methods  */

template <typename T>
void Links<T>::LeftRotateBySourceTree(T rootNode, T pNode) {
    T qNode = LinksIndexArray[pNode].RightAsSource;
    LinkIndex<T> *pNodePtr = &LinksIndexArray[pNode];
    LinkIndex<T> *qNodePtr = &LinksIndexArray[qNode];
    pNodePtr->RightAsSource = qNodePtr->LeftAsSource;
    qNodePtr->LeftAsSource = pNode;
    qNodePtr->SizeAsSource = pNodePtr->SizeAsSource;
    pNodePtr->SizeAsSource = LinksIndexArray[pNodePtr->LeftAsSource].SizeAsSource + LinksIndexArray[pNodePtr->RightAsSource].SizeAsSource + 1;
    if(!rootNode) {
        return;
    }
    if(LinksIndexArray[rootNode].RightAsSource == pNode) {
        LinksIndexArray[rootNode].RightAsSource = qNode;
    }
    else {
        LinksIndexArray[rootNode].LeftAsSource = qNode;
    }
}

template <typename T>
void Links<T>::RightRotateBySourceTree(T rootNode, T qNode) {
    T pNode = LinksIndexArray[qNode].LeftAsSource;
    LinkIndex<T> *qNodePtr = &LinksIndexArray[qNode];
    LinkIndex<T> *pNodePtr = &LinksIndexArray[pNode];
    qNodePtr->LeftAsSource = pNodePtr->RightAsSource;
    pNodePtr->RightAsSource = qNode;
    pNodePtr->SizeAsSource = qNodePtr->SizeAsSource;
    qNodePtr->SizeAsSource = LinksIndexArray[qNodePtr->LeftAsSource].SizeAsSource + LinksIndexArray[pNodePtr->RightAsSource].SizeAsSource + 1;
    if(!rootNode) {
        return;
    }
    if(LinksIndexArray[rootNode].RightAsSource == pNode) {
        LinksIndexArray[rootNode].RightAsSource = qNode;
    }
    else {
        LinksIndexArray[rootNode].LeftAsSource = qNode;
    }
}

template <typename T>
void Links<T>::LeftRotateByTargetTree(T rootNode, T pNode) {
    T qNode = LinksIndexArray[pNode].RightAsTarget;
    LinkIndex<T> *pNodePtr = &LinksIndexArray[pNode];
    LinkIndex<T> *qNodePtr = &LinksIndexArray[qNode];
    pNodePtr->RightAsTarget = qNodePtr->LeftAsTarget;
    qNodePtr->LeftAsTarget = pNode;
    qNodePtr->SizeAsTarget = pNodePtr->SizeAsTarget;
    pNodePtr->SizeAsTarget = LinksIndexArray[pNodePtr->LeftAsTarget].SizeAsTarget + LinksIndexArray[pNodePtr->RightAsTarget].SizeAsTarget + 1;
    if(!rootNode) {
        return;
    }
    if(LinksIndexArray[rootNode].RightAsTarget == pNode) {
        LinksIndexArray[rootNode].RightAsTarget = qNode;
    }
    else {
        LinksIndexArray[rootNode].LeftAsTarget = qNode;
    }
}

template <typename T>
void Links<T>::RightRotateByTargetTree(T rootNode, T qNode) {
    T pNode = LinksIndexArray[qNode].LeftAsTarget;
    LinkIndex<T> *qNodePtr = &LinksIndexArray[qNode];
    LinkIndex<T> *pNodePtr = &LinksIndexArray[pNode];
    qNodePtr->LeftAsTarget = pNodePtr->RightAsTarget;
    pNodePtr->RightAsTarget = qNode;
    pNodePtr->SizeAsTarget = qNodePtr->SizeAsTarget;
    qNodePtr->SizeAsTarget = LinksIndexArray[qNodePtr->LeftAsTarget].SizeAsTarget + LinksIndexArray[pNodePtr->RightAsTarget].SizeAsTarget + 1;
    if(!rootNode) {
        return;
    }
    if(LinksIndexArray[rootNode].RightAsTarget == pNode) {
        LinksIndexArray[rootNode].RightAsTarget = qNode;
    }
    else {
        LinksIndexArray[rootNode].LeftAsTarget = qNode;
    }
}

template <typename T>
void Links<T>::MaintainBySourceTree(T rootNode, T node, bool flag) {
    LinkIndex<T>* nodePtr = &LinksIndexArray[node];
    LinkIndex<T>* nodeLeftPtr = &LinksIndexArray[nodePtr->LeftAsSource];
    LinkIndex<T>* nodeRightPtr = &LinksIndexArray[nodePtr->RightAsSource];
    if(flag) {
        if(nodeLeftPtr->SizeAsSource < LinksIndexArray[nodeRightPtr->LeftAsSource].SizeAsSource) {
            //case 1
            RightRotateBySourceTree(node, nodePtr->RightAsSource);
            LeftRotateBySourceTree(rootNode, node);
        }
        else if(nodeLeftPtr->SizeAsSource < LinksIndexArray[nodeRightPtr->RightAsSource].SizeAsSource) {
            //case 2
            LeftRotateBySourceTree(rootNode, node);
        }
        else {
            return;
        }
    }
    else {
        if(nodeRightPtr->SizeAsSource < LinksIndexArray[nodeLeftPtr->RightAsSource].SizeAsSource) {
            //case 1'
            LeftRotateBySourceTree(node, nodePtr->LeftAsSource);
            RightRotateBySourceTree(rootNode, node);
        }
        else if(nodeRightPtr->SizeAsSource < LinksIndexArray[nodeLeftPtr->LeftAsSource].SizeAsSource) {
            RightRotateBySourceTree(rootNode, node);
        }
        else {
            return;
        }
    }
    MaintainBySourceTree(node, nodePtr->LeftAsSource, false);
    MaintainBySourceTree(node, nodePtr->RightAsSource, true);
    MaintainBySourceTree(rootNode, node, true);
    MaintainBySourceTree(rootNode, node, false);
}

template <typename T>
void Links<T>::MaintainByTargetTree(T rootNode, T node, bool flag) {
    LinkIndex<T>* nodePtr = &LinksIndexArray[node];
    LinkIndex<T>* nodeLeftPtr = &LinksIndexArray[nodePtr->LeftAsTarget];
    LinkIndex<T>* nodeRightPtr = &LinksIndexArray[nodePtr->RightAsTarget];
    if(flag) {
        if(nodeLeftPtr->SizeAsTarget < LinksIndexArray[nodeRightPtr->LeftAsTarget].SizeAsTarget) {
            //case 1
            RightRotateByTargetTree(node, nodePtr->RightAsTarget);
            LeftRotateByTargetTree(rootNode, node);
        }
        else if(nodeLeftPtr->SizeAsTarget < LinksIndexArray[nodeRightPtr->RightAsTarget].SizeAsTarget) {
            //case 2
            LeftRotateByTargetTree(rootNode, node);
        }
        else {
            return;
        }
    }
    else {
        if(nodeRightPtr->SizeAsTarget < LinksIndexArray[nodeLeftPtr->RightAsTarget].SizeAsTarget) {
            //case 1'
            LeftRotateByTargetTree(node, nodePtr->LeftAsTarget);
            RightRotateByTargetTree(rootNode, node);
        }
        else if(nodeRightPtr->SizeAsTarget < LinksIndexArray[nodeLeftPtr->LeftAsTarget].SizeAsTarget) {
            RightRotateByTargetTree(rootNode, node);
        }
        else {
            return;
        }
    }
    MaintainByTargetTree(node, nodePtr->LeftAsTarget, false);
    MaintainByTargetTree(node, nodePtr->RightAsTarget, true);
    MaintainByTargetTree(rootNode, node, true);
    MaintainByTargetTree(rootNode, node, false);
}

template <typename T>
void Links<T>::InsertLinkToSourceTree(T node) {
    T source = LinksDataArray[node].Source;
    T target = LinksDataArray[node].Target;
    T root = LinksIndexArray[source].RootAsSource;
    LinkIndex<T>* sourceIndexPtr = &LinksIndexArray[source];
    LinkIndex<T>* targetIndexPtr = &LinksIndexArray[target];
    if(!root) {
        sourceIndexPtr->RootAsSource = node;
        sourceIndexPtr->SizeAsSource = 1;
        return;
    }
    T rootLeft = LinksIndexArray[root].LeftAsSource;
    T rootRight = LinksIndexArray[root].RightAsSource;
    T currentLink = root;
    std::stack<T> nodes;
    while(true) {
        if(LinksDataArray[currentLink].Target > target) {
            if(LinksIndexArray[currentLink].LeftAsSource != 0) {
                LinksIndexArray[currentLink].SizeAsSource++;
                nodes.push(currentLink);
                currentLink = LinksIndexArray[currentLink].LeftAsSource;
                continue;
            }
            else {
                LinksIndexArray[currentLink].SizeAsSource++;
                nodes.push(currentLink);
                LinksIndexArray[currentLink].LeftAsSource = node;
                break;
            }
        }
        else {
            if(LinksIndexArray[currentLink].RightAsSource != 0) {
                LinksIndexArray[currentLink].SizeAsSource++;
                nodes.push(currentLink);
                currentLink = LinksIndexArray[currentLink].RightAsSource;
                continue;
            }
            else {
                LinksIndexArray[currentLink].SizeAsSource++;
                nodes.push(currentLink);
                LinksIndexArray[currentLink].RightAsSource = node;
                break;
            }
        }
    }
    while(nodes.size() > 2) {
        T lNode = nodes.top();
        nodes.pop();
        T hNode = nodes.top();
        nodes.pop();
        T rNode = nodes.top();
        nodes.push(hNode);
        MaintainBySourceTree(rNode, hNode, LinksDataArray[lNode].Target > LinksDataArray[hNode].Target);
    }
    if(nodes.size() == 2) {
        T lNode = nodes.top();
        nodes.pop();
        T hNode = nodes.top();
        nodes.pop();
        MaintainBySourceTree(0, hNode, LinksDataArray[lNode].Target > LinksDataArray[hNode].Target);
        if(LinksIndexArray[rootLeft].RightAsSource == root) {
            LinksIndexArray[source].RootAsSource = rootLeft;
        }
        else if(LinksIndexArray[rootRight].LeftAsSource == root){
            LinksIndexArray[source].RootAsSource = rootRight;
        }
    }
}

template <typename T>
void Links<T>::InsertLinkToTargetTree(T node) {
    T source = LinksDataArray[node].Source;
    T target = LinksDataArray[node].Target;
    T root = LinksIndexArray[target].RootAsTarget;
    LinkIndex<T>* sourceIndexPtr = &LinksIndexArray[source];
    LinkIndex<T>* targetIndexPtr = &LinksIndexArray[target];
    if(!root) {
        targetIndexPtr->RootAsTarget = node;
        targetIndexPtr->SizeAsTarget = 1;
        return;
    }
    T rootLeft = LinksIndexArray[root].LeftAsSource;
    T rootRight = LinksIndexArray[root].RightAsSource;
    T currentLink = root;
    std::stack<T> nodes;
    while(true) {
        if(LinksDataArray[currentLink].Source > source) {
            if(LinksIndexArray[currentLink].LeftAsTarget != 0) {
                LinksIndexArray[currentLink].SizeAsTarget++;
                nodes.push(currentLink);
                currentLink = LinksIndexArray[currentLink].LeftAsTarget;
                continue;
            }
            else {
                LinksIndexArray[currentLink].SizeAsTarget++;
                nodes.push(currentLink);
                LinksIndexArray[currentLink].LeftAsTarget = node;
                break;
            }
        }
        else {
            if(LinksIndexArray[currentLink].RightAsTarget != 0) {
                LinksIndexArray[currentLink].SizeAsTarget++;
                nodes.push(currentLink);
                currentLink = LinksIndexArray[currentLink].RightAsTarget;
                continue;
            }
            else {
                LinksIndexArray[currentLink].SizeAsTarget++;
                nodes.push(currentLink);
                LinksIndexArray[currentLink].RightAsTarget = node;
                break;
            }
        }
    }
    while(nodes.size() > 2) {
        T lNode = nodes.top();
        nodes.pop();
        T hNode = nodes.top();
        nodes.pop();
        T rNode = nodes.top();
        nodes.push(hNode);
        MaintainByTargetTree(rNode, hNode, LinksDataArray[lNode].Source > LinksDataArray[hNode].Source);
    }
    if(nodes.size() == 2) {
        T lNode = nodes.top();
        nodes.pop();
        T hNode = nodes.top();
        nodes.pop();
        MaintainByTargetTree(0, hNode, LinksDataArray[lNode].Source > LinksDataArray[hNode].Source);
        if(LinksIndexArray[rootLeft].RightAsTarget == root) {
            LinksIndexArray[target].RootAsTarget = rootLeft;
        }
        else if(LinksIndexArray[rootRight].LeftAsTarget == root){
            LinksIndexArray[target].RootAsTarget = rootRight;
        }
    }
}

template <typename T>
void Links<T>::DeleteLinkFromSourceTree(T node) {
    T source = LinksDataArray[node].Source;
    T target = LinksDataArray[node].Target;
    T currentLink = LinksIndexArray[source].RootAsSource;
    T rootOfDelete;
    T previousLink;
    bool leftOrRight;
    while(true) {
        if(LinksDataArray[currentLink].Target > target) {
            LinksIndexArray[currentLink].SizeAsSource--;
            leftOrRight = false; // false - left
            previousLink = currentLink;
            currentLink = LinksIndexArray[currentLink].LeftAsSource;
            continue;
        }
        else if(LinksDataArray[currentLink].Target < target) {
            LinksIndexArray[currentLink].SizeAsSource--;
            leftOrRight = true; // true - right
            previousLink = currentLink;
            currentLink = LinksIndexArray[currentLink].RightAsSource;
            continue;
        }
        else {
            rootOfDelete = previousLink;
            break;
        }
    }
    if(LinksIndexArray[currentLink].LeftAsSource == 0 && LinksIndexArray[currentLink].RightAsSource == 0) {
        if(leftOrRight == false) {
            LinksIndexArray[rootOfDelete].LeftAsSource = 0;
        }
        else {
            LinksIndexArray[rootOfDelete].RightAsSource = 0;
        }
    }
    else if(LinksIndexArray[currentLink].LeftAsSource == 0 && LinksIndexArray[currentLink].RightAsSource != 0) {
        if(leftOrRight == false) {
            LinksIndexArray[rootOfDelete].LeftAsSource = LinksIndexArray[currentLink].RightAsSource;;
        }
        else {
            LinksIndexArray[rootOfDelete].RightAsSource = LinksIndexArray[currentLink].RightAsSource;
        }
    }
    else if(LinksIndexArray[currentLink].LeftAsSource != 0 && LinksIndexArray[currentLink].RightAsSource == 0) {
        if(leftOrRight == false) {
            LinksIndexArray[rootOfDelete].LeftAsSource = LinksIndexArray[currentLink].LeftAsSource;;
        }
        else {
            LinksIndexArray[rootOfDelete].RightAsSource = LinksIndexArray[currentLink].LeftAsSource;
        }
    }
    else {
       
    }
}

template <typename T>
T Links<T>::SearchLinkBySource(T source, T target) {
    T currentLink = LinksIndexArray[source].RootAsSource;
    while(true) {
        if(LinksDataArray[currentLink].Target > target) {
            if(LinksIndexArray[currentLink].LeftAsSource != 0) {
                currentLink = LinksIndexArray[currentLink].LeftAsSource;
                continue;
            }
            else {
                return 0;
            }
        }
        else if(LinksDataArray[currentLink].Target < target) {
            if(LinksIndexArray[currentLink].RightAsSource != 0) {
                currentLink = LinksIndexArray[currentLink].RightAsSource;
                continue;
            }
            else {
                return 0;
            }
        }
        else {
            return currentLink;
        }
    }
}

template <typename T>
T Links<T>::SearchLinkByTarget(T source, T target) {
    T currentLink = LinksIndexArray[target].RootAsTarget;
    while(true) {
        if(LinksDataArray[currentLink].Source > source) {
            if(LinksIndexArray[currentLink].LeftAsTarget != 0) {
                currentLink = LinksIndexArray[currentLink].LeftAsTarget;
                continue;
            }
            else {
                return 0;
            }
        }
        else if(LinksDataArray[currentLink].Source < source) {
            if(LinksIndexArray[currentLink].RightAsTarget != 0) {
                currentLink = LinksIndexArray[currentLink].RightAsTarget;
                continue;
            }
            else {
                return 0;
            }
        }
        else {
            return currentLink;
        }
    }
}

template <typename T>
T Links<T>::SearchLink(T source, T target) {
    T rootAsSource = LinksIndexArray[source].RootAsSource;
    T sizeAsSource = LinksIndexArray[rootAsSource].SizeAsSource;
    T rootAsTarget = LinksIndexArray[target].RootAsTarget;
    T sizeAsTarget = LinksIndexArray[rootAsTarget].SizeAsTarget;
    if(sizeAsSource <= sizeAsTarget) {
        return SearchLinkBySource(source, target);
    }
    else {
        return SearchLinkByTarget(source, target);
    }
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
