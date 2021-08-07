#[repr(C)]
struct TaskContext {
    // struct jmp_buf - IP + flags and page table root.

    rbx: usize,
    rbp: usize,
    r12: usize,
    r13: usize,
    r14: usize,
    r15: usize,
    rsp: usize,

    rflags: usize,
    cr3: usize,
}

pub extern "C" fn switch(from: &mut TaskContext, to: &mut TaskContext) {
    asm!("
        mov [rdi + {offset_rbx}], rbx
        mov rbx, [rsi + {offset_rbx}]

        mov [rdi + {offset_rbp}], rbp
        mov rbp, [rsi + {offset_rbp}]

        mov [rdi + {offset_r12}], r12
        mov r12, [rsi + {offset_r12}]
        ",
        offset_rbx = const(offset_of!(TaskContext, rbx)),
        offset_rbp = const(offset_of!(TaskContext, rbp)),
        offset_r12 = const(offset_of!(TaskContext, r12)),
    );
}