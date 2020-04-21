#include <iostream>
//#include <cstdio>
//#include <stdio.h>

int main()
{
    using namespace std;
    cout << "Hello, world!\n";


#ifdef _WIN32_WINNT
    cout << "_WIN32_WINNT = " << hex << _WIN32_WINNT << endl;
#endif
#ifdef _WIN32_WINDOWS
    cout << "_WIN32_WINDOWS = " << hex << _WIN32_WINDOWS << endl;
#endif
#ifdef WINVER 
    cout << "WINVER = " << hex << WINVER << endl;
#endif
    return 0;
}

