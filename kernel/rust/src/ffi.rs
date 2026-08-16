use core::ffi::c_void;

pub type Thread = c_void;

unsafe extern "C" {
    pub fn c_panic() -> !;

    pub fn print_view(string: *const i8, len: usize) -> i32;

    pub fn kthread_create(f: unsafe extern "C" fn (arg: *mut c_void), arg: *mut c_void) -> *mut Thread;
    pub fn kthread_exit();

    pub fn malloc(size: usize) -> *mut c_void;
    pub fn free(ptr: *mut c_void);
}
