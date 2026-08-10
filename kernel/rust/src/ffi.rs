unsafe extern "C" {
    pub fn panic(message: *const i8) -> !;

    pub fn printf(message: *const i8, ...) -> i32;
}
