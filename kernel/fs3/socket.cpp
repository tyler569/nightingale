#include <ng/fs3/socket.h>
#include <ng/mt/process.h>
#include <ng/net/pk.h>

namespace fs3 {

ssize_t socket_open_file::read(void *buffer, size_t len)
{
    auto pk = m_inode->pop_pk();
    if (!pk)
        return -EAGAIN;
    if (len > pk->size)
        len = pk->size;
    memcpy(buffer, pk->data, len);
    return static_cast<ssize_t>(len);
}

ssize_t socket_open_file::write(const void *buffer, size_t len)
{
    auto pk = new class pk;
    if (len > sizeof(pk->data))
        len = sizeof(pk->data);
    memcpy(pk->data, buffer, len);
    pk->size = len;
    m_inode->push_pk(*pk);
    return static_cast<ssize_t>(len);
}

int socket_open_file::seek(off_t offset, int whence) { return -EOPNOTSUPP; }

int socket_open_file::close() { return -ETODO; }

socket_open_file *socket_inode::open(int flags, int mode)
{
    return new socket_open_file(this);
}

pk *socket_inode::pop_pk()
{
    if (m_pks.empty())
        return nullptr;
    auto pk = &m_pks.front();
    m_pks.pop_front();
    return pk;
}

void socket_inode::push_pk(pk &pk) { m_pks.push_back(pk); }

}

sysret sys_fs3_socket()
{
    auto inode = new fs3::socket_inode;
    auto open_file = inode->open(0, 0);
    running_thread->proc->m_fd3s.push_back(open_file);
    return running_thread->proc->m_fd3s.size() - 1;
}
