#if 0 // ACPI is on back burner
    acpi_rsdp *rsdp = mb_acpi_get_rsdp();
    acpi_rsdt *rsdt = acpi_get_rsdt(rsdp);
    vmm_map((uintptr_t)rsdt, (uintptr_t)rsdt);
    printf("acpi: RSDT found at %lx\n", rsdt);
    acpi_madt *madt = acpi_get_table(MADT);
    if (!madt)  panic("No MADT found!");
    printf("acpi: MADT found at %lx\n", madt);
    acpi_print_table((acpi_header *)madt);
#endif

#if 0 // APIC is on back burner
    enable_apic(0xFEE00000);// TMPTMP HARDCODE

    volatile uint32_t *lapic_timer        = (volatile uint32_t *)(0xfee00000 + 0x320);
    volatile uint32_t *lapic_timer_count  = (volatile uint32_t *)(0xfee00000 + 0x380);
    volatile uint32_t *lapic_timer_divide = (volatile uint32_t *)(0xfee00000 + 0x3E0);

    *lapic_timer_divide = 0x3;      // divide by 16
    *lapic_timer_count = 50000;     // initial countdown amount
    *lapic_timer = 0x20020;         // enabled, periodic, not masked
#endif

#if 0 // VMM fail - save for testing infra
    uintptr_t page = pmm_allocate_page();
    uintptr_t vma = 0x450000;
    vmm_map(vma, page);
    int *value = (int *)vma;

    *value = 100;
    printf("before: %i\n", *value);
    vmm_edit_flags(vma, 0); // !WRITEABLE
    printf("after read : %i\n", *value);
    *value = 100;
    printf("after write: %i\n", *value);
#endif

#ifdef __TEST_UBSAN
    int bar = (int)0x7FFFFFFF;
    bar += 1;
    printf("%i\n", bar);
#endif

#ifdef __TEST_MUTEX
    static kmutex test_mutex = KMUTEX_INIT;

    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("release test mutex: %i\n", release_mutex(&test_mutex));

    printf("acquire test mutex: %i\n", await_mutex(&test_mutex));
    printf("try acquire test mutex: %i\n", try_acquire_mutex(&test_mutex));
    printf("release test mutex: %i\n", release_mutex(&test_mutex));

    printf("acquire test mutex: %i\n", await_mutex(&test_mutex));
#endif

#if 0 // network testing
    uint32_t rtl_nic_addr = pci_find_device_by_id(0x10ec, 0x8139);
    if (rtl_nic_addr == ~0) {
        printf("no rtl8139 found, aborting network test\n");
    } else {

        struct net_if *nic = init_rtl8139(rtl_nic_addr);

        struct eth_hdr *arp_req = calloc(ETH_MTU, 1);
        size_t len = make_ip_arp_req(arp_req, nic->mac_addr, 0x0a00020f, 0x0a000202);

        len = (len > 64) ? len : 64;

        print_arp_pkt((void *)&arp_req->data);
        send_packet(nic, arp_req, len);
        //free(arp_req);
        
        struct mac_addr gw_mac = {{ 0x52, 0x55, 0x0a, 0x00, 0x02, 0x02 }};
        struct mac_addr zero_mac = {{ 0, 0, 0, 0, 0, 0 }};

#define PINGDL 64

        void *ping = calloc(ETH_MTU, 1);
        len = make_eth_hdr(ping, gw_mac, zero_mac, ETH_IP);
        struct eth_hdr *ping_frame = ping;
        //len += make_ip_hdr(ping + len, 0x4050, PROTO_ICMP, 0x0a000202);
        len += make_ip_hdr(ping + len, 0x4050, PROTO_ICMP, 0x0a000202);
        struct ip_hdr *ping_packet = (void *)&ping_frame->data;
        len += make_icmp_req(ping + len, 0xaa, 1);
        struct icmp_pkt *ping_msg = (void *)&ping_packet->data;
        len += PINGDL;
        ping_packet->total_len = htons(len - sizeof(struct eth_hdr));

        for (int i=0; i<3; i++) {
            memset(ping + len - PINGDL, 0x41 + i, PINGDL);

            ping_packet->hdr_checksum = 0;
            ping_msg->checksum = 0;
            place_ip_checksum(ping_packet);
            place_icmp_checksum(ping_msg, PINGDL);

            send_packet(nic, ping, len);
        }
    }
#endif

    // printf("\n");

    // printf("test random: %x\n", rand_get());


    // also acts as __strong_heap_protection litmus against net code above
#if 0
    int *foo = malloc(0x80000);
    foo = malloc(0x80000);
    printf("test unbacked memory: malloc at %#lx ... ", foo);
    *foo = 10;
    if (*foo == 10) {
        printf("works!\n");
    } else {
        printf("didn't work *and* didn't fault? umm...\n");
    }

    extern uintptr_t boot_pml4;
    printf("boot pml4 at %lx\n", &boot_pml4);
#endif

