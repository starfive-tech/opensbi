#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (c) 2020 Shanghai StarFive Technology Co., Ltd.
#
# Authors:
#   StarFive <support@starfivetech.com>
#

# Compiler flags
platform-cppflags-y =
platform-cflags-y =
platform-asflags-y =
platform-ldflags-y =

# Command for platform specific "make run"
# platform-runcmd = qemu-system-riscv$(PLATFORM_RISCV_XLEN) -M sifive_u -m 256M \
#  -nographic -bios $(build_dir)/platform/starfive/vic7100/firmware/fw_payload.elf

# Blobs to build
FW_TEXT_START=0x80000000
#FW_DYNAMIC=y

#FW_JUMP=y
#ifeq ($(PLATFORM_RISCV_XLEN), 32)
  # This needs to be 4MB aligned for 32-bit system
#  FW_JUMP_ADDR=0x80400000
#else
  # This needs to be 2MB aligned for 64-bit system
#  FW_JUMP_ADDR=0x80200000
#endif
#FW_JUMP_FDT_ADDR=0x88000000

FW_PAYLOAD=y
#ifeq ($(PLATFORM_RISCV_XLEN), 32)
  # This needs to be 4MB aligned for 32-bit system
#  FW_PAYLOAD_OFFSET=0x400000
#else
  # This needs to be 2MB aligned for 64-bit system
  FW_PAYLOAD_OFFSET=0x200000
#endif
FW_PAYLOAD_FDT_ADDR=0x88000000

FW_PAYLOAD_ALIGN=0x1000
