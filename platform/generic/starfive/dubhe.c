/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 *
 */

#include <cache.h>
#include <dubhe.h>
#include <platform_override.h>
#include <sbi/riscv_asm.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_error.h>
#include <sbi_utils/fdt/fdt_helper.h>

static void starfive_dubhe_pause_enable(void)
{
	csr_set(CSR_HART_CTRL, CSR_HART_PAUSE_EN_MASK);
}

static int starfive_dubhe_extensions_init(const struct fdt_match *match,
					   struct sbi_hart_features *hfeatures)
{
	starfive_dubhe_pause_enable();
	return 0;
}

static int starfive_vendor_ext_provider(long funcid,
					struct sbi_trap_regs *regs,
					struct sbi_ecall_return *out,
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
		return SBI_EINVAL;
	}

	return ret;
}

static const struct fdt_match starfive_dubhe_match[] = {
	{ .compatible = "starfive,dubhe-90" },
	{ .compatible = "starfive,dubhe-80" },
	{ },
};

const struct platform_override starfive_dubhe = {
	.match_table = starfive_dubhe_match,
	.extensions_init = starfive_dubhe_extensions_init,
	.vendor_ext_provider = starfive_vendor_ext_provider,
};
