use alloc::boxed::Box;
use core::ffi::c_void;

unsafe extern "C" {
    pub fn kthread_create(f: unsafe extern "C" fn (arg: *mut c_void), arg: *mut c_void) -> *mut c_void;
    pub fn kthread_exit();
}

pub struct Task;

impl Task {
    pub fn spawn<F>(f: F)
    where
        F: FnOnce() + Send + 'static,
    {
        let raw_payload = Box::into_raw(Box::new(f)) as *mut ();

        unsafe {
            kthread_create(task_trampoline::<F>, raw_payload as *mut c_void);
        }
    }
}

unsafe extern "C" fn task_trampoline<F: FnOnce()>(arg: *mut c_void) {
    if arg.is_null() {
        return;
    }

    // Safety: `arg` was created via Box::into_raw for type F
    let closure = unsafe { Box::from_raw(arg as *mut F) };
    closure();
    unsafe {
        kthread_exit();
    }
}
