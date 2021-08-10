use core::cell::UnsafeCell;
use core::ffi::c_void;
use core::marker::{Send, Sync};
use core::mem;
use core::ops::{Deref, DerefMut, Drop};
use core::ptr;
use core::sync::atomic::*;
use alloc::boxed::Box;
use alloc::sync::Arc;

extern "C" {
    // TODO: are these properly atomic enough to allow *const ?
    // I don't think this is safe.
    fn block_thread(queue: *const ListHead);
    fn wake_waitq_one(queue: *const ListHead);
    fn wake_waitq_all(queue: *const ListHead);
}

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
        self.next.store(self.next.as_mut_ptr() as usize, Ordering::Release);
        self.previous.store(self.next.as_mut_ptr() as usize, Ordering::Release);
    }
}

#[repr(C)]
pub struct Mutex<T: ?Sized> {
    wait_queue: ListHead,
    state: AtomicI32,
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
        self.lock.state.store(0, Ordering::Release);
        unsafe { wake_waitq_one(&self.lock.wait_queue); }
    }
}

// Safety: Mutex is designed to allow only a maximum of one valid MutexGuard
// (and therefore reference) to exist to the interior data at once. It uses
// nightingale's synchronization primitives to deliver this guarantee, and this
// makes the wrapper type safe to send to other threads.
unsafe impl<T: ?Sized> Send for Mutex<T> {}
unsafe impl<T: ?Sized> Sync for Mutex<T> {}

impl<T> Mutex<T> {
    unsafe fn new(value: T) -> Self {
        Self {
            // Safety: Constructing an invalid Mutex is a Bad Thing, so you must call
            // Mutex::init on this instance before using it.
            wait_queue: unsafe { ListHead::new_uninit() },
            state: 0.into(),
            data: UnsafeCell::new(value),
        }
    }

    pub fn new_box(value: T) -> Box<Self> {
        let boxed = Box::new(unsafe { Self::new(value) });
        unsafe { boxed.wait_queue.init(); }
        boxed
    }

    pub fn new_arc(value: T) -> Arc<Self> {
        let mut boxed = Arc::new(unsafe { Self::new(value) });
        unsafe { boxed.wait_queue.init(); }
        boxed
    }
}

impl<T: ?Sized> Mutex<T> {
    pub fn lock(&self) -> MutexGuard<T> {
        while let Err(_) = self.state.compare_exchange(0, 1, Ordering::Acquire, Ordering::Relaxed) {
            unsafe { block_thread(&self.wait_queue); }
        }
        MutexGuard { lock: self }
    }

    pub fn unlock(guard: MutexGuard<'_, T>) {
        drop(guard);
    }
}
