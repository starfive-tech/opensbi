#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2022 StarFive Technology Co., Ltd.
#
# Author: Jun Liang Tan <junliang.tan@linux.starfivetech.com>
#

carray-platform_override_modules-y += starfive_dubhe
platform-objs-y += starfive/dubhe_cache.o
platform-objs-y += starfive/dubhe.o
