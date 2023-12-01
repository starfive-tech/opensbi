/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 *
 */

#include <cache.h>
#include <dubhe.h>
#include <platform_override.h>
#include <sbi/sbi_console.h>
#include <sbi_utils/fdt/fdt_helper.h>

static int starfive_vendor_ext_provider(long extid, long funcid,
					const struct sbi_trap_regs *regs,
					unsigned long *out_value,
					struct sbi_trap_info *out_trap,
					const struct fdt_match *match)
{
	int ret = 0;

	switch (funcid) {
	case SBI_EXT_STARFIVE_DUBHE_L2_FLUSH:
		__sbi_dubhe_L2_flush_range(regs->a0, regs->a1);
		break;
	case SBI_EXT_STARFIVE_DUBHE_L2_INVALIDATE:
		__sbi_dubhe_L2_inv_range(regs->a0, regs->a1);
		break;
	default:
		sbi_printf("Unsupported vendor sbi call : %ld\n", funcid);
		asm volatile("ebreak");
	}

	return ret;
}

static const struct fdt_match starfive_dubhe_match[] = {
	{ .compatible = "starfive,dubhe" },
	{ },
};

const struct platform_override starfive_dubhe = {
	.match_table = starfive_dubhe_match,
	.vendor_ext_provider = starfive_vendor_ext_provider,
};
