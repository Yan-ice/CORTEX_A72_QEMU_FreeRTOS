/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://www.github.com/FreeRTOS
 *
 */

#include "param.h"
#include "mmkdef.h"

	.section .init
	.globl _start
	.type _start,@function
_start:
	.cfi_startproc

	// Continue primary hart
	MOV	x0, #0
	MOV	x1, #PRIM_HART

	CMP	x0, x1
	B.NE secondary

	// Primary hart
	LDR x28, =_stack_top
	MOV sp, x28
	
	B down_el1

//////////////////////////////////
// if exception level higher than EL1, down to EL1

down_el3:
	MOV x0, #0x3C0
	MSR spsr_el3, x0
	ADR x0, down_el2
	MSR elr_el3, x0
	eret
down_el2:
	MOV x0, #0x3C0
	MSR spsr_el2, x0
	ADR x0, down_el1
	MSR elr_el2, x0
	eret
down_el1:

//////////////////////////////////
// use MMK API to load memory space

	MOV x0, #USE_MMK
	MOV x1, 1
	CMP x0, x1
	B.NE no_mmk
	
	MOV x3, MAP_IDENTICAL
	MOV x7, NKAPI_ALLOC

	//mmk area
	MOV x0, 0
	LDR x1, =0x40000
	LDR x2, =0x40400
	MOV x4, MAP_PERM_R | MAP_PERM_W | MAP_PERM_X
	SUB x2, x2, x1
	MOV x28, -0x1000
	BLR x28

	//rom area
	MOV x0, 0
	LDR x1, =0x40400
	LDR x2, =0x40480
	MOV x4, MAP_PERM_R | MAP_PERM_W | MAP_PERM_X
	SUB x2, x2, x1
	MOV x28, -0x1000
	BLR x28

	//ram area
	MOV x0, 0
	LDR x1, =0x40480
	LDR x2, =0x40500
	MOV x4, MAP_PERM_R | MAP_PERM_W | MAP_PERM_X
	SUB x2, x2, x1
	MOV x28, -0x1000
	BLR x28

	//gic area
	MOV x0, 0
	LDR x1, =0x08000
	LDR x2, =0x08020
	MOV x4, MAP_PERM_R | MAP_PERM_W
	SUB x2, x2, x1
	MOV x28, -0x1000
	BLR x28

	//uart area
	MOV x0, 0
	LDR x1, =0x09000
	LDR x2, =0x09001
	MOV x4, MAP_PERM_R | MAP_PERM_W
	SUB x2, x2, x1
	MOV x28, -0x1000
	BLR x28

	//mmio area
	MOV x0, 0
	LDR x1, =0x0a000
	LDR x2, =0x0a010
	MOV x4, MAP_PERM_R | MAP_PERM_W
	SUB x2, x2, x1
	MOV x28, -0x1000
	BLR x28

no_mmk:

//////////////////////////////////
// Load data section

load_data_sec:
	ADR x0, _data_lma
	ADR x1, _data
	ADR x2, _edata

	CMP x1, x2
	B.GE while1_end

while1:
	LOAD x28, [x0]
	STOR x28, [x1]
	ADD x0, x0, REGSIZE
	ADD x1, x1, REGSIZE
	
	CMP x1, x2
	B.LT while1
while1_end:

//////////////////////////////////

//////////////////////////////////
// Load rodata section

load_rodata_sec:
	ADR x0, _rodata_lma
	ADR x1, _rodata
	ADR x2, _erodata

	CMP x1, x2
	B.GE while2_end

while2:
	LOAD x28, [x0]
	STOR x28, [x1]
	ADD x0, x0, REGSIZE
	ADD x1, x1, REGSIZE
	
	CMP x1, x2
	B.LT while2
while2_end:

//////////////////////////////////

//////////////////////////////////
// Clear bss section

	LDR x0, =_bss
	LDR x1, =_ebss

	CMP x0, x1
	B.GE while3_end
while3:
	STOR XZR, [x0]
	ADD x0, x0, REGSIZE
	
	CMP x0, x1
	B.LE while3
while3_end:

//////////////////////////////////

//////////////////////////////////
// Setup of interrupt

	ADR x0, _freertos_vector_table
	MSR VBAR_EL1, x0

//	MRS x28, cntv_ctl_el0
//	ORR x28, x28, 0b11
//	MSR cntv_ctl_el0, x28

//	MOV x28, 0x10000
//	MSR cntv_tval_el0, x28

//////////////////////////////////


/////////////////////////////////
// modify CPACR_EL1 to allow floating point 

	MRS x0, CPACR_EL1  // 读取 CPACR_EL1
	ORR x0, x0, #0x300000  // 设置 FPEN 位为 0b11，允许 EL0 和 EL1 访问浮点和 SIMD
	MSR CPACR_EL1, x0  // 写回 CPACR_EL1

	MOV x9, #0x40000
	str		x9, [x9, 0]
	ldr     q0, [x9, 0]

//////////////////////////////////


	// argc, argv, envp is 0
	MOV  x0, #0
	MOV  x1, #0
	MOV  x2, #0
	BL main

deadlock:
	MOV x0, #0x233
	B deadlock

secondary:
	// TODO: Multicore is not supported
	MOV x0, #0x665
	B secondary
	.cfi_endproc

	
.section .rodata
