CROSS_COMPILE := aarch64-linux-gnu-

AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)ld
CC := $(CROSS_COMPILE)gcc

SRCS := $(wildcard src/*.c)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

CFLAGS = -fno-builtin -fno-builtin-function -nostdlib \
        -ffunction-sections -fdata-sections \
        -fno-common -mgeneral-regs-only \
        -I src \

libslirp.a: $(OBJS)
	$(AR) rcs libslirp.a $(OBJS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
