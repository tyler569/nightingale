use alloc::boxed::Box;
use alloc::string::String;
use alloc::sync::Arc;
use alloc::vec::Vec;

use intrusive_collections::intrusive_adapter;
use intrusive_collections::{LinkedList, LinkedListLink};

struct InodeChild {
    link: LinkedListLink,
    name: String,
    node: Arc<dyn Inode>,
}

intrusive_adapter!(InodeAdapter = Box<InodeChild>: InodeChild { link: LinkedListLink });

struct StatBuffer;

trait Inode {
    fn core(&self) -> &InodeCore;

    fn stat(&self, _buffer: &mut StatBuffer) -> Result<(), ()> {
        let core = self.core();
        Err(())
    }

    fn iter_children(&self) -> ! {
        self.core().children.iter().map(|l| l.node.clone());
        panic!();
    }
}

#[derive(Default)]
struct InodeCore {
    children: LinkedList<InodeAdapter>,
    file_type: i32,
    mode: i32,
    flags: i32,
    user: i32,
    group: i32,
    size: i32,
    ctime: u32,
    atime: u32,
    mtime: u32,
    block_count: u32,
}

impl InodeCore {
    fn new(file_type: i32) -> Self {
        InodeCore {
            children: LinkedList::new(InodeAdapter::new()),
            file_type,
            ..Default::default()
        }
    }

}

struct TmpFsInode {
    core: InodeCore,
    memory_data: Option<Vec<u8>>,
    socket: Option<()>,
}

const NORMAL_FILE: i32 = 1;
const DIRECTORY_FILE: i32 = 2;

impl TmpFsInode {
    fn new_file() -> Self {
        Self {
            core: InodeCore::new(NORMAL_FILE),
            memory_data: Some(Vec::new()),
            socket: None
        }
    }

    fn new_directory() -> Self {
        Self {
            core: InodeCore::new(DIRECTORY_FILE),
            memory_data: None,
            socket: None,
        }
    }
}

impl Inode for TmpFsInode {
    fn core(&self) -> &InodeCore {
        &self.core
    }
}


