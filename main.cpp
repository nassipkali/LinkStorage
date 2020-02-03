#include <iostream>
#include "Links/links.hpp"
using namespace std;

int main()
{
    Links links("db.links");
    cout << "Hello World!" << endl;
    Link *link = links.Create();
    cout << "Link Count: " << links.GetLinkCount() << endl;
    cout << "My first link! (" << link->Source<< " " << link->Target << ")\n";
    links.Close();
    return 0;
}
