
#include <iostream.hh>
#include <memory.hh>

template <typename T>
class sample_class {
public:
    T value;

    sample_class(int x) : value(x) {
        nx::cout << "constructing\n";
    }

    ~sample_class() {
        nx::cout << "destructing\n";
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
    auto foo = new sample_class<int>(10);
    auto bar = nx::unique_ptr{foo};

    nx::cout << *bar << "\n";

    return 0;
}


