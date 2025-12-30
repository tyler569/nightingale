#![no_std]

pub use core::panic::PanicInfo;

#[repr(C)]
pub struct ModInfo {
    pub name: *const u8,
    pub init: Option<extern "C" fn(*const Mod) -> i32>,
    pub fini: Option<extern "C" fn(*const Mod)>,
}

// SAFETY: ModInfo contains raw pointers that point to static data,
// which is safe to share between threads in a kernel context
unsafe impl Sync for ModInfo {}

#[repr(C)]
pub struct Mod {
    _private: [u8; 0],
}

pub const MODINIT_SUCCESS: i32 = 0;
pub const MODINIT_FAILURE: i32 = 1;

pub mod ffi {
    extern "C" {
        pub fn printf(fmt: *const u8, ...) -> i32;
    }
}

#[cfg(not(test))]
#[panic_handler]
pub fn panic(_info: &PanicInfo) -> ! {
    // TODO: call the existing C panic function
    loop {}
}

/// Helper macro to create a null-terminated byte string literal
///
/// # Example
/// ```ignore
/// use nightingale_kernel::cstr;
/// let s = cstr!("Hello, world!");
/// ```
#[macro_export]
macro_rules! cstr {
    ($s:expr) => {
        concat!($s, "\0").as_ptr() as *const u8
    };
}

/// Helper macro to define a kernel module with proper exports
///
/// # Example
/// ```ignore
/// use nightingale_kernel::*;
///
/// fn my_init(_mod: *const Mod) -> i32 {
///     unsafe {
///         ffi::printf(cstr!("Module loaded!\n"));
///     }
///     MODINIT_SUCCESS
/// }
///
/// kernel_module! {
///     name: "my_module",
///     init: my_init,
/// }
/// ```
#[macro_export]
macro_rules! kernel_module {
    (name: $name:expr, init: $init:expr $(, fini: $fini:expr)? $(,)?) => {
        #[no_mangle]
        pub extern "C" fn modinit(mod_ptr: *const $crate::Mod) -> i32 {
            $init(mod_ptr)
        }

        $(
        #[no_mangle]
        pub extern "C" fn modfini(mod_ptr: *const $crate::Mod) {
            $fini(mod_ptr)
        }
        )?

        #[used]
        #[no_mangle]
        #[link_section = ".data"]
        static modinfo: $crate::ModInfo = $crate::ModInfo {
            name: $crate::cstr!($name),
            init: Some(modinit),
            fini: kernel_module!(@fini $($fini)?),
        };
    };
    (@fini) => { None };
    (@fini $fini:expr) => { Some(modfini) };
}
