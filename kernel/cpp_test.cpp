#include "nx/optional.h"
#include <assert.h>
#include <ng/thread.h>
#include <nx/atomic.h>
#include <nx/functional.h>
#include <nx/list.h>
#include <nx/memory.h>
#include <nx/pair.h>
#include <nx/print.h>
#include <nx/string.h>
#include <nx/unordered_map.h>
#include <nx/vector.h>

template <int N> class destruction_tracker {
public:
    static int constructed_count;
    static int copied_count;
    static int moved_count;
    static int destructed_count;

    bool moved_from { false };
    bool destructed { false };

    destruction_tracker() { constructed_count++; }
    destruction_tracker(const destruction_tracker &)
    {
        assert(!destructed);
        assert(!moved_from);
        constructed_count++;
        copied_count++;
    }
    destruction_tracker(destruction_tracker &&)
    {
        assert(!destructed);
        moved_count++;
        moved_from = true;
    }
    destruction_tracker &operator=(const destruction_tracker &other)
    {
        assert(!destructed);
        assert(!moved_from);
        if (this != &other) {
            constructed_count++;
            copied_count++;
        }
        return *this;
    }
    destruction_tracker &operator=(destruction_tracker &&)
    {
        assert(!destructed);
        moved_count++;
        moved_from = true;
        return *this;
    }
    ~destruction_tracker()
    {
        if (!moved_from)
            destructed_count++;
        destructed = true;
    }

    static void print_counts()
    {
        nx::print("tester %: constructs % copies % moves % destructs %\n", N,
            constructed_count, copied_count, moved_count, destructed_count);
    }

    static void assert_counts(int c, int m, int d)
    {
        if (constructed_count != c || moved_count != m
            || destructed_count != d) {
            nx::print("FAIL: ");
            print_counts();
            assert(false);
        }
    }
};

template <int N> int destruction_tracker<N>::constructed_count = 0;
template <int N> int destruction_tracker<N>::copied_count = 0;
template <int N> int destruction_tracker<N>::moved_count = 0;
template <int N> int destruction_tracker<N>::destructed_count = 0;

class list_tester {
public:
    int m_value;
    nx::list_node link {};

    explicit constexpr list_tester(int value)
        : m_value(value)
    {
    }
};

class mutex {
    nx::atomic<char> m_lock { 0 };

public:
    void lock()
    {
        while (m_lock.exchange(1, nx::memory_order_acquire) == 1) {
            __asm__ volatile("pause");
        }
    }

    void unlock() { m_lock.store(0, nx::memory_order_release); }
};

// test from cppreference.com
template <typename T, typename U>
constexpr bool is_decay_equ = nx::is_same_v<nx::decay_t<T>, U>;
static_assert(is_decay_equ<int, int> && !is_decay_equ<int, float>
    && is_decay_equ<int &, int> && is_decay_equ<int &&, int>
    && is_decay_equ<const int &, int> && is_decay_equ<int[2], int *>
    && !is_decay_equ<int[4][2], int *> && !is_decay_equ<int[4][2], int **>
    && is_decay_equ<int[4][2], int (*)[2]>
    && is_decay_equ<int(int), int (*)(int)>);

template <class T> void print_type() { nx::print("%\n", __PRETTY_FUNCTION__); }

void cpp_test()
{
    nx::string str = "Hello world!";
    assert(str.length() == 12);
    assert(str[0] == 'H');
    assert(str[11] == '!');
    assert(str[12] == '\0');
    assert(strcmp(str.c_str(), "Hello world!") == 0);

    nx::string_view sv = str;
    assert(sv.length() == 12);
    assert(sv[0] == 'H');
    assert(sv[11] == '!');
    assert(sv[12] == '\0');
    assert(strcmp(sv.c_str(), "Hello world!") == 0);

    nx::vector<int> vec;
    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }
    assert(vec.size() == 10);
    assert(vec[0] == 0);
    assert(vec[9] == 9);

    nx::list<list_tester, &list_tester::link> lst;
    for (int i = 0; i < 10; i++) {
        lst.push_back(*new list_tester(i));
    }
    assert(lst.size() == 10);
    for (int i = 0; i < 10; i++) {
        assert(lst[i].m_value == i);
    }

    {
        mutex m;
        m.lock();
        m.unlock();
    }

    {
        auto x = nx::make_unique<destruction_tracker<1>>();
    }
    destruction_tracker<1>::assert_counts(1, 0, 1);

    {
        auto x = nx::make_unique<destruction_tracker<2>>();
        auto y = nx::make_unique<destruction_tracker<2>>();
    }
    destruction_tracker<2>::assert_counts(2, 0, 2);

    {
        auto f = nx::function([&](destruction_tracker<3> &&d) {});
        f(destruction_tracker<3> {});
        f(destruction_tracker<3> {});
    }
    destruction_tracker<3>::assert_counts(2, 0, 2);

    {
        auto list_tester = new destruction_tracker<4>[3];
        delete[] list_tester;
    }
    destruction_tracker<4>::assert_counts(3, 0, 3);

    {
        auto s = nx::make_shared<destruction_tracker<5>>();
        auto t = s;
        auto u = s;
    }
    destruction_tracker<5>::assert_counts(1, 0, 1);

    {
        auto f = nx::function([&](destruction_tracker<6> d) {});
        f(destruction_tracker<6> {});
        f(destruction_tracker<6> {});
    }
    destruction_tracker<6>::assert_counts(2, 2, 2);

    {
        auto f = nx::function([&](const destruction_tracker<7> &d) {});
        f(destruction_tracker<7> {});
        f(destruction_tracker<7> {});
    }
    destruction_tracker<7>::assert_counts(2, 0, 2);

    {
        auto f = nx::function([&](destruction_tracker<8> d) {});
        auto dt = destruction_tracker<8> {};
        f(dt);
    }
    destruction_tracker<8>::assert_counts(1, 1, 1);

    {
        auto f = nx::function([&](const destruction_tracker<9> &d) {});
        auto dt = destruction_tracker<9> {};
        f(dt);
    }
    destruction_tracker<9>::assert_counts(1, 0, 1);

    {
        auto f = nx::function([&](destruction_tracker<10> &&d) {});
        auto dt = destruction_tracker<10> {};
        f(nx::move(dt));
    }
    destruction_tracker<10>::assert_counts(1, 0, 1);

    nx::unordered_map<int, int> map;
    for (int i = 0; i < 10; i++) {
        map[i] = i;
    }
    assert(map.size() == 10);
    for (int i = 0; i < 10; i++) {
        assert(map[i] == i);
    }
    for (auto &[key, value] : map) {
        assert(key == value);
    }

    nx::string s = "Hello world!";
    nx::string t = "Hello world~";
    assert(s == s);
    assert(s != t);
    assert(s < t);
    assert(s <= t);
    assert(t > s);
    assert(t >= s);

    nx::print("reverse iterator: ");
    for (auto v = s.rbegin(); v != s.rend(); v++) {
        nx::print("%", *v);
    }
    nx::print("\n");

    auto f1 = nx::function([] { nx::print("the wrong function\n"); });
    f1 = [] { nx::print("the right function\n"); };
    f1();

    auto f2 = nx::optional<nx::function<void()>> {};
    f2 = [] { nx::print("the right function\n"); };
    f2.value()();

    nx::print("nx: all tests passed!\n");
}
