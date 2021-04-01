/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021
 * Authors: Tekkaman Ninja <tekkamanninja@163.com>
 *
 * t0 --> payload text base addr
 * t1 --> payload end addr in fw_payload.bin
 * t2 --> payload start addr in fw_payload.bin
 * t3 --> payload text end addr (ti - t2 + t0)
 * t4 --> _relocate_payload_done
 *             t2          t1         t0            t3
 * +-----------+-----------+----------+-------------+-------+
 * |   fw_base + fw_payload| -------> |*fw_payload* |       |
 * +-----------+-----------+----------+-------------+-------+
 *
 */

_relocate_payload:
	li	t0, (FW_TEXT_START + FW_PAYLOAD_OFFSET)
	lla	t1, _pl_end
	REG_L	t1, 0(t1)
	lla	t2, _pl_start
	REG_L	t2, 0(t2)

	sub	t3, t1, t2
	add	t3, t3, t0

	beq	t0, t2, _relocate_payload_done

	lla	t4, _relocate_payload_done

	lla	t5, _load_start
	REG_L	t5, 0(t5)
	sub	t4, t4, t5

	lla	t5, _link_start
	REG_L	t5, 0(t5)
	add	t4, t4, t5

	blt	t2, t0, _relocate_payload_copy_to_upper_loop

_relocate_payload_copy_to_lower_loop:
	REG_L	t3, 0(t2)
	REG_S	t3, 0(t0)
	add	t0, t0, __SIZEOF_POINTER__
	add	t2, t2, __SIZEOF_POINTER__
	blt	t2, t1, _relocate_payload_copy_to_lower_loop
	jr	t4

_relocate_payload_copy_to_upper_loop:
	add	t3, t3, -__SIZEOF_POINTER__
	add	t1, t1, -__SIZEOF_POINTER__
	REG_L	t0, 0(t1)
	REG_S	t0, 0(t3)
	blt	t2, t1, _relocate_payload_copy_to_upper_loop
	jr	t4

	.align 3
_pl_end:
	RISCV_PTR	_payload_end
_pl_start:
	RISCV_PTR	_payload_start