#include <ng/fs3/dentry.h>
#include <ng/fs3/file_system.h>
#include <ng/fs3/inode.h>
#include <ng/fs3/socket.h>
#include <nx/memory.h>
#include <nx/print.h>

namespace fs3 {

void tree(dentry &d, int depth = 0)
{
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("%s\n", d.name());
    for (auto &child : d.children()) {
        tree(child, depth + 1);
    }
}

void test()
{
    auto root = nx::make_unique<dentry>("root");

    auto d1 = nx::make_unique<dentry>("d1", root.get());
    auto d2 = nx::make_unique<dentry>("d2", root.get());
    auto d3 = nx::make_unique<dentry>("d3", root.get());
    auto d11 = nx::make_unique<dentry>("d11", d1.get());
    auto d12 = nx::make_unique<dentry>("d12", d1.get());

    tree(*root);

    nx::print("fs3: all tests passed!\n");
}

}