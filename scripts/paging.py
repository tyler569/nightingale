
def p4_index_of(addr):
    return (addr >> 39) & 0o777

def p3_index_of(addr):
    return (addr >> 30) & 0o777

def p2_index_of(addr):
    return (addr >> 21) & 0o777

def p1_index_of(addr):
    return (addr >> 12) & 0o777

def offset_of(addr):
    return addr & 0xFFF

x = 0xffff_ffff_8010_0000
x = 0xffffffffc003a000

def indexes(v):
    return (
        p4_index_of(v),
        p3_index_of(v),
        p2_index_of(v),
        p1_index_of(v),
        offset_of(v)
    )

print(indexes(x))
