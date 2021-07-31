use alloc::boxed::Box;
use core::ffi::c_void;

extern "C" {
    fn kthread_create(func: unsafe extern "C" fn(*mut c_void), data: *mut c_void) -> *mut c_void;
    fn kthread_exit(exit_code: i32) -> !;

    fn kthread_this() -> *mut c_void;
    fn kthread_set_return(thread: *mut c_void, value: *const c_void);
    fn kthread_get_return(thread: *mut c_void) -> *const c_void;
    fn kthread_inc_wait(thread: *mut c_void);
    fn kthread_dec_wait(thread: *mut c_void);
    fn kthread_wait_raw(thread: *mut c_void);
}

pub struct JoinHandle<T> {
    thread: *mut c_void,
    phantom: core::marker::PhantomData<T>,
}

impl<T> JoinHandle<T> {
    pub fn new(thread: *mut c_void) -> Self {
        JoinHandle {
            thread,
            phantom: core::marker::PhantomData::<T>{},
        }
    }

    pub fn join(self) -> Result<Box<T>, &'static str> {
        // Safety: this is an extern function but is not dangerous.
        // self.thread is not null.
        unsafe { kthread_wait_raw(self.thread) };
        // Safety: this is an extern function but is just an accessor.
        // self.thread is not null.
        let return_value = unsafe { kthread_get_return(self.thread) };
        // Safety: return_value is not null because it is only ever set
        // by `spawn`, which always allocates a Box for the return value
        // of the thread.
        let return_value = unsafe { Box::from_raw(return_value as *mut T) };
        Ok(return_value)
    }
}

impl<T> core::ops::Drop for JoinHandle<T> {
    fn drop(&mut self) {
        // Safety: self.thread is not null because self still exists.
        unsafe { kthread_dec_wait(self.thread) };
    }
}

pub fn spawn<F, T>(callback: F) -> JoinHandle<T>
where
    F: FnOnce() -> T + Send + 'static,
    T: Send + 'static,
{
    unsafe extern "C" fn calling_fn<F, T>(ptr: *mut c_void)
    where
        F: FnOnce() -> T + Send + 'static,
        T: Send + 'static,
    {
        kthread_inc_wait(kthread_this());
        let callback = Box::from_raw(ptr as *mut F);
        let return_value = callback();
        kthread_set_return(kthread_this(), Box::into_raw(Box::new(return_value)) as *mut c_void);
        kthread_exit(0);
    }
    unsafe { 
        let ptr = Box::into_raw(Box::new(callback)) as *mut c_void;
        let thread_ptr = kthread_create(calling_fn::<F, T>, ptr);
        JoinHandle::<T>::new(thread_ptr)
    }
}
