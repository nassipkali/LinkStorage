#include <iostream>
#include <string.h>
#include "Links/links.hpp"
const char exitcmd[] = "exit";
const char newlink[] = "nl";
const char getlink[] = "gl";
const char linkcount[] = "lc";
const char mem[] = "mem";
const char hlp[] = "help";
const char del[] = "dl";
const char searchBySrc[] = "sbs";
const char searchByTrg[] = "sbt";
int main(int argc, char* argv[]){
    Links links(argv[1]);
    std::cout << "Opened " << argv[1] << std::endl;
    
    char answer[50];

    while(1) {
        std::cin >> answer;
        if(strncmp(newlink, answer, sizeof(newlink)) == 0) {
            link_t Source = 0, Target = 0;
            std::cin >> Source >> Target;
            Link* link = links.CreateLink(Source, Target);
            std::cout << "Link Created: (" << Source << "," << Target << ") Index: " << links.GetIndexByLink(link) << std::endl;
        }
        else if(strncmp(hlp, answer, sizeof(hlp)) == 0) {
            printf("\t\t    nl (Source index) (Target Index)\n\
                    Creates new link.\n\
                    gl (Link Index)\n\
                    Prints Link Source, Target\n\
                    lc\n\
                    Prints Link Count\n\
                    mem\n\
                    Prints Mapped Memory used/size\n");
        }
        else if(strncmp(mem, answer, sizeof(mem)) == 0) {
            std::cout << "Memory used: " << links.GetMemoryMapSize() << "/" << links.GetMemoryUse() << std::endl;
        }
        else if(strncmp(getlink, answer, sizeof(getlink)) == 0) {
            link_t index;
            std::cin >> index;
            Link *link = links.GetLinkByIndex(index);
            std::cout << index << ": " << link->Source << " " << link->Target << std::endl;
            std::cout << "LeftAsSource: " << link->LeftAsSource << " RightAsSource: " << link->LeftAsSource << " SizeAsSource: " << link->SizeAsSource << std::endl;
            std::cout << "LeftAsTarget: " << link->LeftAsTarget << " RightAsTarget: " << link->LeftAsTarget << " SizeAsSource: " << link->SizeAsTarget << std::endl;
        }
        else if(strncmp(linkcount, answer, sizeof(linkcount)) == 0) {
            std::cout << "Link count: " << links.GetAllocatedLinksCount() << std::endl;
        }
        else if(strncmp(del, answer, sizeof(del)) == 0) {
            link_t index;
            std::cin >> index;
            links.Delete(index);
            std::cout << "Free count: " << links.GetFreeLinksCount() << std::endl;
        }
        else if(strncmp(searchBySrc, answer, sizeof(searchBySrc)) == 0) {
            link_t source, target;
            std::cin >> source >> target;
            Link* link = links.SearchLinkBySource(source, target);
            std::cout << "Link index: " << links.GetIndexByLink(link) << std::endl; 
        }
        else if(strncmp(searchByTrg, answer, sizeof(searchByTrg)) == 0) {
            link_t source, target;
            std::cin >> source >> target;
            Link* link = links.SearchLinkByTarget(source, target);
            std::cout << "Link index: " << links.GetIndexByLink(link) << std::endl;
        }
        else if(strncmp(exitcmd, answer, sizeof(exitcmd)) == 0) {
            break;
        }
        answer[0] = 0;
    }

    links.Close();
    return 0;
}
