WORK_DIR	= $(shell pwd)
BUILD_DIR       = $(WORK_DIR)/build

RTOS_SOURCE_DIR = /home/yanice/Desktop/FreeRTOS/FreeRTOS/Source
RTOS_PLUS_SOURCE_DIR = /home/yanice/Desktop/FreeRTOS/FreeRTOS-Plus/Source

TCP_SOURCE_DIR = $(RTOS_PLUS_SOURCE_DIR)/FreeRTOS-Plus-TCP
DEMO_SOURCE_DIR = $(abspath ../Common)
PORT_SOURCE_DIR = $(abspath ../../Port/GCC/ARM_AARCH64_QEMU)

CROSS   := aarch64-linux-gnu-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
ARCH    = $(CROSS)ar

VIRTIO_NET_ADDR = 0x10013000

########################################################
###
###        Select include and source file.
###
########################################################


###########################
# user files.
#
INCLUDES := -I$(WORK_DIR)
SRCS := $(wildcard $(WORK_DIR)/*.c)
ASMS := start.S vector.S
LIBS := 
DEFINES := -DVIRTIO_NET_ADDR=$(VIRTIO_NET_ADDR)
###########################
# user libs.
#
INCLUDES += -I./libmmk/os/include
LIBS +=	./libmmk/build/libmmk_s.a

INCLUDES += -I./my_stdlib/include
SRCS +=	$(wildcard my_stdlib/src/*.c)

# INCLUDES += -I./newlib/include
# LIBS +=	./newlib/lib/libc.a

INCLUDES += -I./libvirtio
SRCS +=	$(wildcard libvirtio/*.c)

###########################
# RTOS basic files.
#
SRCS +=	$(RTOS_SOURCE_DIR)/event_groups.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/stream_buffer.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/timers.c
	
INCLUDES += -I$(RTOS_SOURCE_DIR)/include

###########################
# RTOS demo files.
#
SRCS +=	$(DEMO_SOURCE_DIR)/Minimal/EventGroupsDemo.c \
	$(DEMO_SOURCE_DIR)/Minimal/TaskNotify.c \
	$(DEMO_SOURCE_DIR)/Minimal/TimerDemo.c \
	$(DEMO_SOURCE_DIR)/Minimal/blocktim.c \
	$(DEMO_SOURCE_DIR)/Minimal/dynamic.c \
	$(DEMO_SOURCE_DIR)/Minimal/recmutex.c

INCLUDES += -I$(DEMO_SOURCE_DIR)/include

###########################
# RTOS port files.
#
SRCS +=	$(PORT_SOURCE_DIR)/port.c \
	$(PORT_SOURCE_DIR)/Drivers/src/gic_v3.c \
	$(PORT_SOURCE_DIR)/Drivers/src/uart.c \
	$(PORT_SOURCE_DIR)/heap_4.c

INCLUDES += -I$(PORT_SOURCE_DIR) \
	-I$(PORT_SOURCE_DIR)/Drivers/include

ASMS +=	$(PORT_SOURCE_DIR)/portASM.S

##########################
# RTOS-Plus TCP files.
#
SRCS +=	$(wildcard $(TCP_SOURCE_DIR)/source/*.c)
INCLUDES += -I$(TCP_SOURCE_DIR)/source/include

_TCP_PORTABLE := $(TCP_SOURCE_DIR)/source/portable
SRCS +=	$(_TCP_PORTABLE)/BufferManagement/BufferAllocation_2.c \
	$(_TCP_PORTABLE)/NetworkInterface/MPS2_AN385/NetworkInterface.c \
	$(_TCP_PORTABLE)/NetworkInterface/MPS2_AN385/ether_lan9118/smsc9220_eth_drv.c

INCLUDES += -I$(_TCP_PORTABLE)/NetworkInterface/MPS2_AN385/ether_lan9118 \
		-I$(_TCP_PORTABLE)/Compiler/GCC


USE_MMK=0
DEBUG=1
TARGET = RTOSDemo

# Generate GCC_VERSION in number format
GCC_VERSION = $(shell $(CC) --version | grep ^$(CC) | sed 's/^.* //g' | awk -F. '{ printf("%d%02d%02d"), $$1, $$2, $$3 }')
GCC_VERSION_NEED_ZICSR = "110100"

ifeq ($(DEBUG), 1)
    CFLAGS += -O0 -Og -ggdb3
else
    CFLAGS += -O2
endif

ifeq ($(USE_MMK), 1)
    MMK_FILE = MMK_aarch64.bin
else
    MMK_FILE = fw_jump.elf
endif

ifeq ($(PICOLIBC), 1)
    CFLAGS += --specs=picolibc.specs -DPICOLIBC_INTEGER_PRINTF_SCANF
   LDFLAGS += --specs=picolibc.specs -DPICOLIBC_INTEGER_PRINTF_SCANF
endif

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o) $(ASMS:%.S=$(BUILD_DIR)/%.o)
DEPS = $(SRCS:%.c=$(BUILD_DIR)/%.d) $(ASMS:%.S=$(BUILD_DIR)/%.d)

CFLAGS = \
	-fno-builtin -fno-builtin-function -nostdlib \
	-ffunction-sections -fdata-sections \
	-fmessage-length=0 -fno-common -mgeneral-regs-only \
	-D USE_MMK=$(USE_MMK) \
	$(INCLUDES)

LDFLAGS = -nostartfiles -Tfake_rom.lds -fno-builtin -nostdlib \
	-Xlinker --gc-sections -fno-common -mgeneral-regs-only\
	-Xlinker --defsym=__stack_size=1000 \
	-Xlinker -Map=$(BUILD_DIR)/$(TARGET).map \
	-D USE_MMK=$(USE_MMK)

$(BUILD_DIR)/$(TARGET).elf: $(OBJS) fake_rom.lds Makefile
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) $< -O binary $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/$(TARGET).bin
	qemu-system-aarch64 -nographic \
		-machine virt -cpu cortex-a72 \
		-device loader,file=$(BUILD_DIR)/$(TARGET).bin,addr=0x40400000 \
		-kernel $(MMK_FILE) \
		-netdev user,id=net0 \
		-device virtio-net,netdev=net0,addr=$(VIRTIO_NET_ADDR)

debug: $(BUILD_DIR)/$(TARGET).bin
	qemu-system-aarch64 -nographic \
		-machine virt -cpu cortex-a72 \
		-device loader,file=$(BUILD_DIR)/$(TARGET).bin,addr=0x40400000 \
		-kernel $(MMK_FILE) \
		-s -S
gdb:
	gdb-multiarch $(BUILD_DIR)/$(TARGET).elf \
		-ex 'target remote localhost:1234'

-include $(DEPS)
