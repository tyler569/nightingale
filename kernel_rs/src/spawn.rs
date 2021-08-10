use alloc::boxed::Box;
use core::ffi::c_void;
use crate::sync::Mutex;

extern "C" {
    fn kthread_create(func: unsafe extern "C" fn(*mut c_void), data: *mut c_void) -> *mut c_void;
    fn kthread_exit(exit_code: i32) -> !;
    fn kthread_this() -> *mut c_void;
}

pub fn spawn<F>(callback: F)
where
    F: FnOnce() + Send + 'static,
{
    unsafe extern "C" fn calling_fn<F>(ptr: *mut c_void)
    where
        F: FnOnce() + Send + 'static,
    {
        let callback = Box::from_raw(ptr as *mut F);
        callback();
        kthread_exit(0);
    }

    let ptr = Box::into_raw(Box::new(callback)) as *mut c_void;
    unsafe { kthread_create(calling_fn::<F>, ptr); }
}
