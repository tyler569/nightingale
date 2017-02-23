
const VGA: *mut u16 = 0xB8000 as *mut u16;

pub fn raw_print(offset: isize, color: u8, message: &[u8]) {
    for (i, c) in message.iter().enumerate() {
        unsafe {
            *VGA.offset(offset + (i as isize)) = (*c as u16) | ((color as u16) << 8);
        }
    }
}

pub fn raw_print_num(offset: isize, color: u8, value: u64) {
    for i in 0..16 {
        unsafe {
            *VGA.offset(offset + i) = match (value >> (60 - 4 * i) & 0xF) as u16 {
                n @ 0x0...0x9 => n + b'0' as u16,
                n @ 0xA...0xF => n - 0xA + b'A' as u16,
                _ => panic!(),
            } | (color as u16) << 8;
        }
    }
}


