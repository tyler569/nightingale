
#include <basic.h>
#include <print.h>

__used
void __cyg_profile_func_enter(void* func, void* callsite) {
    print_ptr(func);
}

__used
void __cyg_profile_func_exit(void* func, void* callsite) {

}

