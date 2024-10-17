CROSS_COMPILE := aarch64-linux-gnu-

AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)ld
CC := $(CROSS_COMPILE)gcc

SRCS := $(wildcard src/*.c)
OBJS = $(SRCS:%.c=%.o)

TARGET = mystdlib

CFLAGS = -fno-builtin -fno-builtin-function -nostdlib \
        -ffunction-sections -fdata-sections \
        -fno-common -mgeneral-regs-only \
        -I include \

lib/lib$(TARGET).a: $(OBJS)
	$(AR) rcs lib/lib$(TARGET).a $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
