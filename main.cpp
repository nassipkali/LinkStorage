#include <iostream>
#include <string.h>
#include <unistd.h>
#include "Links/links.hpp"
const char exitcmd[] = "exit";
const char newlink[] = "nl";
const char newdot[] = "nd";
const char getlinkdata[] = "gld";
const char getlinkindex[] = "gli";
const char linkcount[] = "lc";
const char hlp[] = "help";
const char del[] = "dl";
const char searchLink[] = "sl";

typedef size_t link_t;

int main(int argc, char* argv[]){
    Links<link_t> links(argv[1], argv[2]);
    char answer[50];
    while(1) {
        std::cin >> answer;
        if(strncmp(newlink, answer, sizeof(newlink)) == 0) {
            link_t source, target;
            std::cin >> source >> target;
            link_t link = links.CreateLink(source, target);
            std::cout << "Link Created: (" << source << "," << target << ") Index: " << link << std::endl;
        }
        else if(strncmp(newdot, answer, sizeof(newdot)) == 0) {
            link_t link = links.CreateDot();
            LinkData<link_t> linkData = links.GetLinkData(link);
            std::cout << "Link Dot Created: (" << linkData.source << "," << linkData.target << ") Index: " << link << std::endl;
        }
        else if(strncmp(hlp, answer, sizeof(hlp)) == 0) {
            printf("\t\t    nl (Source index) (Target Index)\n\
                    Creates new link.\n\
                    gld (Link Index)\n\
                    Prints Link Source, Target\n\
                    lc\n\
                    Prints Link Count\n\
                    "
        }
        else if(strncmp(getlinkdata, answer, sizeof(getlinkdata)) == 0) {
            link_t link;
            std::cin >> link;
            LinkData<link_t> linkData = links.GetLinkData(link);
            std::cout << link << ": " << linkData.source << " " << linkData.target << std::endl;
        }
        else if(strncmp(getlinkindex, answer, sizeof(getlinkindex)) == 0) {
            link_t link;
            std::cin >> link;
            LinkIndex<link_t> linkIndex = links.GetLinkIndex(link);
            std::cout << link << " Index " << std::endl;
            std::cout << "RootAsSource: " << linkIndex.root_as_source << std::endl;
            std::cout << "LeftAsSource: " << linkIndex.left_as_source << std::endl;
            std::cout << "RightAsSource: " << linkIndex.right_as_source << std::endl;
            std::cout << "SizeAsSource: " << linkIndex.size_as_source << std::endl;
            std::cout << "RootAsTarget: " << linkIndex.root_as_target << std::endl;
            std::cout << "LeftAsTarget: " << linkIndex.left_as_target << std::endl;
            std::cout << "RightAsTarget: " << linkIndex.right_as_target << std::endl;
            std::cout << "SizeAsTarget: " << linkIndex.size_as_target << std::endl;
        }
        else if(strncmp(linkcount, answer, sizeof(linkcount)) == 0) {
            std::cout << "Link count: " << links.GetLinksCount() << std::endl;
        }
        else if(strncmp(del, answer, sizeof(del)) == 0) {
            link_t index;
            std::cin >> index;
            links.Delete(index);
            std::cout << "Free count: " << links.GetFreeLinksCount() << std::endl;
        }
        /*
        else if(strncmp(searchLink, answer, sizeof(searchLink)) == 0) {
            link_t source, target;
            std::cin >> source >> target;
            link_t link = links.SearchLink(source, target);
            std::cout << "Link Index: " << link << std::endl;
        }*/
        else if(strncmp(exitcmd, answer, sizeof(exitcmd)) == 0) {
            break;
        }
        answer[0] = 0;
    }

    return 0;
}
