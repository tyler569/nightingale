### LibM

DIR := external/libm
OUT := $(SYSLIB)/libm.a

LIBM := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS)
$(OUT): DIR := $(DIR)
$(OUT): $(shell find $(DIR) -type f -name '*.[cS]')
	$(MAKE) -C $(DIR) CFLAGS="$(CFLAGS)"
	$(MAKE) -C $(DIR) CFLAGS="$(CFLAGS)" install

### Lua

DIR := external/lua
OUT := $(SYSBIN)/lua

LUA := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS) -Wno-attributes
$(OUT): LDFLAGS := $(ULDFLAGS)
$(OUT): DIR := $(DIR)
$(OUT): $(shell find $(DIR) -type f -name '*.c')
	$(MAKE) -C $(DIR) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"
	$(MAKE) -C $(DIR) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" install
