/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Chee Hong Ang <cheehong.ang@linux.starfivetech.com>
 *
 */

#define DUBHE_L2_CACHELINE_SIZE	0x40

	/*
	 * __sbi_dubhe_L2_inv_range(start,len)
	 *
	 * Invalidate the L2 cache within the specified region
	 *
	 * start - physical start address of region
	 * len   - size of memory region
	 */

	.align 3
	.global __sbi_dubhe_L2_inv_range
__sbi_dubhe_L2_inv_range:
	beqz	a1, 2f
	li	t0, DUBHE_L2_CACHELINE_SIZE
	addi	t1, t0, -1
	add	a1, a1, a0	/* Compute end address */
	not	t2, t1
	and	a0, a0, t2
1:
	/* CDISCARD.L2 rs1 = a0 */
	fence	rw, rw
	.insn i 0x73, 0, x0, a0, -0x3A
	fence	rw, rw
	add	a0, a0, t0
	blt	a0, a1, 1b
2:
	ret

	/*
	 * __sbi_dubhe_L2_flush_range(start,len)
	 *
	 * Flush and invalidate the L2 cache within the specified region
	 *
	 * start - physical start address of region
	 * len   - size of memory region
	 */

	.align 3
	.global __sbi_dubhe_L2_flush_range
__sbi_dubhe_L2_flush_range:
	beqz	a1, 2f
	li	t0, DUBHE_L2_CACHELINE_SIZE
	addi	t1, t0, -1
	add	a1, a1, a0	/* Compute end address */
	not	t2, t1
	and	a0, a0, t2
1:
	/* CFLUSH.L2 rs1 = a0 */
	fence	rw, rw
	.insn i 0x73, 0, x0, a0, -0x3C
	fence	rw, rw
	add	a0, a0, t0
	blt	a0, a1, 1b
2:
	ret
