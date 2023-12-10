#pragma once
#ifndef NX_STRING_H
#define NX_STRING_H

#include "reverse_iterator.h"
#include <ng/common.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace nx {

namespace {
    constexpr size_t str_len(const char *str)
    {
        size_t len = 0;
        while (str[len] != '\0')
            len++;
        return len;
    }
}

constexpr size_t npos = SIZE_MAX;

class string {
    size_t m_len { 0 };
    size_t m_cap { 0 };
    char *m_str { nullptr };

public:
    string() = default;

    constexpr string(const char *str) // NOLINT (implicit)
        : m_len(str_len(str))
        , m_cap(m_len + 1)
        , m_str((char *)malloc(m_cap))
    {
        memcpy(m_str, str, m_len + 1);
    }

    constexpr string(const string &other)
        : m_len(other.m_len)
        , m_cap(other.m_cap)
        , m_str((char *)malloc(m_cap))
    {
        memcpy(m_str, other.m_str, m_len + 1);
    }

    constexpr string(string &&other) noexcept
        : m_len(other.m_len)
        , m_cap(other.m_cap)
        , m_str(other.m_str)
    {
        other.m_len = 0;
        other.m_cap = 0;
        other.m_str = nullptr;
    }

    constexpr string &operator=(const string &other)
    {
        if (this == &other)
            return *this;

        m_len = other.m_len;
        m_cap = other.m_cap;
        m_str = (char *)malloc(m_cap);
        memcpy(m_str, other.m_str, m_len + 1);
        return *this;
    }

    constexpr string &operator=(string &&other) noexcept
    {
        if (this == &other)
            return *this;

        m_len = other.m_len;
        m_cap = other.m_cap;
        m_str = other.m_str;
        other.m_len = 0;
        other.m_cap = 0;
        other.m_str = nullptr;
        return *this;
    }

    constexpr ~string() { free(m_str); }

    [[nodiscard]] size_t len() const { return m_len; }
    [[nodiscard]] size_t cap() const { return m_cap; }

    void maybe_expand(size_t at_least)
    {
        if (m_cap >= at_least)
            return;
        auto expand_to = MAX(m_cap * 2, at_least);

        m_cap = expand_to;
        m_str = (char *)realloc(m_str, m_cap);
    }

    string &append(const char *str, size_t len)
    {
        maybe_expand(m_len + len + 1);
        memcpy(m_str + m_len, str, len);
        m_len += len;
        m_str[m_len] = '\0';
        return *this;
    }

    string &append(const char *str) { return append(str, strlen(str)); }
    string &append(const string &str) { return append(str.m_str, str.m_len); }

    string &append(char c)
    {
        maybe_expand(m_len + 2);
        m_str[m_len++] = c;
        m_str[m_len] = '\0';
        return *this;
    }

    string &append(size_t count, char c)
    {
        maybe_expand(m_len + count + 1);
        memset(m_str + m_len, c, count);
        m_len += count;
        m_str[m_len] = '\0';
        return *this;
    }

    string &operator+=(const char *str) { return append(str); }
    string &operator+=(const string &str) { return append(str); }
    string &operator+=(char c) { return append(c); }

    string operator+(const char *str) const
    {
        string result = *this;
        result.append(str);
        return result;
    }

    string operator+(const string &str) const
    {
        string result = *this;
        result.append(str);
        return result;
    }

    string operator+(char c) const
    {
        string result = *this;
        result.append(c);
        return result;
    }

    using iterator = char *;
    using const_iterator = const char *;
    using reverse_iterator = nx::reverse_iterator<iterator>;
    using const_reverse_iterator = nx::reverse_iterator<const_iterator>;

    [[nodiscard]] iterator begin() const { return m_str; }
    [[nodiscard]] iterator end() const { return m_str + m_len; }
    [[nodiscard]] const_iterator cbegin() const { return m_str; }
    [[nodiscard]] const_iterator cend() const { return m_str + m_len; }
    [[nodiscard]] reverse_iterator rbegin() const
    {
        return reverse_iterator { end() };
    }
    [[nodiscard]] reverse_iterator rend() const
    {
        return reverse_iterator { begin() };
    }
    [[nodiscard]] const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator { cend() };
    }
    [[nodiscard]] const_reverse_iterator crend() const
    {
        return const_reverse_iterator { cbegin() };
    }

    [[nodiscard]] constexpr const char *c_str() const { return m_str; }

    constexpr char operator[](size_t index) { return m_str[index]; }

    constexpr int operator<=>(const string &other) const
    {
        return strcmp(m_str, other.m_str);
    }

    constexpr bool operator==(const string &other) const
    {
        return m_len == other.m_len && memcmp(m_str, other.m_str, m_len) == 0;
    }

    [[nodiscard]] string substr(size_t start, size_t len) const
    {
        string result;
        result.maybe_expand(len + 1);
        memcpy(result.m_str, m_str + start, len);
        result.m_len = len;
        result.m_str[len] = '\0';
        return result;
    }

    [[nodiscard]] string substr(size_t start) const
    {
        return substr(start, m_len - start);
    }

    [[nodiscard]] size_t find(char c) const
    {
        for (size_t i = 0; i < m_len; i++) {
            if (m_str[i] == c)
                return i;
        }
        return npos;
    }

    [[nodiscard]] size_t find(const string &str) const
    {
        for (size_t i = 0; i < m_len; i++) {
            if (memcmp(m_str + i, str.m_str, str.m_len) == 0)
                return i;
        }
        return npos;
    }

    [[nodiscard]] size_t find(const char *str) const
    {
        return find(string(str));
    }

    [[nodiscard]] size_t rfind(char c) const
    {
        for (size_t i = m_len; i > 0; i--) {
            if (m_str[i - 1] == c)
                return i - 1;
        }
        return npos;
    }

    [[nodiscard]] size_t rfind(const string &str) const
    {
        for (size_t i = m_len; i > 0; i--) {
            if (memcmp(m_str + i - 1, str.m_str, str.m_len) == 0)
                return i - 1;
        }
        return npos;
    }

    [[nodiscard]] size_t rfind(const char *str) const
    {
        return rfind(string(str));
    }

    void clear() { m_len = 0; }
    [[nodiscard]] bool empty() const { return m_len == 0; }
    [[nodiscard]] size_t size() const { return m_len; }
    [[nodiscard]] size_t length() const { return m_len; }
    [[nodiscard]] size_t max_size() const { return SIZE_MAX; }
    void reserve(size_t new_cap) { maybe_expand(new_cap); }
    [[nodiscard]] size_t capacity() const { return m_cap; }
    void shrink_to_fit() { maybe_expand(m_len + 1); }

    // friend std::ostream &operator<<(std::ostream &os, const string &str)
    // {
    //     os.write(str.m_str, static_cast<long>(str.m_len));
    //     return os;
    // }
};

class string_view {
    const char *m_str;
    size_t m_len;

public:
    constexpr string_view(const char *str, size_t len)
        : m_str(str)
        , m_len(len)
    {
    }

    constexpr string_view(const char *str) // NOLINT (implicit)
        : m_str(str)
        , m_len(str_len(str))
    {
    }

    constexpr string_view(const string &str) // NOLINT (implicit)
        : m_str(str.cbegin())
        , m_len(str.len())
    {
    }

    constexpr string_view(const string_view &other) // NOLINT (implicit)
        : m_str(other.m_str)
        , m_len(other.m_len)
    {
    }

    constexpr string_view(string_view &&other) noexcept
        : m_str(other.m_str)
        , m_len(other.m_len)
    {
    }

    constexpr string_view &operator=(const string_view &other)
    {
        if (this == &other)
            return *this;

        m_str = other.m_str;
        m_len = other.m_len;
        return *this;
    }

    constexpr string_view &operator=(string_view &&other) noexcept
    {
        if (this == &other)
            return *this;

        m_str = other.m_str;
        m_len = other.m_len;
        return *this;
    }

    [[nodiscard]] constexpr size_t len() const { return m_len; }
    [[nodiscard]] constexpr size_t size() const { return m_len; }
    [[nodiscard]] constexpr size_t length() const { return m_len; }
    [[nodiscard]] constexpr bool empty() const { return m_len == 0; }
    [[nodiscard]] constexpr const char *c_str() const { return m_str; }
    [[nodiscard]] constexpr const char *data() const { return m_str; }

    constexpr char operator[](size_t index) const { return m_str[index]; }

    constexpr int operator<=>(const string_view &other) const
    {
        if (m_len < other.m_len)
            return -1;
        if (m_len > other.m_len)
            return 1;
        return memcmp(m_str, other.m_str, m_len);
    }

    [[nodiscard]] constexpr string_view substr(size_t start, size_t len) const
    {
        return { m_str + start, len };
    }

    [[nodiscard]] constexpr string_view substr(size_t start) const
    {
        return substr(start, m_len - start);
    }

    [[nodiscard]] constexpr size_t find(char c) const
    {
        for (size_t i = 0; i < m_len; i++) {
            if (m_str[i] == c)
                return i;
        }
        return npos;
    }

    using iterator = const char *;
    using const_iterator = const char *;
    using reverse_iterator = nx::reverse_iterator<iterator>;
    using const_reverse_iterator = nx::reverse_iterator<const_iterator>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = char;
    using pointer = const char *;

    static constexpr size_type npos = SIZE_MAX;

    [[nodiscard]] constexpr iterator begin() const { return m_str; }
    [[nodiscard]] constexpr iterator end() const { return m_str + m_len; }
    [[nodiscard]] constexpr const_iterator cbegin() const { return m_str; }
    [[nodiscard]] constexpr const_iterator cend() const
    {
        return m_str + m_len;
    }
    [[nodiscard]] constexpr reverse_iterator rbegin() const
    {
        return reverse_iterator(end());
    }
    [[nodiscard]] constexpr reverse_iterator rend() const
    {
        return reverse_iterator(begin());
    }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(cend());
    }
    [[nodiscard]] constexpr const_reverse_iterator crend() const
    {
        return const_reverse_iterator(cbegin());
    }

    // friend std::ostream &operator<<(std::ostream &os, const string &str)
    // {
    //     os.write(str.m_str, static_cast<long>(str.m_len));
    //     return os;
    // }
};

}

#ifdef __nx_is_std
namespace std = nx;
#endif

#endif // NX_STRING_H
