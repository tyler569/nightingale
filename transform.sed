
# uint8_t -> u8, etc.
s/uint8_t/u8/g
s/uint16_t/u16/g
s/uint32_t/u32/g
s/uint64_t/u64/g
s/int8_t/i8/g
s/int16_t/i16/g
s/int32_t/i32/g
s/int64_t/i64/g

# pointer-realted to pointer width
s/uintptr_t/usize/g
s/size_t/usize/g
s/intptr_t/isize/g
s/ptrdiff_t/isize/g

# standard C types to mine
s/unsigned long/u64/g
s/long /i64 /g
s/unsigned int/u32/g
s/int /i32 /g
s/unsigned short/u16/g
s/short /i16 /g
s/unsigned char/u8/g
#s/char /i8 /g
