#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace nx {

class list_node {
    list_node *next { nullptr };
    list_node *previous { nullptr };

    template <class U, list_node U::*link_field> friend class list;

public:
    constexpr list_node() = default;

    list_node(const list_node &) = delete;
    list_node &operator=(const list_node &) = delete;

    list_node(list_node &&) = delete;
    list_node &operator=(list_node &&) = delete;

    bool is_null() { return next == nullptr && previous == nullptr; }
};

template <class T, list_node T::*link_field> class list {
    list_node *head {};
    list_node *tail {};

public:
    constexpr list() = default;

    void push_back(T &item)
    {
        auto *node = &(item.*link_field);
        assert(node->next == nullptr);
        assert(node->previous == nullptr);
        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            node->previous = tail;
            tail = node;
        }
    }

    void push_front(T &item)
    {
        auto *node = &(item.*link_field);
        assert(node->next == nullptr);
        assert(node->previous == nullptr);
        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            head->previous = node;
            node->next = head;
            head = node;
        }
    }

    void insert(T &item, T &before)
    {
        auto *node = &(item.*link_field);
        auto *before_node = &(before.*link_field);
        assert(node->next == nullptr);
        assert(node->previous == nullptr);
        if (before_node == head) {
            push_front(item);
        } else {
            node->next = before_node;
            node->previous = before_node->previous;
            before_node->previous->next = node;
            before_node->previous = node;
        }
    }

    T &pop_front()
    {
        assert(head);
        auto *node = head;
        head = head->next;
        if (head) {
            head->previous = nullptr;
        }
        if (node == tail) {
            tail = nullptr;
        }
        node->next = nullptr;
        node->previous = nullptr;
        return object_of(node);
    }

    T &pop_back()
    {
        assert(tail);
        auto *node = tail;
        tail = tail->previous;
        if (tail) {
            tail->next = nullptr;
        }
        if (node == head) {
            head = nullptr;
        }
        node->next = nullptr;
        node->previous = nullptr;
        return object_of(node);
    }

    void remove(T &item)
    {
        auto node = &(item.*link_field);
        if (node == head) {
            head = node->next;
        }
        if (node == tail) {
            tail = node->previous;
        }
        if (node->previous != nullptr) {
            node->previous->next = node->next;
        }
        if (node->next != nullptr) {
            node->next->previous = node->previous;
        }
        node->next = nullptr;
        node->previous = nullptr;
    }

    void clear()
    {
        for (auto *node = head; node != nullptr;) {
            auto old_node = node;
            node = node->next;
            old_node->next = nullptr;
            old_node->previous = nullptr;
        }

        head = nullptr;
        tail = nullptr;
    }

    bool empty() { return head == nullptr; }

    size_t size()
    {
        size_t count = 0;
        for (auto node = head; node != nullptr; node = node->next) {
            count++;
        }
        return count;
    }

    constexpr static ptrdiff_t offset()
    {
        return reinterpret_cast<ptrdiff_t>(
            &(reinterpret_cast<T *>(0)->*link_field));
    }

    constexpr static T &object_of(list_node *node)
    {
        return *reinterpret_cast<T *>(
            reinterpret_cast<uint8_t *>(node) - offset());
    }

    T &front() { return object_of(head); }

    T &back() { return object_of(tail); }

    T &next(T &item)
    {
        auto node = &(item.*link_field);
        return object_of(node->next);
    }

    T &previous(T &item)
    {
        auto node = &(item.*link_field);
        return object_of(node->previous);
    }

    T &operator[](size_t index)
    {
        auto node = head;
        for (size_t i = 0; i < index; i++) {
            node = node->next;
        }
        return object_of(node);
    }

    class iterator {
        list_node *node;

    public:
        explicit iterator(list_node *node)
            : node(node)
        {
        }

        iterator &operator++()
        {
            node = node->next;
            return *this;
        }

        iterator &operator--()
        {
            node = node->previous;
            return *this;
        }

        T &operator*() const { return list::object_of(node); }
        T *operator->() const { return &list::object_of(node); }

        bool operator==(const iterator &other) const
        {
            return node == other.node;
        }

        bool operator!=(const iterator &other) const
        {
            return node != other.node;
        }
    };

    iterator begin() const { return iterator(head); }

    iterator end() const { return iterator(nullptr); }

    template <class F> void for_each(F func)
    {
        for (auto &item : *this) {
            func(item);
        }
    }
};

// template <class T, list_node T::*link_field>
// std::ostream &operator<<(std::ostream &os, list<T, link_field> &list)
// {
//     os << "[";
//     bool first = true;
//     for (auto &item : list) {
//         if (!first) {
//             os << ", ";
//         }
//         os << item;
//         first = false;
//     }
//     os << "]";
//     return os;
// }

}

