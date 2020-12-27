#
# gdb helper commands and functions for Linux kernel debugging
# modified and extended for Nightingale kernel debugging, 2020
#
#  common utilities
#
# Copyright (c) Siemens AG, 2011-2013
#               Tyler Philbrick 2020
#
# Authors:
#  Jan Kiszka <jan.kiszka@siemens.com>
#  Tyler Philbrick <github@tylerphilbrick.com>
#
# This work is licensed under the terms of the GNU GPL version 2.
#

import gdb


class CachedType:
    def __init__(self, name):
        self._type = None
        self._name = name

    def _new_objfile_handler(self, event):
        self._type = None
        gdb.events.new_objfile.disconnect(self._new_objfile_handler)

    def get_type(self):
        if self._type is None:
            self._type = gdb.lookup_type(self._name)
            if self._type is None:
                raise gdb.GdbError(
                    "cannot resolve type '{0}'".format(self._name))
            if hasattr(gdb, 'events') and hasattr(gdb.events, 'new_objfile'):
                gdb.events.new_objfile.connect(self._new_objfile_handler)
        return self._type


long_type = CachedType("long")


def get_long_type():
    global long_type
    return long_type.get_type()


def offset_of(typeobj, field):
    element = gdb.Value(0).cast(typeobj)
    return int(str(element[field].address).split()[0], 16)

# `typeobj` must be the final pointer type that is returned
def container_of(ptr, typeobj, member):
    return (ptr.cast(get_long_type()) -
            offset_of(typeobj, member)).cast(typeobj)

# wrapper for `container_of` that takes gdb parameters as expected by the
# `$container_of` function, and etc.
def rcontainer_of(ptr, raw_type, raw_member):
    return container_of(ptr,
            gdb.lookup_type(raw_type.string()).pointer(),
            raw_member.string())


class ContainerOf(gdb.Function):
    """Return pointer to containing data structure.

$container_of(PTR, "TYPE", "ELEMENT"): Given PTR, return a pointer to the
data structure of the type TYPE in which PTR is the address of ELEMENT.
Note that TYPE and ELEMENT have to be quoted as strings."""

    def __init__(self):
        super(ContainerOf, self).__init__("container_of")

    def invoke(self, ptr, typename, elementname):
        return rcontainer_of(ptr, typename, elementname)

ContainerOf()

list_type = CachedType("struct list")

def list_head(head):
    if head.type == list_type.get_type().pointer():
        head = head.dereference()
    elif head.type != list_type.get_type():
        raise TypeError("Must be struct list not {}".format(head.type))

    return head['next']

def list_for_each(head):
    if head.type == list_type.get_type().pointer():
        head = head.dereference()
    elif head.type != list_type.get_type():
        raise TypeError("Must be struct list not {}".format(head.type))

    node = head['next'].dereference()
    while node.address != head.address:
        yield node.address
        node = node['next'].dereference()

def list_for_each_entry(head, gdbtype, member):
    for node in list_for_each(head):
        yield container_of(node, gdbtype, member)

def rlist_for_each_entry(head, raw_type, raw_member):
    gdbtype = gdb.lookup_type(raw_type.string()).pointer()
    member = raw_member.string()
    
    for node in list_for_each(head):
        yield container_of(node, gdbtype, member)

class ListHead(gdb.Function):
    def __init__(self):
        super(ListHead, self).__init__("list_head")

    def invoke(self, ptr, typename, elementname):
        ptr = list_head(ptr)
        return rcontainer_of(ptr, typename, elementname)

ListHead()

class PrintList(gdb.Function):
    def __init__(self):
        super(PrintList, self).__init__("print_list")

    def invoke(self, head, type, member):
        for i, memb in enumerate(rlist_for_each_entry(head, type, member)):
            gdb.write("$list{} = {}\n".format(i, memb))
            gdb.set_convenience_variable("list{}".format(i), memb)
        return 0

PrintList()
