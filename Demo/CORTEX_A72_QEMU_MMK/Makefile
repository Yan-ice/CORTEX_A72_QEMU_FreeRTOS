BUILD_DIR       = build
RTOS_SOURCE_DIR = ~/Desktop/FreeRTOS/FreeRTOS/Source
DEMO_SOURCE_DIR = $(abspath ../Common/Minimal)
PORT_SOURCE_DIR = $(abspath ../../Port)

CROSS   := aarch64-linux-gnu-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
ARCH    = $(CROSS)ar

DEBUG=1

# Generate GCC_VERSION in number format
GCC_VERSION = $(shell $(CC) --version | grep ^$(CC) | sed 's/^.* //g' | awk -F. '{ printf("%d%02d%02d"), $$1, $$2, $$3 }')
GCC_VERSION_NEED_ZICSR = "110100"

CPPFLAGS = -fno-builtin -fno-builtin-function -nostdlib \
	-DportasmHANDLE_INTERRUPT=handle_trap \
	-I . -I ../Common/include -I ./libmmk/os/include \
	-I $(RTOS_SOURCE_DIR)/include \
	-I $(PORT_SOURCE_DIR)/GCC/ARM_AARCH64_QEMU

CFLAGS  = -Wall -fno-builtin -fno-builtin-function -nostdlib \
	-fmessage-length=0 -fno-common \
	-ffunction-sections \
	-fdata-sections

LDFLAGS = -nostartfiles -Tfake_rom.lds -fno-builtin -nostdlib \
	-Xlinker --gc-sections -fno-common \
	-Xlinker --defsym=__stack_size=400 \
	-Xlinker -Map=$(BUILD_DIR)/RTOSDemo.map

ifeq ($(DEBUG), 1)
    CFLAGS += -O0 -Og -ggdb3
else
    CFLAGS += -O2
endif

ifeq ($(PICOLIBC), 1)
    CFLAGS += --specs=picolibc.specs -DPICOLIBC_INTEGER_PRINTF_SCANF
   LDFLAGS += --specs=picolibc.specs -DPICOLIBC_INTEGER_PRINTF_SCANF
endif

SRCS = main.c main_blinky.c riscv-virt.c uart.c spinlock.c console.c gic_v3.c my_mem.c\
	$(DEMO_SOURCE_DIR)/EventGroupsDemo.c \
	$(DEMO_SOURCE_DIR)/TaskNotify.c \
	$(DEMO_SOURCE_DIR)/TimerDemo.c \
	$(DEMO_SOURCE_DIR)/blocktim.c \
	$(DEMO_SOURCE_DIR)/dynamic.c \
	$(DEMO_SOURCE_DIR)/recmutex.c \
	$(RTOS_SOURCE_DIR)/event_groups.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/stream_buffer.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/timers.c \
	$(PORT_SOURCE_DIR)/MemMang/heap_4.c \
	$(PORT_SOURCE_DIR)/GCC/ARM_AARCH64_QEMU/port.c \
	libmmk/os/src/mmk.c

ASMS = start.S vector.S \
	$(PORT_SOURCE_DIR)/GCC/ARM_AARCH64_QEMU/portASM.S

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o) $(ASMS:%.S=$(BUILD_DIR)/%.o)
DEPS = $(SRCS:%.c=$(BUILD_DIR)/%.d) $(ASMS:%.S=$(BUILD_DIR)/%.d)

$(BUILD_DIR)/RTOSDemo.elf: $(OBJS) fake_rom.lds Makefile
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/RTOSDemo.bin: $(BUILD_DIR)/RTOSDemo.elf
	$(OBJCOPY) $< -O binary $(BUILD_DIR)/RTOSDemo.bin

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/RTOSDemo.bin
	qemu-system-aarch64 -nographic \
		-machine virt -cpu cortex-a72 \
		-device loader,file=$(BUILD_DIR)/RTOSDemo.bin,addr=0x40400000 \
		-kernel MMK_aarch64.bin

debug: $(BUILD_DIR)/RTOSDemo.bin
	qemu-system-aarch64 -nographic \
		-machine virt -cpu cortex-a72 \
		-device loader,file=$(BUILD_DIR)/RTOSDemo.bin,addr=0x40400000 \
		-kernel MMK_aarch64.bin \
		-s -S
gdb:
	gdb-multiarch $(BUILD_DIR)/RTOSDemo.elf \
		-ex 'target remote localhost:1234'

-include $(DEPS)
