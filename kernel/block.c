#include "ng/common.h"
#include "ng/x86/cpu.h"

#define ATA_STATUS_ERR 0x01
#define ATA_STATUS_DRQ 0x08
#define ATA_STATUS_DF 0x20
#define ATA_STATUS_READY 0x40
#define ATA_STATUS_BUSY 0x80

static void wait_until_not_busy(void)
{
    while (inb(0x1F7) & ATA_STATUS_BUSY) { }
}

static void wait_until_not_ready(void)
{
    while (!(inb(0x1F7) & ATA_STATUS_READY)) { }
}

int read_sector(long lba, void *dest)
{
    wait_until_not_busy();
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0xF));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20); // Send the read command
    uint16_t *buffer = (uint16_t *)dest;

    wait_until_not_busy();
    wait_until_not_ready();

    for (int i = 0; i < 256; i++)
        buffer[i] = inw(0x1F0);

    return 0;
}

int write_sector(long lba, void *src)
{
    wait_until_not_busy();
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0xF));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x30); // Send the write command
    uint16_t *buffer = (uint16_t *)src;

    wait_until_not_busy();
    wait_until_not_ready();

    for (int i = 0; i < 256; i++)
        outw(0x1F0, buffer[i]);

    return 0;
}
