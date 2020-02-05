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
  template<typename T>
  T LinkToNumber(Link* link);
};


template<typename T>
Link* Links::NumberToLink(T num) {
	Link* link = Create(1, 0);
	T ExpOfTwo = 1;
	int SecondIter;
	for(int i = 0; i < sizeof(T)*8; i++) {
		if(ExpOfTwo & num) {
			link->Target = i + 1;
			SecondIter = i + 1;
			ExpOfTwo = ExpOfTwo << 1;
			break;
		}
		ExpOfTwo = ExpOfTwo << 1;
	}
	for(int i = SecondIter; i < sizeof(T)*8; i++){
		if(ExpOfTwo & num) {
			link = Create(GetIndexByLink(link), i + 1);
		}
		ExpOfTwo = ExpOfTwo << 1;
	}
	return link;
}

template <typename T>
T Links::LinkToNumber(Link* link) {
  T num = 0;
  for(int i = 0; i < 64; i++) {
    if(link->Source == 1) {
      num += (1 << link->Target) - 1;
      break;
    }
    else {
      num += (1 << link->Target) - 1;
      link = this->GetLinkByIndex(link->Source);
    }
  }
  return num;
}

#endif // LINKS_HPP
