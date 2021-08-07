use core::cell::UnsafeCell;
use core::ffi::c_void;
use core::marker::{Send, Sync};
use core::mem;
use core::ops::{Deref, DerefMut, Drop};
use core::ptr;
use core::sync::atomic;
use alloc::boxed::Box;
use core::sync::atomic::{AtomicUsize, Ordering};

#[repr(C)]
#[derive(Debug)]
struct ListHead {
    next: AtomicUsize,
    previous: AtomicUsize,
}

impl ListHead {
    fn new_uninit() -> Self {
        ListHead {
            previous: Default::default(),
            next: Default::default()
        }
    }

    unsafe fn init(&self) {
        self.next.swap(self.next.as_mut_ptr() as usize, Ordering::Acquire);
        self.previous.swap(self.next.as_mut_ptr() as usize, Ordering::Acquire);
    }
}

#[repr(C)]
#[derive(Debug)]
struct NgMutex {
    wq: ListHead,
    v: atomic::AtomicI32,
}

extern {
    fn mtx_lock(mutex: *const NgMutex);
    fn mtx_unlock(mutex: *const NgMutex);
    fn mtx_try_lock(mutex: *const NgMutex) -> bool;
}

impl NgMutex {
    unsafe fn new_uninit() -> Self {
        NgMutex {
            wq: ListHead::new_uninit(),
            v: atomic::AtomicI32::new(0),
        }
    }

    unsafe fn init(&self) {
        self.wq.init();
        // println!("NgMutex::init {:x?} {:x?}", self as *const _, self);
    }

    fn lock(&self) {
        // Safety: The mutex exists and is initialized because NgMutex is not
        // public and this type is only constructed in Mutex::new. mtx_lock is
        // safe to call as long as the mutex exists and is initialized.
        unsafe { mtx_lock(self); }
    }

    fn unlock(&self) {
        // Safety: The mutex exists and is initialized because NgMutex is not
        // public and this type is only constructed in Mutex::new. The only way
        // to call this function is to have a valid MutexGuard, and the only way
        // to have a valid MutexGuard is to have a valid MutexGuard is to have
        // previously locked the mutex.
        unsafe { mtx_unlock(self); }
    }

    fn try_lock(&self) -> bool {
        // Safety: The mutex exists and is initialized because NgMutex is not
        // public and this type is only constructed in Mutex::new. mtx_try_lock
        // is safe to call as long as the mutex exists and is initialized.
        unsafe { mtx_try_lock(self) }
    }
}

pub struct Mutex<T: ?Sized> {
    ng_lock: NgMutex,
    data: UnsafeCell<T>,
}

pub struct MutexGuard<'a, T: ?Sized + 'a> {
    lock: &'a Mutex<T>,
}

impl<'a, T: ?Sized> Deref for MutexGuard<'a, T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        unsafe { &*self.lock.data.get() }
    }
}

impl<'a, T: ?Sized> DerefMut for MutexGuard<'a, T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<'a, T: ?Sized> Drop for MutexGuard<'a, T> {
    fn drop(&mut self) {
        self.lock.ng_lock.unlock();
    }
}

// Safety: Mutex is designed to allow only a maximum of one valid MutexGuard
// (and therefore reference) to exist to the interior data at once. It uses
// nightingale's synchronization primitives to deliver this guarantee, and this
// makes the wrapper type safe to send to other threads.
unsafe impl<T: ?Sized> Send for Mutex<T> {}
unsafe impl<T: ?Sized> Sync for Mutex<T> {}

impl<T> Mutex<T> {
    pub fn new(value: T) -> Self {
        Self {
            // Safety: Constructing an invalid Mutex is a Bad Thing, so you must call
            // Mutex::init on this instance before using it.
            ng_lock: unsafe { NgMutex::new_uninit() },
            data: UnsafeCell::new(value),
        }
    }

    pub unsafe fn init(&self) {
        self.ng_lock.init();
    }

    pub fn lock(&self) -> MutexGuard<T> {
        self.ng_lock.lock();
        MutexGuard { lock: self }
    }

    pub fn unlock(guard: MutexGuard<'_, T>) {
        drop(guard);
    }
}
