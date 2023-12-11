#pragma once

#include "list.h"
#include "nx.h"
#include "pair.h"
#include "vector.h"
#include <stddef.h>

namespace NX_STD {

template <class T> class hasher {
    static constexpr size_t FNV_OFFSET_BASIS = 14695981039346656037ul;
    static constexpr size_t FNV_PRIME = 1099511628211ul;

    static size_t hash(const char *str, size_t len)
    {
        size_t hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < len; i++) {
            hash ^= str[i];
            hash *= FNV_PRIME;
        }
        return hash;
    }

public:
    static size_t operator()(const T &value)
    {
        return hasher::hash(reinterpret_cast<const char *>(&value), sizeof(T));
    }
};

template <class K, class V, class H = hasher<K>> class unordered_map {
public:
    unordered_map() = default;
    unordered_map(const unordered_map &other) = default;
    unordered_map(unordered_map &&other) = default;
    unordered_map &operator=(const unordered_map &other) = default;
    unordered_map &operator=(unordered_map &&other) = default;
    ~unordered_map() = default;

    V &operator[](const K &key) { return find_node(key)->pair.second; }

    V &operator[](K &&key) { return find_node(key)->pair.second; }

    void insert(const K &key, const V &value)
    {
        find_node(key)->pair.second = value;
    }

    void insert(K &&key, V &&value)
    {
        find_node(key)->pair.second = move(value);
    }

    void insert(const K &key, V &&value)
    {
        find_node(key)->pair.second = move(value);
    }

    void insert(K &&key, const V &value)
    {
        find_node(key)->pair.second = value;
    }

    void insert(const unordered_map &other)
    {
        for (auto &pair : other) {
            insert(pair.first, pair.second);
        }
    }

    void insert(unordered_map &&other)
    {
        for (auto &pair : other) {
            insert(move(pair.first), move(pair.second));
        }
    }

    size_t size() const { return m_size; }
    bool empty() const { return m_size == 0; }

    void clear()
    {
        m_buckets.clear();
        m_size = 0;
    }

    void erase(const K &key)
    {
        auto &bucket = m_buckets[bucket_index(key)];
        for (auto it = bucket.begin(); it != bucket.end(); it++) {
            if (it->pair.first == key) {
                bucket.remove(it);
                m_size--;
                return;
            }
        }
    }

    void erase(const K &key, const V &value)
    {
        auto &bucket = m_buckets[bucket_index(key)];
        for (auto it = bucket.begin(); it != bucket.end(); it++) {
            if (it->pair.first == key && it->pair.second == value) {
                bucket.remove(it);
                m_size--;
                return;
            }
        }
    }

private:
    struct node {
        std::pair<const K, V> pair;
        nx::list_node list_node {};

        node(K &&key, V &&value)
            : pair(std::pair<K, V>(move(key), move(value)))
        {
        }

        node(const K &key, const V &value)
            : pair(key, value)
        {
        }
    };

    std::vector<nx::list<node, &node::list_node>> m_buckets {};
    size_t m_size { 0 };

    size_t bucket_index(const K &key) const
    {
        return H()(key) % m_buckets.size();
    }

    size_t bucket_index(const K &key, size_t bucket_count) const
    {
        return H()(key) % bucket_count;
    }

    node *find_node(const K &key)
    {
        consider_rehash();
        auto &bucket = m_buckets[bucket_index(key)];
        for (auto &node : bucket) {
            if (node.pair.first == key) {
                return &node;
            }
        }
        auto *new_node = new node { key, V {} };
        bucket.push_back(*new_node);
        m_size++;
        return new_node;
    }

    const node *find_node(const K &key) const
    {
        consider_rehash();
        auto &bucket = m_buckets[bucket_index(key)];
        for (auto &node : bucket) {
            if (node.pair.first == key) {
                return &node;
            }
        }
        return nullptr;
    }

    void consider_rehash()
    {
        if (m_buckets.size() == 0) {
            rehash(16);
            return;
        }

        if (m_size > m_buckets.size() * 2) {
            rehash(m_buckets.size() * 2);
        }
    }

    void rehash(size_t new_size)
    {
        vector<nx::list<node, &node::list_node>> new_buckets(new_size);
        for (auto &bucket : m_buckets) {
            for (auto &node : bucket) {
                auto &new_bucket
                    = new_buckets[bucket_index(node.pair.first, new_size)];
                new_bucket.push_back(node);
            }
        }
        m_buckets = move(new_buckets);
    }

public:
    class iterator {
    public:
        explicit iterator(unordered_map &map)
            : m_map(&map)
            , m_list_iterator(nullptr)
        {
            for (size_t i = 0; i < m_map->m_buckets.size(); i++) {
                if (!m_map->m_buckets[i].empty()) {
                    m_bucket_index = i;
                    m_list_iterator = m_map->m_buckets[i].begin();
                    return;
                }
            }
            m_bucket_index = m_map->m_buckets.size();
        }

        iterator(const iterator &other) = default;
        iterator(iterator &&other) = default;
        iterator &operator=(const iterator &other) = default;
        iterator &operator=(iterator &&other) = default;
        ~iterator() = default;

        iterator &operator++()
        {
            if (++m_list_iterator != m_map->m_buckets[m_bucket_index].end()) {
                return *this;
            }

            for (size_t i = m_bucket_index + 1; i < m_map->m_buckets.size();
                 i++) {
                if (!m_map->m_buckets[i].empty()) {
                    m_bucket_index = i;
                    m_list_iterator = m_map->m_buckets[i].begin();
                    return *this;
                }
            }

            m_bucket_index = m_map->m_buckets.size();
            return *this;
        }

        bool operator==(const iterator &other) const
        {
            return (m_bucket_index == other.m_bucket_index
                       && m_list_iterator == other.m_list_iterator)
                || (m_bucket_index == m_map->m_buckets.size()
                    && other.m_bucket_index == m_map->m_buckets.size());
        }

        std::pair<const K, V> &operator*() const
        {
            return m_list_iterator->pair;
        }

        static iterator end(unordered_map &map)
        {
            iterator it(map);
            it.m_bucket_index = map.m_buckets.size();
            return it;
        }

    private:
        const unordered_map *m_map;
        size_t m_bucket_index { 0 };
        nx::list<node, &node::list_node>::iterator m_list_iterator;
    };

    iterator begin() { return iterator(*this); }
    iterator end() { return iterator::end(*this); }
};

}