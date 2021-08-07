use core::panic::PanicInfo;

extern "C" {
    fn break_point();
    fn disable_irqs();
    fn halt() -> !;
}

#[panic_handler]
fn panic(panic_info: &PanicInfo) -> ! {
    unsafe {
        break_point();
        disable_irqs();
        println!("[PANIC] panic from Rust: {:?}", panic_info);
        halt();
    }
}
