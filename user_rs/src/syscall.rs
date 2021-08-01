pub type CSyscall = usize;

pub const _EXIT: CSyscall = 2;
pub const WRITE: CSyscall = 5;

pub type COsError = usize;

pub const EOTHER: COsError = 4095;

fn return_to_result(return_value: usize) -> Result<usize, COsError> {
    if (return_value as isize) > -4096 && (return_value as isize) < 0 {
        Err(-(return_value as isize) as usize)
    } else {
        Ok(return_value)
    }
}

pub unsafe fn c_syscall0(number: CSyscall) -> Result<usize, COsError> {
    let return_value: usize;
    asm!(
        "int 0x80",
        inlateout("rax") number => return_value);
    return_to_result(return_value)
}

pub unsafe fn c_syscall1(number: CSyscall, arg1: usize) -> Result<usize, COsError> {
    let return_value: usize;
    asm!(
        "int 0x80",
        inlateout("rax") number => return_value,
        in("rdi") arg1);
    return_to_result(return_value)
}

pub unsafe fn c_syscall3(number: CSyscall, arg1: usize, arg2: usize, arg3: usize) -> Result<usize, COsError> {
    let return_value: usize;
    asm!(
        "int 0x80",
        inlateout("rax") number => return_value,
        in("rdi") arg1,
        in("rsi") arg2,
        in("rdx") arg3);
    return_to_result(return_value)
}
