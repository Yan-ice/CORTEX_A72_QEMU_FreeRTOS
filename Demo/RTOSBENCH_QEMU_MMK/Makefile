WORK_DIR	= $(shell pwd)
BUILD_DIR       = $(WORK_DIR)/build

RTOS_SOURCE_DIR = ~/Desktop/FreeRTOS/FreeRTOS/Source
DEMO_SOURCE_DIR = $(abspath ../Common)
PORT_SOURCE_DIR = $(abspath ../../Port/GCC/ARM_AARCH64_QEMU)

USE_MMK=1
DEBUG=1

##################################
# NO Bench config.
NOBENCH_SOURCE_DIR = $(abspath ./RTOSBench)

# select a specific test file.

# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/context-switch/round_robin.c

NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/interrupt/interrupt_processing.c

# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/mq/mq_processing.c
# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/mq/mq.c

#NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/mq/mq_workload.c
############## MQ workload works very slow. ################
# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/mutex/mutex.c
# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/mutex/mutex_pip.c
# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/mutex/mutex_processing.c
# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/mutex/mutex_workload.c

# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/partition/jitter.c

# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/semaphore/sem.c
# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/semaphore/sem_prio.c
# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/semaphore/sem_workload.c
# NO_TEST_SRC ?= $(NOBENCH_SOURCE_DIR)/tests/semaphore/sem_processing.c

NO_BASE_PRIO = 10
NO_AFFINITY = 7
NO_BUFFER_SIZE = 512
NO_TRACING = $(NOBENCH_SOURCE_DIR)/tracing

##################################

CROSS   := aarch64-linux-gnu-
CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
ARCH    = $(CROSS)ar

TARGET = kernel7

# Generate GCC_VERSION in number format
GCC_VERSION = $(shell $(CC) --version | grep ^$(CC) | sed 's/^.* //g' | awk -F. '{ printf("%d%02d%02d"), $$1, $$2, $$3 }')
GCC_VERSION_NEED_ZICSR = "110100"

CFLAGS = \
	-fno-builtin -fno-builtin-function -nostdlib \
	-ffunction-sections -fdata-sections \
	-fmessage-length=0 -fno-common -mgeneral-regs-only \
	-I $(WORK_DIR) \
	-I ./libmmk/os/include \
       	-I ./my_stdlib/include \
       	-I $(DEMO_SOURCE_DIR)/include \
	-I $(RTOS_SOURCE_DIR)/include \
	-I $(NOBENCH_SOURCE_DIR)/include \
	-I $(PORT_SOURCE_DIR) \
	-I $(PORT_SOURCE_DIR)/Drivers/include \
	-I $(NO_TRACING) \
	-D USE_MMK=$(USE_MMK) \
	-D NO_BUFFER_SIZE=$(NO_BUFFER_SIZE) \
	# -D TRACING

LDFLAGS = -nostartfiles -Tfake_rom.lds -fno-builtin -nostdlib \
	-Xlinker --gc-sections -fno-common -mgeneral-regs-only\
	-Xlinker --defsym=__stack_size=1000 \
	-Xlinker -Map=$(BUILD_DIR)/RTOSDemo.map \
	-D USE_MMK=$(USE_MMK)

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

SRCS = freertos_porting_layer.c \
	$(wildcard ./my_stdlib/src/*.c) \
	$(DEMO_SOURCE_DIR)/Minimal/EventGroupsDemo.c \
	$(DEMO_SOURCE_DIR)/Minimal/TaskNotify.c \
	$(DEMO_SOURCE_DIR)/Minimal/TimerDemo.c \
	$(DEMO_SOURCE_DIR)/Minimal/blocktim.c \
	$(DEMO_SOURCE_DIR)/Minimal/dynamic.c \
	$(DEMO_SOURCE_DIR)/Minimal/recmutex.c \
	$(RTOS_SOURCE_DIR)/event_groups.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/stream_buffer.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/timers.c \
	$(PORT_SOURCE_DIR)/heap_4.c \
	$(PORT_SOURCE_DIR)/port.c \
	$(PORT_SOURCE_DIR)/Drivers/src/gic_v3.c \
	$(PORT_SOURCE_DIR)/Drivers/src/uart.c \
	$(NO_TEST_SRC) \
	$(NO_TRACING)/trace.c

ASMS = start.S vector.S \
	$(PORT_SOURCE_DIR)/portASM.S

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o) $(ASMS:%.S=$(BUILD_DIR)/%.o)
DEPS = $(SRCS:%.c=$(BUILD_DIR)/%.d) $(ASMS:%.S=$(BUILD_DIR)/%.d)

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
		-kernel $(MMK_FILE)

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
