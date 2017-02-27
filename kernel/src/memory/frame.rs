
struct Frame {
    id: usize,
}

impl Frame {
    fn new(id: usize) -> Frame {
        Frame { id: id }
    }
}

trait FrameAllocator {
    fn allocate_frame(&mut self) -> Option<Frame>;
    fn deallocate_frame (&mut self, frame: Frame);
}

struct BumpFrameAllocator {
    pool_start: usize,
    pool_end: usize,
}

impl BumpFrameAllocator {
    fn new(region_start: usize, region_end: usize) -> BumpFrameAllocator {
        BumpFrameAllocator { pool_start: region_start, pool_end: region_end }
    }
}

impl FrameAllocator for BumpFrameAllocator {
    fn allocate_frame(&mut self) -> Option<Frame> {
        // Ensure Frame is aligned to 4k
        let frame_start = (self.pool_start + 0x3FF) & (!0x3FF); 
        if pool_end - frame_start > 4096 {
            Some(Frame { id: frame_start })
        } else {
            None
        }
    }

    fn deallocate_frame(&mut self, frame: Frame) {
        // Leak it
        panic!("Leaking frame {:?}", frame);
    }
}

