use core::ffi::c_void;
use crate::file;
use crate::file::FileDescription;

fn get_file_description(file_descriptor: i64) -> FileDescription {
    todo!()
}

fn syscall_file_result_to_error_code<T>(result: file::Result<T>) -> i64 {
    todo!()
}

#[no_mangle]
pub unsafe extern fn sys_rs_read(file_descriptor: i64, buf: *mut c_void, length: usize) -> i64 {
    let buf = core::slice::from_raw_parts_mut(buf as *mut u8, length as usize);
    let mut fd = get_file_description(file_descriptor);
    let file = fd.file.clone();
    let result = file.read(&mut fd, buf);
    syscall_file_result_to_error_code(result)
}

#[no_mangle]
pub unsafe extern fn sys_rs_write(file_descriptor: i64, buf: *const c_void, length: usize) -> i64 {
    let buf = core::slice::from_raw_parts(buf as *const u8, length as usize);
    let mut fd = get_file_description(file_descriptor);
    let file = fd.file.clone();
    let result = file.write(&mut fd, buf);
    syscall_file_result_to_error_code(result)
}
