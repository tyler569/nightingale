DIR := user
BDIR := $(DIR)
PROGRAMS := init echo uname what bf strace bomb cat threads false forks \
	top clone insmod fcat args pipe sg kill write segv echoserv sleep bg \
	fio float malloctest net rsh udpnc ls rot13 time multiread create \
	crash ab hog head threadsw column bf2 sigtest
CSRC := $(patsubst %,$(DIR)/%.c,$(PROGRAMS))
ASRC :=
COBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(CSRC))
AOBJ := $(patsubst $(DIR)/%,$(BUILD)/$(BDIR)/%.o,$(ASRC))
OBJ := $(COBJ) $(AOBJ)
OUT := $(patsubst %,$(SYSBIN)/%,$(PROGRAMS))

PROGRAMS := $(OUT)

$(OUT): CFLAGS := $(UCFLAGS)
$(OUT): INCLUDE := $(UINCLUDE)
$(COBJ): $(BUILD)/$(BDIR)/%.c.o: $(DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<
$(AOBJ): $(BUILD)/$(BDIR)/%.S.o: $(DIR)/%.S
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OUT): $(SYSBIN)/%: $(BUILD)/$(BDIR)/%.c.o
	$(MKDIR)
	$(CC) $(CFLAGS) -o $@ $<

