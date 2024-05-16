.extern c_interrupt_shim

.globl interrupt_shim
interrupt_shim:
    SUB SP, SP, #256
    STP X0, X1, [SP], +16
    STP X2, X3, [SP], +16
    STP X4, X5, [SP], +16
    STP X6, X7, [SP], +16
    STP X8, X9, [SP], +16
    STP X10, X11, [SP], +16
    STP X12, X13, [SP], +16
    STP X14, X15, [SP], +16
    STP X16, X17, [SP], +16
    STP X18, X19, [SP], +16
    STP X20, X21, [SP], +16
    STP X22, X23, [SP], +16
    STP X24, X25, [SP], +16
    STP X26, X27, [SP], +16
    STP X28, X29, [SP], +16
    STP X30, X31, [SP], +16
    MOV X0, SP
    BL c_interrupt_shim
    LDP X0, X1, [SP, #-16]!
    LDP X2, X3, [SP, #-16]!
    LDP X4, X5, [SP, #-16]!
    LDP X6, X7, [SP, #-16]!
    LDP X8, X9, [SP, #-16]!
    LDP X10, X11, [SP, #-16]!
    LDP X12, X13, [SP, #-16]!
    LDP X14, X15, [SP, #-16]!
    LDP X16, X17, [SP, #-16]!
    LDP X18, X19, [SP, #-16]!
    LDP X20, X21, [SP, #-16]!
    LDP X22, X23, [SP, #-16]!
    LDP X24, X25, [SP, #-16]!
    LDP X26, X27, [SP, #-16]!
    LDP X28, X29, [SP, #-16]!
    LDP X30, X31, [SP, #-16]!
    ADD SP, SP, #256
    ERET