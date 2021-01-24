/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_domain.h>
#include <sbi/sbi_math.h>
#include <platform_override.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/fdt/fdt_fixup.h>

static u64 starfive_vic7100_tlbr_flush_limit(const struct fdt_match *match)
{
	return 0;
}

static int starfive_vic7100_fdt_fixup(void *fdt, const struct fdt_match *match)
{
	fdt_reserved_memory_nomap_fixup(fdt);

	return 0;
}

static const struct fdt_match starfive_vic7100_match[] = {
	{ .compatible = "starfive,fu74-g000" },
	{ .compatible = "sifive,freedom-u74-arty" },
	{ },
};

const struct platform_override starfive_vic7100 = {
	.match_table = starfive_vic7100_match,
	.tlbr_flush_limit = starfive_vic7100_tlbr_flush_limit,
	.fdt_fixup = starfive_vic7100_fdt_fixup,
};
