unsafe extern "C" {
    pub fn c_panic() -> !;

    pub fn print_view(string: *const i8, len: usize) -> i32;
}
