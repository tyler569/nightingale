
// Nightingale
// A kernel for x86_64
// Copyright (C) 2017, Tyler Philbrick
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


pub struct BootInformation {
    info: *const u32,
}

#[repr(u32)]
#[derive(Debug)]
#[allow(dead_code)]
pub enum Tag {
    End,
    BootCommandLine { len: u32, cmd: u8 },
    BootloaderName { len: u32, name: u8 },
    Module { len: u32, start: usize, end: usize, string: u8 },
    MemoryInfo { len: u32, mem_lower: u32, mem_upper: u32 },
    BiosBootDevice { len: u32, bisdev: u32,
                     partition: u32, sub_partition: u32 },
    MemoryMap { len: u32, entry_size: u32, entry_version: u32, entries: u8 },
    VBEInfo { len: u32, vbe_mode: u16 }, //TODO + stuff
    FramebufferInfo { len: u32, addr: usize, pitch: u32, width: u32,
                      height: u32, bpp: u8, color: u8 }, //TODO: ColorInfo enum
    ELFSymbols { len: u32, num: u16, entsize: u16, shndx: u16,
                 _reserved: u16, headers: u8 },
    APMTable { len: u32, version: u16, cseg: u16, offset: u32,
               cseg_16: u16, dseg: u16, flags: u16, cseg_len: u16,
               cseg_16_len: u16, dseg_len: u16 },
}

#[allow(dead_code)]
impl BootInformation {
    pub unsafe fn new(info: *const u32) -> BootInformation {
        BootInformation {
            info: info
        }
    }

    pub fn total_size(&self) -> u32 {
        unsafe {
            *self.info.offset(0)
        }
    }
    
    pub fn memory_map(&self) -> Option<&Tag> {
        self.get_tag(6)
    }

    pub fn get_tag(&self, search_tag_id: u32) -> Option<&Tag> {
        let mut offset = 2_isize;

        loop {
            let this_tag_id = unsafe { *self.info.offset(offset) };

            if this_tag_id == search_tag_id {
                return Some(unsafe { &*(self.info.offset(offset) as *const Tag) });
            }
            if this_tag_id == 0 {
                return None;
            } else {
                let size = unsafe { *self.info.offset(offset + 1) } as isize;
                // Multiboot Tags are 8-byte aligned, and .offset is 4 bytes
                offset += (size + size % 8) / 4;
            }
        }
   }

    pub fn info_raw(&self) -> *const u32 {
        self.info
    }

}

