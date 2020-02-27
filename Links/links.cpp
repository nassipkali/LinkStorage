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
    InsertLinkBySource(GetLinkByIndex(index));
    InsertLinkByTarget(GetLinkByIndex(index));
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
    InsertLinkBySource(GetLinkByIndex(index));
    InsertLinkByTarget(GetLinkByIndex(index));
}

link_t Links::BFactorBySource(Link* node) {
    return GetLinkByIndex(node->RightAsSource)->SizeAsSource - GetLinkByIndex(node->LeftAsSource)->SizeAsSource;
}

void Links::FixSizeBySource(Link* node) {
    node->SizeAsSource = 0;
    if(node->LeftAsSource) {
        node->SizeAsSource = GetLinkByIndex(node->LeftAsSource)->SizeAsSource; 
    }
    if(node->RightAsSource) {
        node->SizeAsSource += GetLinkByIndex(node->RightAsSource)->SizeAsSource;
    }
}

Link* Links::LeftRotateBySource(Link* mNode) {
    Link* sNode = GetLinkByIndex(mNode->RightAsSource);
    mNode->RightAsSource = sNode->LeftAsSource; 
    sNode->LeftAsSource = GetIndexByLink(mNode);
    FixSizeBySource(mNode);
    FixSizeBySource(sNode);
    return sNode;
}

Link* Links::RightRotateBySource(Link* mNode) {
    Link* sNode = GetLinkByIndex(mNode->RightAsSource);
    mNode->LeftAsSource = sNode->RightAsSource;
    sNode->LeftAsSource = GetIndexByLink(mNode);
    FixSizeBySource(mNode);
    FixSizeBySource(sNode);
    return sNode;
}


Link* Links::BalanceBySource(Link* node) {
    FixSizeBySource(node);
    if(BFactorBySource(node) == 2) {
        if(BFactorBySource(GetLinkByIndex(node->RightAsSource)) < 0) {
            node->RightAsSource = GetIndexByLink(
                    RightRotateBySource(
                        GetLinkByIndex(node->RightAsSource)));
        }
        return LeftRotateBySource(node);
    }
    if(BFactorBySource(node) == -2) {
        if(BFactorBySource(GetLinkByIndex(node->LeftAsSource)) > 0) {
            node->LeftAsSource = GetIndexByLink(
                    RightRotateBySource(
                        GetLinkByIndex(node->LeftAsSource)));
        }
        return RightRotateBySource(node);
    }
    return node;
}

Link* Links::InsertLinkBySource(Link* link) {
    link_t index = GetIndexByLink(link);
    Link* sourceLink = GetLinkByIndex(link->Source);
    link_t rootIndex = sourceLink->RootAsSource;
    Link* rootAsSource = GetLinkByIndex(rootIndex);
    Link* currentNode = rootAsSource;
    if(rootIndex != 0) {

        while(true) {
            if(currentNode->Target > link->Target) {
                if(currentNode->LeftAsSource == 0) {
                    currentNode->LeftAsSource = GetIndexByLink(link);
                    currentNode->SizeAsSource++;
                    return BalanceBySource(currentNode);
                }
                else {
                    currentNode = GetLinkByIndex(currentNode->LeftAsSource);
                    continue;
                }
            }
            else if(currentNode->Target < link->Target){
                if(currentNode->RightAsSource == 0) {
                    currentNode->RightAsSource = GetIndexByLink(link);
                    currentNode->SizeAsSource++;
                    return BalanceBySource(currentNode);
                }
                else {
                    currentNode = GetLinkByIndex(currentNode->RightAsSource);
                    continue;
                }
            }
            else {
                return currentNode;
            }
        }
    }
    else {
        sourceLink->RootAsSource = GetIndexByLink(link);
        rootAsSource->SizeAsSource++;
        return rootAsSource;
    }
}

Link* Links::SearchLinkBySource(link_t source, link_t target) {
    Link* sourceLink = GetLinkByIndex(source);
    Link* rootAsSource = GetLinkByIndex(sourceLink->RootAsSource);
    if(rootAsSource != 0) {
        Link* currentNode = rootAsSource;
        while(true) {
            if(currentNode->Target > target) {
                if(currentNode->LeftAsSource == 0) {
                    return nullptr;
                    std::cout << "LinksPlatform] Link not found" << std::endl;
                    break;
                }
                else {
                    currentNode = GetLinkByIndex(currentNode->LeftAsSource);
                    continue;
                }
            }
            else if(currentNode->Target < target) {
                if(currentNode->RightAsSource == 0) {
                    return nullptr;
                    std::cout <<"[LinksPlatform] Link not found" << std::endl;
                    break;
                }
                else {
                    currentNode = GetLinkByIndex(currentNode->RightAsSource);
                    continue;
                }
            }
            else {
                return currentNode;
            }
        }
    }
    else {
        return nullptr;
        std::cout << "[LinksPlatform] Link not found" << std::endl;
    }
}



link_t Links::BFactorByTarget(Link* node) {
    return GetLinkByIndex(node->RightAsTarget)->SizeAsTarget - GetLinkByIndex(node->LeftAsTarget)->SizeAsTarget;
}

void Links::FixSizeByTarget(Link* node) {
    node->SizeAsTarget = 0;
    if(node->LeftAsTarget) {
        node->SizeAsTarget = GetLinkByIndex(node->LeftAsTarget)->SizeAsTarget; 
    }
    if(node->RightAsTarget) {
        node->SizeAsTarget += GetLinkByIndex(node->RightAsTarget)->SizeAsTarget;
    }
}

Link* Links::LeftRotateByTarget(Link* mNode) {
    Link* sNode = GetLinkByIndex(mNode->RightAsTarget);
    mNode->RightAsTarget = sNode->LeftAsTarget; 
    sNode->LeftAsTarget = GetIndexByLink(mNode);
    FixSizeByTarget(mNode);
    FixSizeByTarget(sNode);
    return sNode;
}

Link* Links::RightRotateByTarget(Link* mNode) {
    Link* sNode = GetLinkByIndex(mNode->RightAsTarget);
    mNode->LeftAsTarget = sNode->RightAsTarget;
    sNode->LeftAsTarget = GetIndexByLink(mNode);
    FixSizeByTarget(mNode);
    FixSizeByTarget(sNode);
    return sNode;
}


Link* Links::BalanceByTarget(Link* node) {
    FixSizeByTarget(node);
    if(BFactorByTarget(node) == 2) {
        if(BFactorByTarget(GetLinkByIndex(node->RightAsTarget)) < 0) {
            node->RightAsTarget = GetIndexByLink(
                    RightRotateByTarget(
                        GetLinkByIndex(node->RightAsTarget)));
        }
        return LeftRotateByTarget(node);
    }
    if(BFactorByTarget(node) == -2) {
        if(BFactorByTarget(GetLinkByIndex(node->LeftAsTarget)) > 0) {
            node->LeftAsTarget = GetIndexByLink(
                    RightRotateByTarget(
                        GetLinkByIndex(node->LeftAsTarget)));
        }
        return RightRotateByTarget(node);
    }
    return node;
}

Link* Links::InsertLinkByTarget(Link* link) {
    link_t index = GetIndexByLink(link);
    Link* targetLink = GetLinkByIndex(link->Target);
    link_t rootIndex = targetLink->RootAsTarget;
    Link* rootAsTarget = GetLinkByIndex(rootIndex);
    Link* currentNode = rootAsTarget;
    if(rootIndex != 0) {
        while(true) {
            if(currentNode->Source > link->Source) {
                if(currentNode->LeftAsTarget == 0) {
                    currentNode->LeftAsTarget = GetIndexByLink(link);
                    currentNode->SizeAsTarget++;
                    return BalanceByTarget(currentNode);
                }
                else {
                    currentNode = GetLinkByIndex(currentNode->LeftAsTarget);
                    continue;
                }
            }
            else if(currentNode->Source < link->Source){
                if(currentNode->RightAsTarget == 0) {
                    currentNode->RightAsTarget = GetIndexByLink(link);
                    currentNode->SizeAsTarget++;
                    return BalanceByTarget(currentNode);
                }
                else {
                    currentNode = GetLinkByIndex(currentNode->RightAsTarget);
                    continue;
                }
            }
            else {
                return currentNode;
            }
        }
    }
    else {
        targetLink->RootAsTarget = GetIndexByLink(link);
        rootAsTarget->SizeAsTarget++;
        return rootAsTarget;
    }
}

Link* Links::SearchLinkByTarget(link_t source, link_t target) {
    Link* targetLink = GetLinkByIndex(target);
    Link* rootAsTarget = GetLinkByIndex(targetLink->RootAsTarget);
    if(rootAsTarget != 0) {
        Link* currentNode = rootAsTarget;
        while(true) {
            if(currentNode->Target > target) {
                if(currentNode->LeftAsTarget == 0) {
                    return nullptr;
                    std::cout << "[LinksPlatform] Link not found" << std::endl;
                    break;
                }
                else {
                    currentNode = GetLinkByIndex(currentNode->LeftAsTarget);
                    continue;
                }
            }
            else if(currentNode->Target < target) {
                if(currentNode->RightAsTarget == 0) {
                    return nullptr;
                    std::cout <<"[LinksPlatform] Link not found" << std::endl;
                    break;
                }
                else {
                    currentNode = GetLinkByIndex(currentNode->RightAsTarget);
                    continue;
                }
            }
            else {
                return currentNode;
            }
        }
    }
    else {
        return nullptr;
        std::cout << "[LinksPlatform] Link not found" << std::endl;
    }
}
