use core::ffi::c_void;

#[no_mangle]
pub unsafe extern fn sys_rs_read(_file_descriptor: i64, _buf: *mut c_void, _length: usize) -> i64 {
    todo!();
}

#[no_mangle]
pub unsafe extern fn sys_rs_write(_file_descriptor: i64, _buf: *const c_void, _length: usize) -> i64 {
    todo!();
}
