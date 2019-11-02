
#include <iostream.hh>

template <typename T>
class sample_class {
public:
    T value;

    sample_class(int x) : value(x) {
        printf("Constructing a %s\n", __PRETTY_FUNCTION__);
    }

    ~sample_class() {
        printf("Destructing a %s\n", __PRETTY_FUNCTION__);
    }
    
    template <typename U>
    friend nx::ostream& operator<<(nx::ostream&, sample_class<U> const&);
};

template <typename T>
nx::ostream& operator<<(nx::ostream& s, sample_class<T> const& v) {
    return s << "sample_class{" << v.value << "}";
}

extern "C"
int main() {
    sample_class<int> object{100};
    nx::cout << object << "\n";

    return 0;
}


