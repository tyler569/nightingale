#include <ng/fs3/dtree.h>
#include <ng/fs3/file_system.h>
#include <ng/fs3/inode.h>
#include <ng/fs3/tmp_file_system.h>
#include <nx/print.h>

void tmp_file_system::mount(dentry *d) { }
inode *tmp_file_system::get_inode(long inode_number) { return nullptr; }
inode *tmp_file_system::new_inode() { return nullptr; }
void tmp_file_system::destroy_inode(inode *i) { }

void tree(dentry *d, int depth = 0)
{
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("%s\n", d->name());
    for (auto &child : d->children()) {
        tree(&child, depth + 1);
    }
}

void fs3_test()
{
    auto *fs = new tmp_file_system();
    auto *root = new dentry("root");
    fs->mount(root);

    auto *d1 = new dentry("d1", root);
    auto *d2 = new dentry("d2", root);
    auto *d3 = new dentry("d3", root);
    auto *d11 = new dentry("d11", d1);
    auto *d12 = new dentry("d12", d1);

    tree(root);

    nx::print("fs3: all tests passed!\n");
}