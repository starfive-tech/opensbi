/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 *
 */

#ifndef _STARFIVE_DUBHE_H_
#define _STARFIVE_DUBHE_H_

#define CSR_HART_CTRL			0xbd0
#define CSR_HART_PAUSE_EN_OFFSET	11
#define CSR_HART_PAUSE_EN_MASK		BIT(CSR_HART_PAUSE_EN_OFFSET)

enum sbi_ext_dubhe_cmo_fid {
	SBI_EXT_STARFIVE_DUBHE_L2_FLUSH = 0,
	SBI_EXT_STARFIVE_DUBHE_L2_INVALIDATE,
};

#endif /* _STARFIVE_DUBHE_H_ */
