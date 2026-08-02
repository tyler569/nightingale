#include <cstdio>
#include <cstdint>
#include <new>

using std::ptrdiff_t;

template <class T, class Inner>
T *containerof(Inner *obj, Inner T::*member) {
    constexpr T* dummy = nullptr;
    ptrdiff_t offset = reinterpret_cast<uintptr_t>(&(dummy->*member));
    
    return reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(obj) - offset);
}

struct list_node {
    list_node *next, *prev;

    list_node() : next(this), prev(this) {}

    void insert_after(list_node *n) {
        n->prev = this;
        n->next = next;
        next->prev = n;
        next = n;
    }

    void insert_before(list_node *n) {
        n->prev = prev;
        n->next = this;
        prev->next = n;
        prev = n;
    }
};

template <class T, list_node T::*node>
struct list {
    list_node sentinel;

    struct iterator {
        list_node *cursor;

        iterator& operator++() { cursor = cursor->next; return *this; }
        iterator& operator--() { cursor = cursor->prev; return *this; }
        bool operator==(const iterator& rhs) const { return cursor == rhs.cursor; }
        bool operator!=(const iterator& rhs) const { return cursor != rhs.cursor; }
        T& operator*() { return *containerof(cursor, node); }
    };

    struct const_iterator {
        const list_node *cursor;

        iterator& operator++() { cursor = cursor->next; return *this; }
        iterator& operator--() { cursor = cursor->prev; return *this; }
        bool operator==(const iterator& rhs) const { return cursor == rhs.cursor; }
        bool operator!=(const iterator& rhs) const { return cursor != rhs.cursor; }
        const T& operator*() { return *containerof(cursor, node); }
    };

    iterator begin() { return iterator{sentinel.next}; }
    iterator end() { return iterator{&sentinel}; }
    const_iterator begin() const { return iterator{sentinel.next}; }
    const_iterator end() const { return iterator{&sentinel}; }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    void append(T *obj) { sentinel.insert_before(&(obj->*node)); }
    void prepend(T *obj) { sentinel.insert_after(&(obj->*node)); }
};


struct foo {
    list_node node;
    int data;
};

list<foo, &foo::node> foos;

int main() {
    foo some_foos[10];
    for (int i = 0; i < 10; i++) {
        foo *f = &some_foos[i];
        f->data = i * 10;
        foos.append(f);
    }

    for (auto&& f : foos) {
        printf("f.data = %i\n", f.data);
    }
}
