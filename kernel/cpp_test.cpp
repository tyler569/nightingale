#include <assert.h>
#include <ng/thread.h>
#include <nx/atomic.h>
#include <nx/functional.h>
#include <nx/list.h>
#include <nx/memory.h>
#include <nx/print.h>
#include <nx/string.h>
#include <nx/vector.h>

template <int N> class destruction_tracker {
public:
    static int constructed_count;
    static int destructed_count;

    destruction_tracker() { constructed_count++; }
    ~destruction_tracker() { destructed_count++; }
};

template <int N> int destruction_tracker<N>::constructed_count = 0;
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
    nx::atomic<int> m_lock { 0 };

public:
    void lock()
    {
        while (m_lock.exchange(1, nx::memory_order_acquire) == 1) {
            __asm__ volatile("pause");
        }
    }

    void unlock() { m_lock.store(0, nx::memory_order_release); }
};

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
    assert(destruction_tracker<1>::constructed_count == 1);
    assert(destruction_tracker<1>::destructed_count == 1);

    {
        auto x = nx::make_unique<destruction_tracker<2>>();
        auto y = nx::make_unique<destruction_tracker<2>>();
    }
    assert(destruction_tracker<2>::constructed_count == 2);
    assert(destruction_tracker<2>::destructed_count == 2);

    {
        auto f = nx::function([&](destruction_tracker<3> &&d) {});
        f(destruction_tracker<3> {});
        f(destruction_tracker<3> {});
    }
    assert(destruction_tracker<3>::constructed_count == 2);
    assert(destruction_tracker<3>::destructed_count == 2);

    {
        auto list_tester = new destruction_tracker<4>[3];
        delete[] list_tester;
    }
    assert(destruction_tracker<4>::constructed_count == 3);
    assert(destruction_tracker<4>::destructed_count == 3);

    nx::print("nx: all tests passed!\n");
}
