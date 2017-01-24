
ORGANIZATION
============

kernel/

include/kernel : kernel internal API
- used by kernel code to do kernel things
- ARCHITECTURE INDEPENDANT IMPLEMETATION

include/kernel/arch : architecture specific definitions
- defines the primitives an architecture MUST implement in its arch/\*/
- usually very simple stuff: i.e.
    - serial is required
    - the architecture MUST define functions to read and write a serial console
        - uint8_t serial_read()
        - void serial_write(uint8_t)
    - the kernel can then use those to define other things
        - int32_t serial_print(char *)


