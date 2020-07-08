#include <iostream>
#include <string.h>
#include "Links/links.hpp"
const char exitcmd[] = "exit";
const char newlink[] = "nl";
const char getlinkdata[] = "gld";
const char getlinkindex[] = "gli";
const char linkcount[] = "lc";
const char hlp[] = "help";
const char del[] = "dl";
//const char sist[] = "sist"; //Search in Source Tree
//const char sitt[] = "sitt"; //Search in Target Tree

typedef size_t link_t;

int main(int argc, char* argv[]){
    Links<link_t> links(argv[1], argv[2]);
    std::cout << "Opened " << argv[1] << " "<< argv[2] << std::endl;
    
    char answer[50];

    while(1) {
        std::cin >> answer;
        if(strncmp(newlink, answer, sizeof(newlink)) == 0) {
            link_t Source = 0, Target = 0;
            std::cin >> Source >> Target;
            link_t link = links.CreateLink(Source, Target);
            std::cout << "Link Created: (" << Source << "," << Target << ") Index: " << link << std::endl;
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
        else if(strncmp(getlinkdata, answer, sizeof(getlinkdata)) == 0) {
            link_t link;
            std::cin >> link;
            LinkData<link_t> linkData = links.GetLinkData(link);
            std::cout << link << ": " << linkData.Source << " " << linkData.Target << std::endl;
        }
        else if(strncmp(getlinkindex, answer, sizeof(getlinkindex)) == 0) {
            link_t link;
            std::cin >> link;
            LinkIndex<link_t> linkIndex = links.GetLinkIndex(link);
            std::cout << link << " Index " << std::endl;
            std::cout << "RootAsSource: " << linkIndex.RootAsSource << std::endl;
            std::cout << "LeftAsSource: " << linkIndex.LeftAsSource << std::endl;
            std::cout << "RightAsSource: " << linkIndex.RightAsSource << std::endl;
            std::cout << "SizeAsSource: " << linkIndex.SizeAsSource << std::endl;
            std::cout << "RootAsTarget: " << linkIndex.RootAsTarget << std::endl;
            std::cout << "LeftAsTarget: " << linkIndex.LeftAsTarget << std::endl;
            std::cout << "RightAsTarget: " << linkIndex.RightAsTarget << std::endl;
            std::cout << "SizeAsTarget: " << linkIndex.SizeAsTarget << std::endl;

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
        /*
        else if(strncmp(searchBySrc, answer, sizeof(searchBySrc)) == 0) {
            link_t source, target;
            std::cin >> source >> target;
            Link_t* link = links.SearchLinkBySource(source, target);
            std::cout << "Link index: " << links.GetIndexByLink(link) << std::endl; 
        }
        else if(strncmp(searchByTrg, answer, sizeof(searchByTrg)) == 0) {
            link_t source, target;
            std::cin >> source >> target;
            Link_t* link = links.SearchLinkByTarget(source, target);
            std::cout << "Link index: " << links.GetIndexByLink(link) << std::endl;
        }
        */
        else if(strncmp(exitcmd, answer, sizeof(exitcmd)) == 0) {
            break;
        }
        answer[0] = 0;
    }

    links.Close();
    return 0;
}
