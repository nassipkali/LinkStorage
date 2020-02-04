#ifndef LINKS_HPP
#define LINKS_HPP
#include "link.hpp"
#include "linksmemory.hpp"
#include <stdint.h>
#include <iostream>



class Links
{
private:
    	LinksMemory Memory;
    	void Init();
public:
	Links(const char* dbname);
	Links(const char* dbname, size_t BlockSize);
	void CreateNoRet();
	void CreateNoRet(link_t target);
	void CreateNoRet(link_t source, link_t target);
	Link* Create();
	Link* Create(link_t target);
	Link* Create(link_t source, link_t target);
	void Close();
	size_t GetLinkCount();
	Link* GetLinkByIndex(link_t index);
	link_t GetIndexByLink(Link* link);
	size_t GetMemoryMapSize();
	size_t GetMemoryUse();
 	template<typename T>
	Link* NumberToLink(T num);
};


template<typename T>
Link* Links::NumberToLink(T num) {
	Link* link = Create(0, 0);
	T ExpOfTwo = 1;
	int SecondIter;
	for(int i = 0; i < sizeof(T)*8; i++) {
		if(ExpOfTwo & num) {
			link->Target = i + 1;
			std::cout << "Stepen:" << i << std::endl;
			SecondIter = i + 1;
			ExpOfTwo = ExpOfTwo << 1;
			break;
		}
		ExpOfTwo = ExpOfTwo << 1;
	}
	for(int i = SecondIter; i < sizeof(T)*8; i++){
		if(ExpOfTwo & num) {
			link = Create(GetIndexByLink(link), i);
			std::cout << "Stepen:" << ExpOfTwo << std::endl;
		}
		ExpOfTwo = ExpOfTwo << 1;
	}
	return link;
}


#endif // LINKS_HPP
