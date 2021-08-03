use alloc::sync::Arc;
use alloc::vec::Vec;
use alloc::vec;
use core::fmt::Write;

pub enum Error {
    NoPermission,
    NotImplemented,
    NotFound,
}

pub enum FileType {
    Regular,
    Directory,
    Procedural,
    Device,
}

pub type OpenFlags = usize;
pub const READ_ONLY: OpenFlags = 1;
pub const WRITE_ONLY: OpenFlags = 2;
pub const READ_WRITE: OpenFlags = READ_ONLY | WRITE_ONLY;

pub type Result<T> = ::core::result::Result<T, Error>;

pub trait File {
    fn filetype(&self) -> FileType;
    fn open(&self, flags: OpenFlags) -> FileDescription;

    fn read(&self, fd: &mut FileDescription, buf: &mut [u8]) -> Result<u64>;
    fn write(&self, fd: &mut FileDescription, buf: &[u8]) -> Result<u64>;
    fn list_children(&self, fd: &mut FileDescription, buf: &mut [DirectoryEntry]) -> Result<u64>;
    fn child(&self, fd: &mut FileDescription, name: &str) -> Result<Arc<dyn File>>;
}

pub struct FileDescription {
    pub file: Arc<dyn File>,
    pub offset: u64,
    pub eof: bool,
}

#[repr(C)]
pub struct DirectoryEntry {
    name: [u8; 64],
    filetype: i32,
}

struct Directory {
}

struct Inode {
}

struct InodeCache {} // owns all the in-kernel Inodes

// test
