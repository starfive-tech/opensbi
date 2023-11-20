/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/sbi_error.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_console.h>

static int sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 const struct sbi_trap_regs *regs,
				 unsigned long *out_val,
				 struct sbi_trap_info *out_trap)
{
	int ret = 0;

	if (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_smode(regs->a0, regs->a1);
	else if (funcid == SBI_EXT_IPI_SEND_EXT_DOMAIN)
		ret = sbi_ipi_send_ext(regs->a1, NULL, regs->a2);
	else if (funcid == SBI_EXT_IPI_SET_AMP_DATA_ADDR)
		sbi_ipi_set_amp_data_addr(regs->a0);
	else
		ret = SBI_ENOTSUPP;

	return ret;
}

struct sbi_ecall_extension ecall_ipi = {
	.extid_start = SBI_EXT_IPI,
	.extid_end = SBI_EXT_IPI,
	.handle = sbi_ecall_ipi_handler,
};
