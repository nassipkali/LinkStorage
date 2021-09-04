#ifndef LINKSTORAGE_HPP
#define LINKSTORAGE_HPP
#include "linkdata.hpp"
#include "linkindex.hpp"
#include "linksmemory.hpp"
#include <stack>
#include <iostream>
#include <vector>
template <typename T>
class LinkStorage
{
    private:
        MemoryMappedFileArray<LinkData<T>> data_array;
        MemoryMappedFileArray<LinkIndex<T>> index_array;
        T free_links;
        T last_free_link;
        void LeftRotateInSourceTree(T rnode, T tnode);
        void RightRotateInSourceTree(T rnode, T tnode);
        void LeftRotateInTargetTree(T rnode, T tnode);
        void RightRotateInTargetTree(T rnode, T tnode);
        void MaintainInSourceTree(T rnode, T node, bool flag);
        void MaintainInTargetTree(T rnode, T node, bool flag);
        void InsertLinkToSourceTree(T link);
        void InsertLinkToTargetTree(T link);
        T SearchLinkInSourceTree(T source, T target);
    public:
        LinkStorage(const char* data_file, const char* index_file);
        ~LinkStorage();
        T CreateLink(T source, T target);
        T CreateDot();
        void UpdateLink(T linkindex, T source, T target);
        bool Delete(T index);
        T SearchLink(T source, T target);
      	T GetSourcedCount(T link);
        T GetTargetedCount(T link);
        std::vector<T> GetSourced(T link);
        std::vector<T> GetTargeted(T link);
        T GetSourcedWithSource(T link, T source);
        T GetSourcedWithTarget(T link, T target);
        T GetTargetedWithSource(T link, T source);
        T GetTargetedWithTarget(T link, T target);
        T CreateSequence(std::vector<T> links);
        T SearchSequence(std::vector<T> links);
        LinkData<T> GetLinkData(T link);
        LinkIndex<T> GetLinkIndex(T link);
        T GetFreeLinksCount();
        size_t GetLinksCount();
};



template <typename T>
LinkStorage<T>::LinkStorage(const char* data_file, const char* index_file) : data_array(data_file), index_array(index_file) {
    // MetaData loads from 0 link
    if(this->data_array.GetSize() == 0) {
    	LinkData<T> zero_data = {0,0};
    	LinkIndex<T> zero_index = {0,0,0,0,0,0,0,0};
    	this->data_array.Add(zero_data);
    	this->index_array.Add(zero_index);
    }
    this->free_links = this->index_array[0].root_as_source; // Количество неиспользуемых/удаленных связей
    this->last_free_link = this->index_array[0].left_as_source; //Индекс последней удаленной связи
}



template <typename T>
T LinkStorage<T>::CreateLink(T source, T target) {
    if(source == 0 || target == 0) {
        std::cerr << "[LinkStorage] Can't create link with reference to zero" << std::endl;
        return 0;
    }
    
    T link;
    
    if(this->free_links > 0) {
        link = this->last_free_link;
        this->last_free_link = this->data_array[this->last_free_link].source;
        this->free_links--;
        this->data_array[link] = {source, target};
        this->index_array[link] = {0,0,0,0,0,0,0,0};
    }
    else {
    	link = data_array.GetSize();
    	LinkData<T> link_data = {source, target};
    	this->data_array.Add(link_data);
        LinkIndex<T> link_index = {0,0,0,0,0,0,0,0};
        this->index_array.Add(link_index);
    }
    
    //Insert to tree
    this->InsertLinkToSourceTree(link);
    
    return link;
}

template <typename T>
T LinkStorage<T>::CreateDot() {  
    T link;
    
    if(this->free_links > 0) {
        link = this->last_free_link;
        this->last_free_link = this->data_array[this->last_free_link].source;
        this->free_links--;
        this->data_array[link] = {link, link};
        this->index_array[link] = {0,0,0,0,0,0,0,0};
        
    }
    else {
    	link = data_array.GetSize();
    	LinkData<T> link_data = {link, link};
    	this->data_array.Add(link_data);
        LinkIndex<T> link_index = {0,0,0,0,0,0,0,0};
        this->index_array.Add(link_index);
    }
    
    //Insert to tree
    this->InsertLinkToSourceTree(link);
    
    return link;
}

template <typename T>
void LinkStorage<T>::UpdateLink (T link, T source, T target ) {
	if(source == 0 || target == 0) {
        std::cerr << "[LinkStorage] Can't create link with reference to zero" << std::endl;
        return;
    }
    this->data_array[link].source = source;
    this->data_array[link].target = target;
}

template <typename T>
bool LinkStorage<T>::Delete(T link) {
	if(this->free_links == 0) {
		this->last_free_link = link;
		this->free_links++;
	}
	else {
		this->data_array[this->last_free_link].target = link;
		this->data_array[link].source = this->last_free_link;
		this->last_free_link = link;
		this->free_links++;
	}
	return true;
}

template <typename T>
void LinkStorage<T>::InsertLinkToSourceTree(T link) {
	LinkData<T> link_data = this->data_array[link];
	T source = link_data.source;
	T target = link_data.target;
	T root = this->index_array[source].root_as_source;
	if(!root) {
		this->index_array[source].root_as_source = link;
		this->index_array[source].size_as_source = 1;
		return;
	}
	T current_link = root;
	std::vector<T> nodes;
	while(true) {
		if(this->data_array[current_link].target > target) {
			if(this->index_array[current_link].left_as_source != 0) {
				this->index_array[current_link].size_as_source++;
				nodes.push_back(current_link);
				current_link = this->index_array[current_link].left_as_source;
				continue;
			}
			else {
				this->index_array[current_link].size_as_source++;
				nodes.push_back(current_link);
				this->index_array[current_link].left_as_source = link;
				break;
			}
		}
		else {
			if(this->index_array[current_link].right_as_source != 0) {
				this->index_array[current_link].size_as_source++;
				nodes.push_back(current_link);
				current_link = this->index_array[current_link].right_as_source;
			}
			else {
				this->index_array[current_link].size_as_source++;
				nodes.push_back(current_link);
				this->index_array[current_link].right_as_source = link;
				break;
			}
		}
	}
	while(nodes.size() > 2) {
		size_t pos = nodes.size() - 1;
        T lnode = nodes[pos];
        nodes.pop_back();
        T hnode = nodes[pos];
        T rnode = nodes[pos - 1];
        this->MaintainInSourceTree(rnode, hnode, this->data_array[lnode].target > this->data_array[hnode].target);
    }
    if(nodes.size() == 2) {
        T lnode = nodes.back();
        nodes.pop_back();
        T hnode = nodes.back();
        nodes.pop_back();
        this->MaintainInSourceTree(0, hnode, this->data_array[lnode].target > this->data_array[hnode].target);
        T root_left = this->index_array[root].left_as_source;
    	T root_right = this->index_array[root].right_as_source;
        if(this->index_array[root_left].right_as_source == root) {
            this->index_array[source].root_as_source = root_left;
        }
        else if(this->index_array[root_right].left_as_source == root){
            this->index_array[source].root_as_source = root_right;
        }
    }
}

template <typename T>
void LinkStorage<T>::LeftRotateInSourceTree(T rnode, T tnode) {

	T knode = this->index_array[tnode].right_as_source;
	this->index_array[tnode].right_as_source = this->index_array[knode].left_as_source;
	this->index_array[knode].left_as_source = tnode;
	this->index_array[knode].size_as_source = this->index_array[tnode].size_as_source;
	T tnode_left = this->index_array[tnode].left_as_source;
	T tnode_right = this->index_array[tnode].right_as_source;
	T tnode_left_size = this->index_array[tnode_left].size_as_source;
	T tnode_right_size = this->index_array[tnode_right].size_as_source;
	this->index_array[tnode].size_as_source = tnode_left_size + tnode_right_size + 1;
	
	if(!rnode) {
		return;
	}
	
	if(this->index_array[rnode].right_as_source == tnode) {
        this->index_array[rnode].right_as_source = knode;
    }
    else {
        this->index_array[rnode].left_as_source = knode;
    }
    
}

template <typename T>
void LinkStorage<T>::RightRotateInSourceTree(T rnode, T tnode) {

	T knode = this->index_array[tnode].left_as_source;
	this->index_array[tnode].left_as_source = this->index_array[knode].right_as_source;
	this->index_array[knode].right_as_source = tnode;
	this->index_array[knode].size_as_source = this->index_array[tnode].size_as_source;
	T tnode_left = this->index_array[tnode].left_as_source;
	T tnode_right = this->index_array[tnode].right_as_source;
	T tnode_left_size = this->index_array[tnode_left].size_as_source;
	T tnode_right_size = this->index_array[tnode_right].size_as_source;
	this->index_array[tnode].size_as_source = tnode_left_size + tnode_right_size + 1;
	
	if(!rnode) {
		return;
	}
	
	if(this->index_array[rnode].right_as_source == tnode) {
        this->index_array[rnode].right_as_source = knode;
    }
    else {
        this->index_array[rnode].left_as_source = knode;
    }
    
}

template <typename T>
void LinkStorage<T>::MaintainInSourceTree(T rnode, T node, bool flag) {
	T node_left = this->index_array[node].left_as_source;
	T node_right = this->index_array[node].right_as_source;
	
	if(flag) {
        if(this->index_array[node_left].size_as_source < this->index_array[this->index_array[node_right].left_as_source].size_as_source) {
            //case 1
            this->RightRotateInSourceTree(node, this->index_array[node].right_as_source);
            this->LeftRotateInSourceTree(rnode, node);
        }
        else if(this->index_array[node_left].size_as_source < this->index_array[this->index_array[node_right].right_as_source].size_as_source) {
            //case 2
            this->LeftRotateInSourceTree(rnode, node);
        }
        else {
            return;
        }
    }
    else {
        if(this->index_array[node_right].size_as_source < this->index_array[this->index_array[node_left].right_as_source].size_as_source) {
            //case 1'
            this->LeftRotateInSourceTree(node, this->index_array[node].left_as_source);
            this->RightRotateInSourceTree(rnode, node);
        }
        else if(this->index_array[node_right].size_as_source < this->index_array[this->index_array[node_left].left_as_source].size_as_source) {
            this->RightRotateInSourceTree(rnode, node);
        }
        else {
            return;
        }
    }
    this->MaintainInSourceTree(node, this->index_array[node].left_as_source, false);
    this->MaintainInSourceTree(node, this->index_array[node].right_as_source, true);
    this->MaintainInSourceTree(rnode, node, true);
    this->MaintainInSourceTree(rnode, node, false);	
}

template <typename T>
T LinkStorage<T>::SearchLinkInSourceTree(T source, T target) {
	T current_link = this->index_array[source].root_as_source;
    while(true) {
        if(this->data_array[current_link].target > target) {
            if(this->index_array[current_link].left_as_source != 0) {
                current_link = this->index_array[current_link].left_as_source;
                continue;
            }
            else {
                return 0;
            }
        }
        else if(this->data_array[current_link].target < target) {
            if(this->index_array[current_link].right_as_source != 0) {
                current_link = this->index_array[current_link].right_as_source;
                continue;
            }
            else {
                return 0;
            }
        }
        else {
            return current_link;
        }
    }
}

/*
template <typename T>
void LinkStorage<T>::MaintainInTargetTree(T rnode, T node, bool flag) {
	
}
*/

template <typename T>
T LinkStorage<T>::SearchLink(T source, T target) {
	T link = this->SearchLinkInSourceTree(source, target);
	return link;
}

template <typename T>
LinkData<T> LinkStorage<T>::GetLinkData(T link) {
    return this->data_array[link];
}

template <typename T>
LinkIndex<T> LinkStorage<T>::GetLinkIndex(T link) {
    return this->index_array[link];
}

template <typename T>
size_t LinkStorage<T>::GetLinksCount() {
	return data_array.GetSize() - 1;
}


template <typename T>
T LinkStorage<T>::GetFreeLinksCount() {
    return this->free_links;
}

template <typename T>
LinkStorage<T>::~LinkStorage() {
	this->index_array[0].root_as_source = this->free_links; // Количество неиспользуемых/удаленных связей
	this->index_array[0].left_as_source = this->last_free_link; //Индекс последней удаленной связи
}



#endif // LINKS_HPP
