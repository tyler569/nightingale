#![allow(unused)]

use core::ffi::c_int;

pub type VirtAddr = usize;
pub type PhysAddr = usize;

pub const PAGE_PRESENT: u64 = 0x01;
pub const PAGE_WRITABLE: u64 = 0x02;
pub const PAGE_USERMODE: u64 = 0x04;
pub const PAGE_ACCESSED: u64 = 0x20;
pub const PAGE_DIRTY: u64 = 0x40;
pub const PAGE_ISHUGE: u64 = 0x80;
pub const PAGE_GLOBAL: u64 = 0x100;

unsafe extern "C" {
    pub fn vmm_resolve(vma: VirtAddr) -> PhysAddr;
    pub fn vmm_virt_to_phy(vma: VirtAddr) -> PhysAddr;

    pub fn vmm_map_range(vma: VirtAddr, pma: PhysAddr, len: usize, flags: c_int);
    pub fn vmm_create_unbacked_range(vma: VirtAddr, len: usize, flags: c_int);
    pub fn vmm_unmap_range(vma: VirtAddr, len: usize);
}

pub unsafe fn resolve(vma: VirtAddr) -> PhysAddr {
    unsafe { vmm_resolve(vma) }
}

pub unsafe fn map(vma: VirtAddr, pma: PhysAddr, len: usize, flags: u64) {
    unsafe { vmm_map_range(vma, pma, len, flags as c_int) }
}

pub unsafe fn create_unbacked(vma: VirtAddr, len: usize, flags: u64) {
    unsafe { vmm_create_unbacked_range(vma, len, flags as c_int) }
}

pub unsafe fn unmap(vma: VirtAddr, len: usize) {
    unsafe { vmm_unmap_range(vma, len) }
}
