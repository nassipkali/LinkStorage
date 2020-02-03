#include <iostream>
#include "Links/links.hpp"
using namespace std;

int main()
{
    Links links("db.links");
    cout << "Hello World!" << endl;
    Link link = links.Create();
    return 0;
}
