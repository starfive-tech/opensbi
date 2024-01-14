/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 StarFive Technology Co., Ltd.
 *
 * Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
 *
 */

#ifndef _DUBHE_CACHE_H_
#define _DUBHE_CACHE_H_

#include <sbi/sbi_types.h>

void __sbi_dubhe_L2_flush_range(uint64_t start, uint64_t len);
void __sbi_dubhe_L2_inv_range(uint64_t start, uint64_t len);

#endif /* _DUBHE_CACHE_H_ */
