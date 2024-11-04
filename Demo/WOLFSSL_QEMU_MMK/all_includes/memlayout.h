// Physical memory layout

// qemu -machine virt is set up like this,
// based on qemu's hw/arm/virt.c:
//
// 00000000 -- boot ROM, provided by qemu
// 08000000 -- GICv2
// 09000000 -- uart0 
// 0a000000 -- virtio disk 
// 40000000 -- boot ROM jumps here in machine mode
//             -kernel loads the kernel here
// unused RAM after 40000000.

// the kernel uses physical memory thus:
// 40000000 -- entry.S, then kernel text and data
// end -- start of kernel page allocation area
// PHYSTOP -- end RAM used by the kernel

#ifndef _MEMLAYOUT_H
#define _MEMLAYOUT_H

#define EXTMEM    0x40000000L               // Start of extended memory
#define PHYSTOP   (EXTMEM + 128*1024*1024)  // Top physical memory

// qemu puts UART registers here in physical memory.
#define UART0 (0x09000000L)
#define UART0_IRQ 33

// virtio mmio interface
#define VIRTIO0  (0x0a000000L)

// interrupt controller GICv3
#define QEMU_VIRT_GIC_BASE			(0x08000000)
#define QEMU_VIRT_GIC_INT_MAX		(64)
#define QEMU_VIRT_GIC_PRIO_MAX		(16)
#define GICV3_REDIST  (0x080a0000L)
/* SGI: Interrupt IDs 0-15 */
/* PPI: Interrupt IDs 16-31 */
/* SPI: Interrupt IDs 32-63 */
#define QEMU_VIRT_GIC_INTNO_SGIO	(0)
#define QEMU_VIRT_GIC_INTNO_PPIO	(16)
#define QEMU_VIRT_GIC_INTNO_SPIO	(32)

#define GIC_BASE					(QEMU_VIRT_GIC_BASE)
#define GIC_INT_MAX					(QEMU_VIRT_GIC_INT_MAX)
#define GIC_PRIO_MAX				(QEMU_VIRT_GIC_PRIO_MAX)
#define GIC_INTNO_SGI0				(QEMU_VIRT_GIC_INTNO_SGIO)
#define GIC_INTNO_PPI0				(QEMU_VIRT_GIC_INTNO_PPIO)
#define GIC_INTNO_SPI0				(QEMU_VIRT_GIC_INTNO_SPIO)

#define GIC_PRI_SHIFT				(4)
#define GIC_PRI_MASK				(0x0f)

#define VIRTIO0_IRQ                 (48)
#define TIMER0_IRQ					(27)  /** Timer IRQ  */

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#define KSTACK(p) (MAXVA - ((p)+1) * 2*PGSIZE)

#endif