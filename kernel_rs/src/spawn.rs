use alloc::boxed::Box;
use core::ffi::c_void;

extern "C" {
    fn kthread_create(func: unsafe extern "C" fn(*mut c_void), data: *mut c_void);
    fn kthread_exit(exit_code: i32) -> !;
}

pub fn spawn<F>(callback: F)
where
    F: Fn()
{
    unsafe extern "C" fn calling_fn<F: Fn()>(ptr: *mut c_void) {
        let callback = core::mem::transmute::<_, Box<F>>(Box::from_raw(ptr));
        callback();
        kthread_exit(0);
    }
    unsafe { 
        let ptr = Box::into_raw(Box::new(callback)) as *mut c_void;
        kthread_create(calling_fn::<F>, ptr);
    }
}
