
def vma_to_table_indexes(vma):
    offset = vma & 0xfff
    vma >>= 12
    p1_index = vma & 0x1ff
    vma >>= 9
    p2_index = vma & 0x1ff
    vma >>= 9
    p3_index = vma & 0x1ff
    vma >>= 9
    p4_index = vma & 0x1ff

    return (p4_index, p3_index, p2_index, p1_index, offset)

def table_indexes_to_vma(offsets):
    if (offsets[0] > 255):
        vma = 0xffff000000000000
    else:
        vma = 0
    vma |= offsets[0] << 39
    vma |= offsets[1] << 30
    vma |= offsets[2] << 21
    vma |= offsets[3] << 12
    if (len(offsets) > 4):
        vma |= offsets[4]

    return vma

def recursive_map(vma, rec_index):
    p4_e, p3_e, p2_e, p1_e, off = vma_to_table_indexes(vma)

    pml4 = table_indexes_to_vma((rec_index, rec_index, rec_index, rec_index))
    pdpt = table_indexes_to_vma((rec_index, rec_index, rec_index, p4_e))
    pd = table_indexes_to_vma((rec_index, rec_index, p4_e, p3_e))
    pt = table_indexes_to_vma((rec_index, p4_e, p3_e, p2_e))

    return (pml4, pdpt, pd, pt)

def main():
    print("Test:")
    vma = 0xffff804020100000
    print(vma)
    print(hex(vma))
    print(oct(vma))
    o = vma_to_table_indexes(vma)
    print(o)
    print([hex(i) for i in o])
    print([oct(i) for i in o])
    v = table_indexes_to_vma(o)
    print(v)
    print(hex(v))
    print(oct(v))

    print("Ok really now, this is my vma:")
    print(hex(table_indexes_to_vma((510, 0, 0, 0))))

    print("Recursive map for kernel:")

    p4, p3, p2, p1 = recursive_map(0xffffffff80100000, 0x100)
    print(hex(p4))
    print(hex(p3))
    print(hex(p2))
    print(hex(p1))

    print("Fork recursive map for kernel:")
    p4, p3, p2, p1 = recursive_map(0xffffffff80100000, 0x101)
    print(hex(p4))
    print(hex(p3))
    print(hex(p2))
    print(hex(p1))

if __name__ == "__main__":
    main()
