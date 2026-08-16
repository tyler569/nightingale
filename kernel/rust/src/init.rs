#[macro_export]
macro_rules! define_init {
    ($func:expr, $level:literal) => {
        #[unsafe(link_section=concat!("init.", $level))]
        #[used]
        static FN_PTR: fn() = $func;
    }
}
