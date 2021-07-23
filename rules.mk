NIGHTINGALE_VERSION := $(shell git describe --tags)

COMMON_CFLAGS := \
	-std=c11 \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-g \
	-O2 \
	-D__nightingale__=1 \
	-ffreestanding \
	-nostdlib \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \
	-Wno-sign-compare \
	-Wno-address-of-packed-member \

KERNEL_CFLAGS := \
	$(COMMON_CFLAGS) \
	-mno-red-zone \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-DNIGHTINGALE_VERSION="\"$(NIGHTINGALE_VERSION)\"" \
	-D__kernel__=1 \
	-mcmodel=kernel \
	-fno-asynchronous-unwind-tables \
	-fno-omit-frame-pointer \
	-fsanitize=undefined \

USER_CFLAGS := \
	$(COMMON_CFLAGS) \
	-fuse-ld=lld

KERNEL_COMPILE = clang -target x86_64-unknown-none -I../include $(KERNEL_CFLAGS) -o $@ -c $<
KERNEL_LINK = ld.lld --no-check-sections -Tarch/x86/link_hh.ld -o $@ $^

USER_COMPILE = clang -target x86_64-unknown-none -I../include $(USER_CFLAGS) -o $@ -c $<
USER_LINK = clang -target x86_64-unknown-none $(USER_CFLAGS) -o $@ $<
USER_AR = ar rcs $@ $^

SYSROOT_INSTALL = cp $^ ../sysroot/$(INSTALL_DIR)/$@
