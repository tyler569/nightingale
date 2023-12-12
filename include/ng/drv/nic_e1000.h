#pragma once

#include "pci_device.h"
#include <ng/vmm.h>
#include <nx/print.h>

class nic_e1000 : public pci_device {
    static constexpr uint32_t reg_ctrl = 0x0000;
    static constexpr uint32_t reg_status = 0x0008;
    static constexpr uint32_t reg_eeprom = 0x0014;
    static constexpr uint32_t reg_ctrl_ext = 0x0018;
    static constexpr uint32_t reg_imask = 0x00d0;

    static constexpr uint32_t reg_rctrl = 0x0100;
    static constexpr uint32_t reg_rxdesclo = 0x2800;
    static constexpr uint32_t reg_rxdeschi = 0x2804;
    static constexpr uint32_t reg_rxdesclen = 0x2808;
    static constexpr uint32_t reg_rxdeschead = 0x2810;
    static constexpr uint32_t reg_rxdesctail = 0x2818;

    static constexpr uint32_t reg_tctrl = 0x0400;
    static constexpr uint32_t reg_txdesclo = 0x3800;
    static constexpr uint32_t reg_txdeschi = 0x3804;
    static constexpr uint32_t reg_txdesclen = 0x3808;
    static constexpr uint32_t reg_txdeschead = 0x3810;
    static constexpr uint32_t reg_txdesctail = 0x3818;

    // rx delay timer register
    static constexpr uint32_t reg_rdtr = 0x2820;
    // rx descriptor control
    static constexpr uint32_t reg_rxdctl = 0x2828;
    // rx int. absolute delay timer
    static constexpr uint32_t reg_radv = 0x282c;
    // rx small packet detect interrupt
    static constexpr uint32_t reg_rsrpd = 0x2c00;

    // transmit inter packet gap
    static constexpr uint32_t reg_tipg = 0x0410;
    // set link up
    static constexpr uint32_t ectrl_slu = 0x40;

    // receiver enable
    static constexpr uint32_t rctl_en = (1 << 1);
    // store bad packets
    static constexpr uint32_t rctl_sbp = (1 << 2);
    // unicast promiscuous enabled
    static constexpr uint32_t rctl_upe = (1 << 3);
    // multicast promiscuous enabled
    static constexpr uint32_t rctl_mpe = (1 << 4);
    // long packet reception enable
    static constexpr uint32_t rctl_lpe = (1 << 5);
    // no loopback
    static constexpr uint32_t rctl_lbm_none = (0 << 6);
    // phy or external serdesc loopback
    static constexpr uint32_t rctl_lbm_phy = (3 << 6);
    // free buffer threshold is 1/2 of rdlen
    static constexpr uint32_t rtcl_rdmts_half = (0 << 8);
    // free buffer threshold is 1/4 of rdlen
    static constexpr uint32_t rtcl_rdmts_quarter = (1 << 8);
    // free buffer threshold is 1/8 of rdlen
    static constexpr uint32_t rtcl_rdmts_eighth = (2 << 8);
    // multicast offset - bits 47:36
    static constexpr uint32_t rctl_mo_36 = (0 << 12);
    // multicast offset - bits 46:35
    static constexpr uint32_t rctl_mo_35 = (1 << 12);
    // multicast offset - bits 45:34
    static constexpr uint32_t rctl_mo_34 = (2 << 12);
    // multicast offset - bits 43:32
    static constexpr uint32_t rctl_mo_32 = (3 << 12);
    // broadcast accept mode
    static constexpr uint32_t rctl_bam = (1 << 15);
    // vlan filter enable
    static constexpr uint32_t rctl_vfe = (1 << 18);
    // canonical form indicator enable
    static constexpr uint32_t rctl_cfien = (1 << 19);
    // canonical form indicator bit value
    static constexpr uint32_t rctl_cfi = (1 << 20);
    // discard pause frames
    static constexpr uint32_t rctl_dpf = (1 << 22);
    // pass mac control frames
    static constexpr uint32_t rctl_pmcf = (1 << 23);
    // strip ethernet crc
    static constexpr uint32_t rctl_secrc = (1 << 26);

    // buffer sizes
    static constexpr uint32_t rctl_bsize_256 = (3 << 16);
    static constexpr uint32_t rctl_bsize_512 = (2 << 16);
    static constexpr uint32_t rctl_bsize_1024 = (1 << 16);
    static constexpr uint32_t rctl_bsize_2048 = (0 << 16);
    static constexpr uint32_t rctl_bsize_4096 = ((3 << 16) | (1 << 25));
    static constexpr uint32_t rctl_bsize_8192 = ((2 << 16) | (1 << 25));
    static constexpr uint32_t rctl_bsize_16384 = ((1 << 16) | (1 << 25));

    // transmit command
    // end of packet
    static constexpr uint32_t cmd_eop = (1 << 0);
    // insert fcs
    static constexpr uint32_t cmd_ifcs = (1 << 1);
    // insert checksum
    static constexpr uint32_t cmd_ic = (1 << 2);
    // report status
    static constexpr uint32_t cmd_rs = (1 << 3);
    // report packet sent
    static constexpr uint32_t cmd_rps = (1 << 4);
    // vlan packet enable
    static constexpr uint32_t cmd_vle = (1 << 6);
    // interrupt delay enable
    static constexpr uint32_t cmd_ide = (1 << 7);

    // tctl register
    // transmit enable
    static constexpr uint32_t tctl_en = (1 << 1);
    // pad short packets
    static constexpr uint32_t tctl_psp = (1 << 3);
    // collision threshold
    static constexpr uint32_t tctl_ct_shift = 4;
    // collision distance
    static constexpr uint32_t tctl_cold_shift = 12;
    // software xoff transmission
    static constexpr uint32_t tctl_swxoff = (1 << 22);
    // re-transmit on late collision
    static constexpr uint32_t tctl_rtlc = (1 << 24);

    // descriptor done
    static constexpr uint32_t tsta_dd = (1 << 0);
    // excess collisions
    static constexpr uint32_t tsta_ec = (1 << 1);
    // late collision
    static constexpr uint32_t tsta_lc = (1 << 2);
    // transmit underrun
    static constexpr uint32_t lsta_tu = (1 << 3);

    struct __PACKED rdesc {
        uint64_t addr;
        uint16_t length;
        uint16_t checksum;
        uint8_t status;
        uint8_t errors;
        uint16_t special;
    };

    struct __PACKED tdesc {
        uint64_t addr;
        uint16_t length;
        uint8_t cso;
        uint8_t cmd;
        uint8_t status;
        uint8_t css;
        uint16_t special;
    };

    uintptr_t m_mmio_base;

public:
    static constexpr uint16_t s_vendor_id = 0x8086;
    static constexpr uint16_t s_device_id = 0x100e;

    explicit nic_e1000(pci_address addr)
        : pci_device(addr)
    {
        m_mmio_base = (uint64_t)(bar0() & 0xfffffff0) | HW_MAP_BASE;

        printf("e1000: mmio_base: %lu\n", m_mmio_base);
        printf("e1000: vendor_id: %04x\n", vendor_id());
        printf("e1000: device_id: %04x\n", device_id());
        printf("e1000: status: %04x\n", pci_read16(reg_status));
        printf("e1000: eeprom: %04x\n", pci_read16(reg_eeprom));
        printf("e1000: ctrl: %04x\n", pci_read16(reg_ctrl));
        printf("e1000: ctrl_ext: %04x\n", pci_read16(reg_ctrl_ext));
        printf("e1000: imask: %04x\n", pci_read16(reg_imask));
        printf("e1000: rctrl: %04x\n", mmio_read16(reg_rctrl));
        printf("e1000: rxdesclo: %04x\n", mmio_read16(reg_rxdesclo));
        printf("e1000: rxdeschi: %04x\n", mmio_read16(reg_rxdeschi));
        printf("e1000: rxdesclen: %04x\n", mmio_read16(reg_rxdesclen));
        printf("e1000: rxdeschead: %04x\n", mmio_read16(reg_rxdeschead));
        printf("e1000: rxdesctail: %04x\n", mmio_read16(reg_rxdesctail));
        printf("e1000: tctrl: %04x\n", mmio_read16(reg_tctrl));
        printf("e1000: txdesclo: %04x\n", mmio_read16(reg_txdesclo));
        printf("e1000: txdeschi: %04x\n", mmio_read16(reg_txdeschi));
        printf("e1000: txdesclen: %04x\n", mmio_read16(reg_txdesclen));
        printf("e1000: txdeschead: %04x\n", mmio_read16(reg_txdeschead));
        printf("e1000: txdesctail: %04x\n", mmio_read16(reg_txdesctail));
        printf("e1000: rdtr: %04x\n", mmio_read16(reg_rdtr));
        printf("e1000: rxdctl: %04x\n", mmio_read16(reg_rxdctl));
        printf("e1000: radv: %04x\n", mmio_read16(reg_radv));
        printf("e1000: rsrpd: %04x\n", mmio_read16(reg_rsrpd));
        printf("e1000: tipg: %04x\n", mmio_read16(reg_tipg));
    }

    uint8_t mmio_read8(int offset) const
    {
        return *(volatile uint8_t *)(m_mmio_base + offset);
    }

    uint16_t mmio_read16(int offset) const
    {
        return *(volatile uint16_t *)(m_mmio_base + offset);
    }

    uint32_t mmio_read32(int offset) const
    {
        return *(volatile uint32_t *)(m_mmio_base + offset);
    }

    uint64_t mmio_read64(int offset) const
    {
        return *(volatile uint64_t *)(m_mmio_base + offset);
    }

    void mmio_write8(int offset, uint8_t value) const
    {
        *(volatile uint8_t *)(m_mmio_base + offset) = value;
    }

    void mmio_write16(int offset, uint16_t value) const
    {
        *(volatile uint16_t *)(m_mmio_base + offset) = value;
    }

    void mmio_write32(int offset, uint32_t value) const
    {
        *(volatile uint32_t *)(m_mmio_base + offset) = value;
    }

    void mmio_write64(int offset, uint64_t value) const
    {
        *(volatile uint64_t *)(m_mmio_base + offset) = value;
    }
};
