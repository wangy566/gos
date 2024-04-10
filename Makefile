GNU = riscv64-unknown-linux-gnu-

TOPDIR := $(shell pwd)

CC := $(GNU)gcc
LD := $(GNU)ld
RISCV_COPY := $(GNU)objcopy
RISCV_COPY_FLAGS := --set-section-flags .bss=alloc,contents --set-section-flags .sbss=alloc,contents -O binary
RISCV_DUMP := $(GNU)objdump

DEBUG := -DUSE_QEMU
DEBUG += -DUSE_AIA
#DEBUG += -DIOMMU_PTWALK_TEST

COPS := -g -O0 -Wall -nostdlib -mcmodel=medany -mabi=lp64 -march=rv64imafdc -fno-PIE -fomit-frame-pointer -Wno-builtin-declaration-mismatch

export TOPDIR
export CC
export LD
export COPS
export DEBUG

BUILD_DIR = build
MYSBI_DIR = mysbi
BSP_DIR   = bsp
MYGUEST_DIR = myGuest

default: clean mysbi myGuest gos pack.sh
	./pack.sh

fpga:
	./bin2fpgadata -i out/Image.bin

#build guest
GUEST_TARGET := myGuest.elf
GUEST_TARGET_BIN := myGuest.bin

MYGUEST_ENTRY_DIR = $(MYGUEST_DIR)/entry
MYGUEST_CORE_DIR = $(MYGUEST_DIR)/core
MYGUEST_DRIVERS_DIR = $(MYGUEST_DIR)/drivers
MYGUEST_LIB_DIR = $(MYGUEST_DIR)/lib

GUEST_ENTRY_C_FILES = $(wildcard $(MYGUEST_ENTRY_DIR)/*.c)
GUEST_ENTRY_ASM_FILES = $(wildcard $(MYGUEST_ENTRY_DIR)/*.S)
GUEST_CORE_C_FILES = $(wildcard $(MYGUEST_CORE_DIR)/*.c)
GUEST_DRIVERS_C_FILES = $(wildcard $(MYGUEST_DRIVERS_DIR)/*.c)
GUEST_LIB_C_FILES = $(wildcard $(MYGUEST_LIB_DIR)/*.c)

MYGUEST_OBJ_FILES = $(GUEST_ENTRY_ASM_FILES:$(MYGUEST_ENTRY_DIR)/%.S=$(MYGUEST_ENTRY_DIR)/%_s.o)
MYGUEST_OBJ_FILES += $(GUEST_ENTRY_C_FILES:$(MYGUEST_ENTRY_DIR)/%.c=$(MYGUEST_ENTRY_DIR)/%_c.o)
MYGUEST_OBJ_FILES += $(GUEST_LIB_C_FILES:$(MYGUEST_LIB_DIR)/%.c=$(MYGUEST_LIB_DIR)/%_c.o)
MYGUEST_OBJ_FILES += $(GUEST_CORE_C_FILES:$(MYGUEST_CORE_DIR)/%.c=$(MYGUEST_CORE_DIR)/%_c.o)
MYGUEST_OBJ_FILES += $(GUEST_DRIVERS_C_FILES:$(MYGUEST_DRIVERS_DIR)/%.c=$(MYGUEST_DRIVERS_DIR)/%_c.o)

myGuest: $(MYGUEST_OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(LD) -T $(MYGUEST_DIR)/myGuest.lds -o $(BUILD_DIR)/$(GUEST_TARGET) $(MYGUEST_OBJ_FILES) -Map $(BUILD_DIR)/myGuest.map
	$(RISCV_COPY) $(BUILD_DIR)/$(GUEST_TARGET) -O binary $(BUILD_DIR)/$(GUEST_TARGET_BIN)

$(MYGUEST_ENTRY_DIR)/%_s.o: $(MYGUEST_ENTRY_DIR)/%.S
	$(CC) $(COPS) -I$(MYGUEST_DIR)/include -c $< -o $@

$(MYGUEST_ENTRY_DIR)/%_c.o: $(MYGUEST_ENTRY_DIR)/%.c
	$(CC) $(COPS) -I$(MYGUEST_DIR)/include -c $< -o $@

$(MYGUEST_CORE_DIR)/%_c.o: $(MYGUEST_CORE_DIR)/%.c
	$(CC) $(COPS) -I$(MYGUEST_DIR)/include -c $< -o $@

$(MYGUEST_DRIVERS_DIR)/%_c.o: $(MYGUEST_DRIVERS_DIR)/%.c
	$(CC) $(COPS) -I$(MYGUEST_DIR)/include -c $< -o $@

$(MYGUEST_LIB_DIR)/%_c.o: $(MYGUEST_LIB_DIR)/%.c
	$(CC) $(COPS) -I$(MYGUEST_DIR)/include -c $< -o $@

# build sbi
SBI_TARGET := mysbi.elf
SBI_TARGET_BIN := mysbi.bin

MYSBI_ENTRY_DIR = $(MYSBI_DIR)/entry
MYSBI_SBI_DIR   = $(MYSBI_DIR)/sbi
MYSBI_LIB_DIR   = $(MYSBI_DIR)/lib

SBI_C_FILES = $(wildcard $(MYSBI_SBI_DIR)/*.c)
SBI_ASM_FILES = $(wildcard $(MYSBI_SBI_DIR)/*.S)
SBI_ENTRY_C_FILES = $(wildcard $(MYSBI_ENTRY_DIR)/*.c)
SBI_ENTRY_ASM_FILES = $(wildcard $(MYSBI_ENTRY_DIR)/*.S)
SBI_LIB_C_FILES = $(wildcard $(MYSBI_LIB_DIR)/*.c)
SBI_BSP_C_FILES = $(wildcard $(BSP_DIR)/*.c)

MYSBI_OBJ_FILES = $(SBI_ENTRY_ASM_FILES:$(MYSBI_ENTRY_DIR)/%.S=$(MYSBI_ENTRY_DIR)/%_s.o)
MYSBI_OBJ_FILES += $(SBI_ENTRY_C_FILES:$(MYSBI_ENTRY_DIR)/%.c=$(MYSBI_ENTRY_DIR)/%_c.o)
MYSBI_OBJ_FILES += $(SBI_C_FILES:$(MYSBI_SBI_DIR)/%.c=$(MYSBI_SBI_DIR)/%_c.o)
MYSBI_OBJ_FILES += $(SBI_ASM_FILES:$(MYSBI_SBI_DIR)/%.S=$(MYSBI_SBI_DIR)/%_s.o)
MYSBI_OBJ_FILES += $(SBI_BSP_C_FILES:$(BSP_DIR)/%.c=$(BSP_DIR)/%_c.o)
MYSBI_OBJ_FILES += $(ENTRY_C_FILES:$(MYSBI_ENTRY_DIR)/%.c=$(MYSBI_ENTRY_DIR)/%_c.o)
MYSBI_OBJ_FILES += $(SBI_LIB_C_FILES:$(MYSBI_LIB_DIR)/%.c=$(MYSBI_LIB_DIR)/%_c.o)

mysbi: $(MYSBI_OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	$(LD) -T $(MYSBI_DIR)/mysbi.lds -o $(BUILD_DIR)/$(SBI_TARGET) $(MYSBI_OBJ_FILES) -Map $(BUILD_DIR)/mysbi.map
	$(RISCV_COPY) $(BUILD_DIR)/$(SBI_TARGET) -O binary $(BUILD_DIR)/$(SBI_TARGET_BIN)
	
$(MYSBI_ENTRY_DIR)/%_s.o: $(MYSBI_ENTRY_DIR)/%.S
	$(CC) $(COPS) -I$(MYSBI_DIR)/include -c $< -o $@

$(MYSBI_ENTRY_DIR)/%_c.o: $(MYSBI_ENTRY_DIR)/%.c
	$(CC) $(COPS) -I$(MYSBI_DIR)/include -c $< -o $@

$(MYSBI_SBI_DIR)/%_s.o: $(MYSBI_SBI_DIR)/%.S
	$(CC) $(COPS) -I$(MYSBI_DIR)/include -c $< -o $@

$(MYSBI_SBI_DIR)/%_c.o: $(MYSBI_SBI_DIR)/%.c
	$(CC) $(COPS) -I$(MYSBI_DIR)/include -c $< -o $@

$(MYSBI_LIB_DIR)/%_c.o: $(MYSBI_LIB_DIR)/%.c
	$(CC) $(COPS) -I$(MYSBI_DIR)/include -c $< -o $@

$(BSP_DIR)/%_c.o: $(BSP_DIR)/%.c
	$(CC) $(COPS) -I$(MYSBI_DIR)/include $(DEBUG) -c $< -o $@

# build gos
GOS_ENTRY_DIR = entry
GOS_LIB_DIR = lib
GOS_HAL_DIR = $(TOPDIR)/hal
APP_DIR   = app

GOS_VIRT_DIR = virt

export GOS_HAL_DIR

GOS_TARGET := gos.elf
GOS_TARGET_BIN := gos.bin

GOS_ENTRY_ASM_FILES = $(wildcard $(GOS_ENTRY_DIR)/*.S)
GOS_ENTRY_C_FILES = $(wildcard $(GOS_ENTRY_DIR)/*.c)
GOS_LIB_C_FILES = $(wildcard $(GOS_LIB_DIR)/*.c)
GOS_VIRT_ASM_FILES = $(wildcard $(GOS_VIRT_DIR)/*.S)
GOS_VIRT_C_FILES = $(wildcard $(GOS_VIRT_DIR)/*.c)

GOS_OBJ_FILES = $(GOS_ENTRY_ASM_FILES:$(GOS_ENTRY_DIR)/%.S=$(GOS_ENTRY_DIR)/%_s.o)
GOS_OBJ_FILES += $(GOS_ENTRY_C_FILES:$(GOS_ENTRY_DIR)/%.c=$(GOS_ENTRY_DIR)/%_c.o)
GOS_OBJ_FILES += $(GOS_LIB_C_FILES:$(GOS_LIB_DIR)/%.c=$(GOS_LIB_DIR)/%_c.o)
GOS_OBJ_FILES += $(GOS_VIRT_ASM_FILES:$(GOS_VIRT_DIR)/%.S=$(GOS_VIRT_DIR)/%_s.o)
GOS_OBJ_FILES += $(GOS_VIRT_C_FILES:$(GOS_VIRT_DIR)/%.c=$(GOS_VIRT_DIR)/%_c.o)

obj-y += drivers/
obj-y += hal/
obj-y += mm/
obj-y += app/

gos: $(GOS_OBJ_FILES)
	mkdir -p $(BUILD_DIR)
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(LD) -T ./gos.lds -o $(BUILD_DIR)/$(GOS_TARGET) $(GOS_OBJ_FILES) built-in.o -Map $(BUILD_DIR)/gos.map
	$(RISCV_COPY) $(BUILD_DIR)/$(GOS_TARGET) -O binary $(BUILD_DIR)/$(GOS_TARGET_BIN)

$(GOS_ENTRY_DIR)/%_s.o: $(GOS_ENTRY_DIR)/%.S
	$(CC) $(COPS) -I$(TOPDIR)/include -c $< -o $@

$(GOS_ENTRY_DIR)/%_c.o: $(GOS_ENTRY_DIR)/%.c
	$(CC) $(COPS) -I$(TOPDIR)/include -I$(GOS_HAL_DIR)/include -I$(APP_DIR) -c $< -o $@

$(GOS_LIB_DIR)/%_c.o: $(GOS_LIB_DIR)/%.c
	$(CC) $(COPS) -I$(TOPDIR)/include -I$(GOS_HAL_DIR)/include -c $< -o $@

$(GOS_VIRT_DIR)/%_s.o: $(GOS_VIRT_DIR)/%.S
	$(CC) $(COPS) -I$(TOPDIR)/include -c $< -o $@

$(GOS_VIRT_DIR)/%_c.o: $(GOS_VIRT_DIR)/%.c
	$(CC) $(COPS) -I$(TOPDIR)/include -I$(GOS_HAL_DIR)/include -c $< -o $@
	
clean:
	rm -rf $(shell find -name "*.o")
	rm -rf $(BUILD_DIR)
	rm -rf out
	
run:
	./qemu-system-riscv64 -nographic \
	-machine virt -smp 8 -m 8G \
	-bios out/Image.bin \

run-aia:
	./qemu-system-riscv64-debug -nographic \
        -machine virt,aia=aplic-imsic -smp 8 \
	-cpu rv64,sv39=on -m 8G \
        -bios out/Image.bin \

run-debug:
	./qemu-system-riscv64-new -nographic \
        -machine virt,aia=aplic-imsic -smp 8 \
	-cpu rv64,sv39=on -m 8G \
        -bios out/Image.bin \
	-S -s

format:
	find . -name *.c |xargs ./Lindent
	find . -name *.h |xargs ./Lindent
	find . -name *.c~ |xargs rm
	find . -name *.h~ |xargs rm
