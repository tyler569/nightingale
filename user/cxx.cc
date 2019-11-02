
extern "C" {
#include <stdio.h>
}

template <typename T>
class sample_class {
public:
    T value;

    sample_class() {
        printf("Constructing a %s\n", __PRETTY_FUNCTION__);
    }

    ~sample_class() {
        printf("Destructing a %s\n", __PRETTY_FUNCTION__);
    }
};

extern "C"
int main() {
    sample_class<int> object{};

    printf("object value is %i\n", object.value);

    return 0;
}


