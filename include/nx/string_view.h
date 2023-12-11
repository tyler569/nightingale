#pragma once

#include "nx.h"
#include "reverse_iterator.h"
#include "string.h"
#include <stddef.h>
#include <string.h>

namespace NX_STD {

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
    using reverse_iterator = NX_STD::reverse_iterator<iterator>;
    using const_reverse_iterator = NX_STD::reverse_iterator<const_iterator>;
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