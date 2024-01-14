#
# SPDX-License-Identifier: BSD-2-Clause
#

carray-platform_override_modules-$(CONFIG_PLATFORM_STARFIVE_JH7110) += starfive_jh7110
carray-platform_override_modules-y += starfive_dubhe
platform-objs-$(CONFIG_PLATFORM_STARFIVE_JH7110) += starfive/jh7110.o
platform-objs-y += starfive/dubhe_cache.o
platform-objs-y += starfive/dubhe.o
