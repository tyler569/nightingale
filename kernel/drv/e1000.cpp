#include <ng/drv/pci_device.h>

class e1000 : public pci_device {
public:
    explicit e1000(pci_address pci_address)
        : pci_device(pci_address)
    {
    }

    void init();
    void send_packet(const void *data, size_t len);
    void recv_packet(void *data, size_t len);
};

void e1000::init() { }