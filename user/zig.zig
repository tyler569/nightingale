// This file is not built with nightingale by default because not everyone
// will have a zig environment easily available. To build this test program
// run `make` in the project root, then run
// zig build-exe -target x86_64-freestanding -Isysroot/usr/include user/zig.zig --output-dir sysroot/usr/bin
// then, `make rawinit`, `make`, and run.

const c = @cImport({
    @cInclude("ng/syscall_consts.h");
    @cInclude("errno.h");
});

const ng_syscall = c.ng_syscall;
const ng_errno = c.errno_value;
const sysret = usize;

const ng_error = error {
    EINVAL,
    EAGAIN,
    ENOEXEC,
    ENOENT,
    EAFNOSUPPORT,
    EPROTONOSUPPORT,
    ECHILD,
    EPERM,
    EFAULT,
    EBADF,
    ERANGE,
    EDOM,
    EACCES,
    ESPIPE,
    EISDIR,
    ENOMEM,
    EINTR,
    ESRCH,
    ETODO,
};

fn sysret_to_error(v: sysret) ng_error!usize {
    if (v < 0xFFFF_FFFF_FFFF_0000) {
        return v;
    }
    const errno_num = -@bitCast(isize, v);
    const errno_c_num = @intCast(c_int, errno_num);
    const errno_v = @intToEnum(ng_errno, errno_c_num);
    return switch (errno_v) {
        ng_errno.SUCCESS => 0,
        ng_errno.EINVAL => error.EINVAL,
        ng_errno.EAGAIN => error.EAGAIN,
        ng_errno.ENOEXEC => error.ENOEXEC,
        ng_errno.ENOENT => error.ENOENT,
        ng_errno.EAFNOSUPPORT => error.EAFNOSUPPORT,
        ng_errno.EPROTONOSUPPORT => error.EPROTONOSUPPORT,
        ng_errno.ECHILD => error.ECHILD,
        ng_errno.EPERM => error.EPERM,
        ng_errno.EFAULT => error.EFAULT,
        ng_errno.EBADF => error.EBADF,
        ng_errno.ERANGE => error.ERANGE,
        ng_errno.EDOM => error.EDOM,
        ng_errno.EACCES => error.EACCES,
        ng_errno.ESPIPE => error.ESPIPE,
        ng_errno.EISDIR => error.EISDIR,
        ng_errno.ENOMEM => error.ENOMEM,
        ng_errno.EINTR => error.EINTR,
        ng_errno.ESRCH => error.ESRCH,
        ng_errno.ETODO => error.ETODO,
        _ => error.EFAULT,
    };
}

fn ng_syscall1(num: ng_syscall, a1: usize) ng_error!usize {
    var ret: sysret = undefined;
    asm volatile (
        "int $0x80"
        : [ret] "={rax}" (ret)
        : [num] "{rax}" (num),
          [a1]  "{rdi}" (a1)
    );
    return sysret_to_error(ret);
}

fn ng_syscall3(num: ng_syscall, a1: usize, a2: usize, a3: usize) ng_error!usize {
    var ret: sysret = undefined;
    asm volatile (
        "int $0x80"
        : [ret] "={rax}" (ret)
        : [num] "{rax}" (num),
          [a1]  "{rdi}" (a1),
          [a2]  "{rsi}" (a2),
          [a3]  "{rdx}" (a3)
    );
    return sysret_to_error(ret);
}

fn ng_exit(code: i32) noreturn {
    const code_u = @intCast(usize, code);
    _ = ng_syscall1(ng_syscall.NG_EXIT, code_u) catch unreachable;
    unreachable;
}

const ng_stdout = 1;

fn ng_write(fd: i32, string: []const u8) ng_error!usize {
    const fd_u = @intCast(usize, fd);
    const pt_u = @ptrToInt(&string[0]);
    return ng_syscall3(ng_syscall.NG_WRITE, fd_u, pt_u, string.len);
}

pub fn main() void {
    _ = ng_write(ng_stdout, "Hello World\n") catch unreachable;
    ng_exit(0);
}

export fn _start() void {
    main();
    unreachable;
}
