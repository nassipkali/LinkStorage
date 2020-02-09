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

void Links::SourceLeftRotate ( Link* root ) {
    Link* right = GetLinkByIndex(root->RightAsSource);
    if(right == nullptr) {
        std::cerr << "SourceLeftRotate: Right is null" << std::endl;
    }
    root->RightAsSource = right->LeftAsSource;
    right->LeftAsSource = GetIndexByLink(root);
    right->SizeAsSource = root->SizeAsSource;
    Link* left = GetLinkByIndex(root->LeftAsSource);
    root->SizeAsSource = left->SizeAsSource + right->SizeAsSource + 1;
}

void Links::SourceRightRotate( Link* root) {
    Link* left = GetLinkByIndex(root->LeftAsSource);
    if(left == nullptr) {
        std::cerr << "SourceRightRotate: Left is null" << std::endl;
    }
    root->LeftAsSource = left->RightAsSource;
    left->RightAsSource = GetIndexByLink(root);
    left->SizeAsSource = root->SizeAsSource;
    Link* right = GetLinkByIndex(root->RightAsSource);
    root->SizeAsSource = left->SizeAsSource + right->SizeAsSource + 1;
}

void Links::InsertToSourceTree(link_t index) {
    while(1) {
        Link* link = GetLinkByIndex(index);
        Link* root = GetLinkByIndex(FirstAsSource);
        Link* left = GetLinkByIndex(root->LeftAsSource);
        size_t leftSize = left->SizeAsSource;
        Link* right = GetLinkByIndex(root->RightAsSource);
        size_t rightSize = right->SizeAsSource;
        if(root->Source > link->Source || (root->Source == link->Source && root->Target > link->Target)) {
            if(left == nullptr) {
                root->SizeAsSource++;
                link->SizeAsSource = 1;
                *left = *link;
                return;
            }
            if(left->Source > link->Source || (left->Source == link->Source && left->Target > link->Target)) {
                if((leftSize + 1) > rightSize) {
                    SourceRightRotate(root);
                }
                else {
                    root->SizeAsSource++;
                    *root = *left;
                }
            }
            else {
                Link* leftRight = GetLinkByIndex(left->RightAsSource);
                size_t leftRightSize = leftRight->SizeAsSource;
                if ((leftRightSize + 1) > rightSize)
                {
                    if (leftRightSize == 0 && rightSize == 0)
                    {
                        link->LeftAsSource = GetIndexByLink(left);
                        link->RightAsSource = GetIndexByLink(root);
                        link->SizeAsSource = leftSize + 2;
                        root->LeftAsSource = 0;
                        root->SizeAsSource = 1;
                        *root = *link;
                        return;
                    }
                    SourceLeftRotate(left);
                    SourceRightRotate(root);
                }
                else
                {
                    root->SizeAsSource++;
                    *root = *left;
                }
            }
        }
        else {
            if(right == nullptr) {
                root->SizeAsSource++;
                link->SizeAsSource = 1;
                *right = *link;
                return;
            }
            if(root->Source < link->Source || (root->Source == link->Source && root->Target < link->Target)) {
                if((rightSize + 1) > leftSize) {
                    SourceLeftRotate(root);
                }
                else {
                    root->SizeAsSource++;
                    *root = *right;
                }
            }
            else {
                Link* rightLeft = GetLinkByIndex(right->LeftAsSource);
                size_t rightLeftSize = rightLeft->SizeAsSource;
                if((rightLeftSize + 1) > leftSize) {
                    if (rightLeftSize == 0 && leftSize == 0) {
                        link->LeftAsSource = GetIndexByLink(root);
                        link->RightAsSource = GetIndexByLink(right);
                        link->SizeAsSource = rightSize + 2;
                        root->RightAsSource = 0;
                        root->SizeAsSource = 1;
                        *root = *link;
                        return;
                    }
                    SourceRightRotate(right);
                    SourceLeftRotate(root);
                }
                else {
                    root->SizeAsSource++;
                    *root = *right;
                }
            }
        }
    }
}

void Links::InsertToSourceTree(Link* link) {
    while(1) {
        Link* root = GetLinkByIndex(FirstAsSource);
        Link* left = GetLinkByIndex(root->LeftAsSource);
        size_t leftSize = left->SizeAsSource;
        Link* right = GetLinkByIndex(root->RightAsSource);
        size_t rightSize = right->SizeAsSource;
        if(root->Source > link->Source || (root->Source == link->Source && root->Target > link->Target)) {
            if(left == nullptr) {
                root->SizeAsSource++;
                link->SizeAsSource = 1;
                *left = *link;
                return;
            }
            if(left->Source > link->Source || (left->Source == link->Source && left->Target > link->Target)) {
                if((leftSize + 1) > rightSize) {
                    SourceRightRotate(root);
                }
                else {
                    root->SizeAsSource++;
                    *root = *left;
                }
            }
            else {
                Link* leftRight = GetLinkByIndex(left->RightAsSource);
                size_t leftRightSize = leftRight->SizeAsSource;
                if ((leftRightSize + 1) > rightSize)
                {
                    if (leftRightSize == 0 && rightSize == 0)
                    {
                        link->LeftAsSource = GetIndexByLink(left);
                        link->RightAsSource = GetIndexByLink(root);
                        link->SizeAsSource = leftSize + 2;
                        root->LeftAsSource = 0;
                        root->SizeAsSource = 1;
                        *root = *link;
                        return;
                    }
                    SourceLeftRotate(left);
                    SourceRightRotate(root);
                }
                else
                {
                    root->SizeAsSource++;
                    *root = *left;
                }
            }
        }
        else {
            if(right == nullptr) {
                root->SizeAsSource++;
                link->SizeAsSource = 1;
                *right = *link;
                return;
            }
            if(root->Source < link->Source || (root->Source == link->Source && root->Target < link->Target)) {
                if((rightSize + 1) > leftSize) {
                    SourceLeftRotate(root);
                }
                else {
                    root->SizeAsSource++;
                    *root = *right;
                }
            }
            else {
                Link* rightLeft = GetLinkByIndex(right->LeftAsSource);
                size_t rightLeftSize = rightLeft->SizeAsSource;
                if((rightLeftSize + 1) > leftSize) {
                    if (rightLeftSize == 0 && leftSize == 0) {
                        link->LeftAsSource = GetIndexByLink(root);
                        link->RightAsSource = GetIndexByLink(right);
                        link->SizeAsSource = rightSize + 2;
                        root->RightAsSource = 0;
                        root->SizeAsSource = 1;
                        *root = *link;
                        return;
                    }
                    SourceRightRotate(right);
                    SourceLeftRotate(root);
                }
                else {
                    root->SizeAsSource++;
                    *root = *right;
                }
            }
        }
    }
}
void Links::InsertToTargetTree(link_t index) {
    Link* root = GetLinkByIndex(FirstAsTarget);
}
void Links::InsertToTargetTree(Link* link) {
    Link* root = GetLinkByIndex(FirstAsTarget);
}

link_t Links::SearchInSourceTree( link_t source, link_t target ) {
    
}

link_t Links::SearchInTargetTree(link_t source, link_t target) {
    
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




