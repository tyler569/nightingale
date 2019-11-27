
#include <basic.h>
#include <memory.hh>
#include <iostream.hh>
#include <string.hh>

using nx::string;
using nx::cout;

extern "C"
int main() {
        string foo = "Hello world!\n";
        cout << foo;

        return 0;
}

