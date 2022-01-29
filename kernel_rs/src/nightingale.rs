extern "C" {
    pub fn malloc(size: usize) -> *mut u8;
    pub fn free(ptr: *mut u8);

    pub fn raw_print(file: *const (), buf: *const u8, len: usize) -> i32;
}
