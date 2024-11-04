/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * All rights reserved.
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#ifndef __CPU_H
#define __CPU_H

#ifndef __ASSEMBLER__
#define STRINGIFY(x...) #x
#define EXPAND(x) STRINGIFY(x)

static inline void barrier(void)
{
	asm volatile("" : : : "memory");
}
#define cpu_relax() barrier()

static inline void sync(void)
{
  asm volatile("isb");
}
#define mb() sync()

static inline void flush_cache(void* r, long n)
{
  asm volatile("dsb ishst");
  asm volatile("tlbi vmalle1is");
  asm volatile("dsb ish");
  asm volatile("isb");
}

#endif /* __ASSEMBLER__ */

#endif
