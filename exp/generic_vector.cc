#include <cstdio>
#include <cstdlib>
#include <cstddef>

template <typename T>
struct vector {
    T *ptr;
    size_t len;
    size_t cap;

    void init() {
        ptr = (T*)malloc(sizeof(T) * 32);
        len = 0;
        cap = 32;
    }
    T get(size_t off) const {
        return ptr[off];
    }
    T& ref(size_t off) const {
        return ptr[off];
    }
    void put(size_t off, T value) {
        ptr[off] = value;
    }
    void push(T value) {
        ptr[len] = value;
        len++;
    }
    T pop() {
        len -= 1;
        return ptr[len];
    }
    void sort(bool (*cmp)(T, T)) {
        printf("comparison sort\n");
    }
    void sort(int (*key)(T)) {
        printf("key sort\n");
    }

    using iterator = T*;
    using const_iterator = const T*;
    iterator begin() const {
        return ptr;
    }
    iterator end() const {
        return ptr + len;
    }
};

vector<int> x;

int main() {
    x.init();
    x.push(100);
    x.push(101);
    x.push(102);

    for (int i=0; i<x.len; i++) {
        printf("%i\n", x.get(i));
    }

    for (int v : x) {
        printf("%i\n", v);
    }

    x.sort([](int x) { return x; });
    x.sort([](int x, int y) { return x < y; });
}

