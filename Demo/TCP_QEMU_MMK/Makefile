WORK_DIR	= $(shell pwd)
BUILD_DIR       = $(WORK_DIR)/build

RTOS_SOURCE_DIR = /home/yanice/Desktop/CORTEX_A72_QEMU_FreeRTOS/FreeRTOS/FreeRTOS/Source
RTOS_PLUS_SOURCE_DIR = /home/yanice/Desktop/CORTEX_A72_QEMU_FreeRTOS/FreeRTOS/FreeRTOS-Plus/Source

TCP_SOURCE_DIR = $(RTOS_PLUS_SOURCE_DIR)/FreeRTOS-Plus-TCP
DEMO_SOURCE_DIR = $(abspath ../Common)
PORT_SOURCE_DIR = $(abspath ../../Port/GCC/ARM_AARCH64_QEMU)

CROSS   := /home/yanice/Desktop/CORTEX_A72_QEMU_FreeRTOS/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin/aarch64-none-elf-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
ARCH    = $(CROSS)ar

VIRTIO_NET_ADDR = 0x20000

########################################################
###
###        Select include and source file.
###
########################################################


###########################
# user files.
#
INCLUDES := $(WORK_DIR)
SRCS := $(wildcard $(WORK_DIR)/*.c)
ASMS := start.S vector.S
LIBS := 
DEFINES := VIRTIO_NET_ADDR=$(VIRTIO_NET_ADDR)

###########################
# user libs.
#
INCLUDES += $(WORK_DIR)/libmmk/os/include
LIBS +=	$(WORK_DIR)/libmmk/build/libmmk_s.a

INCLUDES += $(WORK_DIR)/my_util/include
SRCS +=	$(wildcard my_util/src/*.c)

INCLUDES += $(WORK_DIR)/posix/include $(WORK_DIR)/posix/internal_include # $(WORK_DIR)/posix/include/FreeRTOS_POSIX

INCLUDES += $(WORK_DIR)/include_sys
# copy from /usr/lib/gcc-cross/aarch64-linux-gnu/9

SRCS +=	$(wildcard posix/src/*.c)

###########################
# picolibc.
#
INCLUDES += $(WORK_DIR)/piconewlib/include
LIBS += -L$(WORK_DIR)/piconewlib/lib -lg -lc -lm -lgcc

###########################
# libvirtio.
#
INCLUDES += $(WORK_DIR)/libvirtio
SRCS += $(wildcard $(WORK_DIR)/libvirtio/*.c)

###########################
# RTOS basic files.
#
SRCS +=	$(RTOS_SOURCE_DIR)/event_groups.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/stream_buffer.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/timers.c
	
INCLUDES += $(RTOS_SOURCE_DIR)/include

###########################
# RTOS demo files.
#
SRCS +=	$(DEMO_SOURCE_DIR)/Minimal/EventGroupsDemo.c \
	$(DEMO_SOURCE_DIR)/Minimal/TaskNotify.c \
	$(DEMO_SOURCE_DIR)/Minimal/TimerDemo.c \
	$(DEMO_SOURCE_DIR)/Minimal/blocktim.c \
	$(DEMO_SOURCE_DIR)/Minimal/dynamic.c \
	$(DEMO_SOURCE_DIR)/Minimal/recmutex.c

INCLUDES += $(DEMO_SOURCE_DIR)/include

###########################
# RTOS port files.
#
SRCS +=	$(PORT_SOURCE_DIR)/port.c \
	$(PORT_SOURCE_DIR)/Drivers/src/gic_v3.c \
	$(PORT_SOURCE_DIR)/Drivers/src/uart.c \
	$(PORT_SOURCE_DIR)/heap_4.c

INCLUDES += $(PORT_SOURCE_DIR) \
	$(PORT_SOURCE_DIR)/Drivers/include

ASMS +=	$(PORT_SOURCE_DIR)/portASM.S

##########################
# RTOS-Plus TCP files.
#
SRCS +=	$(wildcard $(TCP_SOURCE_DIR)/source/*.c)
INCLUDES += $(TCP_SOURCE_DIR)/source/include

_TCP_PORTABLE := $(TCP_SOURCE_DIR)/source/portable
SRCS +=	$(_TCP_PORTABLE)/BufferManagement/BufferAllocation_2.c 

INCLUDES += $(_TCP_PORTABLE)/NetworkInterface/MPS2_AN385/ether_lan9118 \
		$(_TCP_PORTABLE)/Compiler/GCC

##########################
# RTOS-Plus FAT files.
#
FAT_SOURCE_DIR = $(WORK_DIR)/fat

SRCS +=	$(wildcard $(FAT_SOURCE_DIR)/*.c)
INCLUDES += $(FAT_SOURCE_DIR)/include

_FAT_PORTABLE := $(FAT_SOURCE_DIR)/portable

SRCS +=	$(_FAT_PORTABLE)/common/ff_ramdisk.c
INCLUDES += $(_FAT_PORTABLE)/common



INCLUDE_FLAG = $(addprefix -I, $(INCLUDES))
DEFINE_FLAG = $(addprefix -D, $(DEFINES))

USE_MMK=0
DEBUG=1
TARGET = RTOSDemo

# Generate GCC_VERSION in number format
GCC_VERSION = $(shell $(CC) --version | grep ^$(CC) | sed 's/^.* //g' | awk -F. '{ printf("%d%02d%02d"), $$1, $$2, $$3 }')
GCC_VERSION_NEED_ZICSR = "110100"

ifeq ($(USE_MMK), 1)
    MMK_FILE = MMK_aarch64.bin
else
    MMK_FILE = fw_jump.elf
endif

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o) $(ASMS:%.S=$(BUILD_DIR)/%.o)
DEPS = $(SRCS:%.c=$(BUILD_DIR)/%.d) $(ASMS:%.S=$(BUILD_DIR)/%.d)

CFLAGS = \
	-fno-builtin -fno-builtin-functions -nostdlib -nostdinc -fno-inline \
	-ffunction-sections -fdata-sections -fno-stack-protector \
	-fno-common -mgeneral-regs-only -nostdlib\
	-D USE_MMK=$(USE_MMK) \
	$(INCLUDE_FLAG)

LDFLAGS = -nostartfiles -Tfake_rom.lds -fno-builtin -nostdlib -nostdinc -fno-inline\
	-Xlinker --gc-sections -fno-common -mgeneral-regs-only -fno-stack-protector\
	-Xlinker --defsym=__stack_size=1000 \
	-Xlinker -Map=$(BUILD_DIR)/$(TARGET).map \
	-D USE_MMK=$(USE_MMK) \
	$(LIBS)

ifeq ($(DEBUG), 1)
    CFLAGS += -O0 -Og -ggdb3
else
    CFLAGS += -O2
endif

ifeq ($(PICOLIBC), 1)
    CFLAGS += --specs=picolibc.specs -DPICOLIBC_INTEGER_PRINTF_SCANF -D__SSP_FORTIFY_LEVEL=0
   LDFLAGS += --specs=picolibc.specs -DPICOLIBC_INTEGER_PRINTF_SCANF -D__SSP_FORTIFY_LEVEL=0
endif



build: $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/$(TARGET).elf: $(OBJS) fake_rom.lds Makefile
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) $< -O binary $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# move all includes into one folder, for vscode use.

copy_include:
	rm -rf all_includes
	mkdir all_includes
	$(foreach include_dir, $(INCLUDES), \
		echo "Processing $(include_dir)"; \
		find $(include_dir) -name "*.h" | xargs -I {} cp {} all_includes; \
	)

clean:
	rm -rf $(BUILD_DIR)

test: $(BUILD_DIR)/$(TARGET).bin
	qemu-system-aarch64 -nographic \
		-machine virt -cpu cortex-a72 \
		-device loader,file=$(BUILD_DIR)/$(TARGET).bin,addr=0x40400000 \
		-kernel $(MMK_FILE) \
		-netdev user,net=192.168.31.0/24,host=192.168.31.1,hostname=qemu,id=net0,hostfwd=tcp::2333-:2333\
		-device virtio-net-device,netdev=net0

debug: $(BUILD_DIR)/$(TARGET).bin
	qemu-system-aarch64 -nographic \
		-machine virt -cpu cortex-a72 \
		-device loader,file=$(BUILD_DIR)/$(TARGET).bin,addr=0x40400000 \
		-kernel $(MMK_FILE) \
		-netdev user,net=192.168.31.0/24,host=192.168.31.1,hostname=qemu,id=net0,hostfwd=tcp::2333-:2333\
		-device virtio-net-device,netdev=net0 \
		-s -S

gdb:
	gdb-multiarch $(BUILD_DIR)/$(TARGET).elf \
		-ex 'target remote localhost:1234'

-include $(DEPS)
