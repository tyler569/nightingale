#pragma once

#include "nx.h"
#include "utility.h"

namespace NX_STD {

class nullopt_t {
public:
    constexpr nullopt_t() noexcept = default;
};

template <class T> class optional {
    bool m_has_value;
    union {
        T m_value;
        char m_dummy;
    };

public:
    constexpr optional() noexcept
        : m_has_value(false)
        , m_dummy(0)
    {
    }

    constexpr optional(nullopt_t) noexcept // NOLINT (explicit)
        : m_has_value(false)
        , m_dummy(0)
    {
    }

    constexpr ~optional() noexcept
    {
        if (m_has_value) {
            m_value.~T();
        }
    }

    constexpr optional(const T &value) noexcept // NOLINT (explicit)
        : m_has_value(true)
        , m_value(value)
    {
    }

    constexpr optional(T &&value) noexcept // NOLINT (explicit)
        : m_has_value(true)
        , m_value(move(value))
    {
    }

    constexpr optional(const optional &other) noexcept
        : m_has_value(other.m_has_value)
        , m_value(other.m_value)
    {
    }

    constexpr optional(optional &&other) noexcept
        : m_has_value(other.m_has_value)
        , m_value(move(other.m_value))
    {
    }

    constexpr optional &operator=(const optional &other) noexcept
    {
        m_has_value = other.m_has_value;
        m_value = other.m_value;
        return *this;
    }

    constexpr optional &operator=(optional &&other) noexcept
    {
        m_has_value = other.m_has_value;
        m_value = move(other.m_value);
        return *this;
    }

    [[nodiscard]] constexpr bool has_value() const noexcept
    {
        return m_has_value;
    }

    constexpr T &value() noexcept { return m_value; }

    constexpr const T &value() const noexcept { return m_value; }

    constexpr T &operator*() noexcept { return m_value; }

    constexpr const T &operator*() const noexcept { return m_value; }

    constexpr T *operator->() noexcept { return &m_value; }

    constexpr const T *operator->() const noexcept { return &m_value; }

    constexpr operator bool() const noexcept
    {
        return m_has_value;
    } // NOLINT (explicit)

    constexpr void reset() noexcept
    {
        if (m_has_value)
            m_value.~T();
        m_has_value = false;
    }

    constexpr void reset(const T &value) noexcept
    {
        if (m_has_value)
            m_value.~T();
        m_has_value = true;
        m_value = value;
    }

    constexpr void reset(T &&value) noexcept
    {
        if (m_has_value)
            m_value.~T();
        m_has_value = true;
        m_value = move(value);
    }

    template <class... Args> constexpr void emplace(Args &&...args) noexcept
    {
        if (m_has_value)
            m_value.~T();
        m_has_value = true;
        m_value = T(forward<Args>(args)...);
    }

    template <class U> constexpr T value_or(U &&default_value) const noexcept
    {
        if (m_has_value) {
            return m_value;
        } else {
            return default_value;
        }
    }

    template <class U> constexpr T value_or(U &&default_value) noexcept
    {
        if (m_has_value) {
            return m_value;
        } else {
            return default_value;
        }
    }
};

constexpr nullopt_t nullopt;

}
