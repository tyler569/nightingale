#include <ng/mt/sync.h>

void sync_mt_test()
{
    mt_mutex m;
    m.lock();
    m.unlock();
}