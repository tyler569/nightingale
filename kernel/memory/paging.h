
#pragma once
#ifndef NIGHTINGALE_PAGING_H
#define NIGHTINGALE_PAGING_H

typedef struct PageTable {} PageTable;

#define PAGE_PRESENT 0x01
#define PAGE_WRITEABLE 0x02
#define PAGE_USERMODE 0x04
#define PAGE_ACCESSED 0x20
#define PAGE_DIRTY 0x40
#define PAGE_ISHUGE 0x80
#define PAGE_GLOBAL 0x100

// 3 + 3 + 4
#define PAGE_MASK_1G 07777777777

// 3 + 4
#define PAGE_MASK_2M    07777777

// 4
#define PAGE_MASK_4K       07777

#endif
