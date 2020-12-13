/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
# Copyright (c) 2020 Shanghai StarFive Technology Co., Ltd.
#
# Authors:
#   StarFive <support@starfivetech.com>
 */

#include <libfdt.h>
#include <sbi/riscv_asm.h>
#include <sbi/riscv_io.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_math.h>
#include <sbi/sbi_platform.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/irqchip/plic.h>
#include <sbi_utils/serial/uart8250.h>
#include <sbi_utils/sys/clint.h>

/* clang-format off */

#define VIC7100_HART_COUNT					2

#define VIC7100_SYS_CLK						1000000000

#define VIC7100_CLINT_ADDR					0x2000000

#define VIC7100_PLIC_ADDR					0xc000000
#define VIC7100_PLIC_NUM_SOURCES			0x83
#define VIC7100_PLIC_NUM_PRIORITIES			7

#define VIC7100_UART0_ADDR					0x12440000
#define VIC7100_UART_BAUDRATE				115200
#define VIC7100_UART_REG_SHIFT				2
#define VIC7100_UART_REG_WIDTH				4

#define VIC7100_FULLMUX_BASE_ADDR		0x11910000UL
/* GPIO 63 for reset on BeagleV */
#define VIC7100_RESET_DOUT_ADDR			(VIC7100_FULLMUX_BASE_ADDR + 0x248)
#define VIC7100_RESET_DOEN_ADDR			(VIC7100_FULLMUX_BASE_ADDR + 0x24C)

/* Full tlb flush always */
#define VIC7100_TLB_RANGE_FLUSH_LIMIT		0

/* Example for future use */
enum sbi_ext_starfive_fid {
	SBI_EXT_STARFIVE_RDTIME = 0,
	SBI_EXT_STARFIVE_GET_MTIME,
};

/* clang-format on */

#define ROOT_FW_REGION		0
#define ROOT_ALL_REGION		1
#define ROOT_END_REGION		2
static struct sbi_domain_memregion vic7100_root_memregs[ROOT_END_REGION + 1] = { 0 };

static struct plic_data plic = {
	.addr = VIC7100_PLIC_ADDR,
	.num_src = VIC7100_PLIC_NUM_SOURCES,
};

static struct clint_data clint = {
	.addr = VIC7100_CLINT_ADDR,
	.first_hartid = 0,
	.hart_count = VIC7100_HART_COUNT,
	.has_64bit_mmio = TRUE,
};

static uintptr_t mcall_print_mtime(unsigned long addr)
{
	u64 time = readq_relaxed((volatile u64 *)addr);

	sbi_printf("read clint mtime from sbi: 0x%lx\n", time);
	return 0;
}

static void vic7100_modify_dt(void *fdt)
{
	fdt_cpu_fixup(fdt);

	fdt_fixups(fdt);

	/*
	 * SiFive Freedom U540 has an erratum that prevents S-mode software
	 * to access a PMP protected region using 1GB page table mapping, so
	 * always add the no-map attribute on this platform.
	 */
	fdt_reserved_memory_nomap_fixup(fdt);
}

static int vic7100_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	vic7100_modify_dt(fdt);

	return 0;
}

static int vic7100_console_init(void)
{
	return uart8250_init(VIC7100_UART0_ADDR, VIC7100_SYS_CLK / 10,
						 VIC7100_UART_BAUDRATE,
						 VIC7100_UART_REG_SHIFT, VIC7100_UART_REG_WIDTH);
}

static int vic7100_irqchip_init(bool cold_boot)
{
	int rc;
	u32 hartid = current_hartid();

	if (cold_boot) {
		rc = plic_cold_irqchip_init(&plic);
		if (rc)
			return rc;
	}

	return plic_warm_irqchip_init(&plic, (hartid) ? (2 * hartid - 1) : 0,
				      (hartid) ? (2 * hartid) : -1);
}

static int vic7100_ipi_init(bool cold_boot)
{
	int rc;

	if (cold_boot) {
		rc = clint_cold_ipi_init(&clint);
		if (rc)
			return rc;
	}

	return clint_warm_ipi_init();
}

static u64 vic7100_get_tlbr_flush_limit(void)
{
	return VIC7100_TLB_RANGE_FLUSH_LIMIT;
}

static int vic7100_timer_init(bool cold_boot)
{
	int rc;

	if (cold_boot) {
		rc = clint_cold_timer_init(&clint, NULL);
		if (rc)
			return rc;
	}

	return clint_warm_timer_init();
}

static struct sbi_domain_memregion *vic7100_domains_init(void)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	/* Root domain firmware memory region */
	vic7100_root_memregs[ROOT_FW_REGION].order = log2roundup(scratch->fw_size);
	vic7100_root_memregs[ROOT_FW_REGION].base = scratch->fw_start &
				~((1UL << vic7100_root_memregs[0].order) - 1UL);
	vic7100_root_memregs[ROOT_FW_REGION].flags = 0;

	/* Root domain allow everything memory region */
	vic7100_root_memregs[ROOT_ALL_REGION].order = 39;
	vic7100_root_memregs[ROOT_ALL_REGION].base = 0;
	vic7100_root_memregs[ROOT_ALL_REGION].flags = (SBI_DOMAIN_MEMREGION_READABLE |
						SBI_DOMAIN_MEMREGION_WRITEABLE |
						SBI_DOMAIN_MEMREGION_EXECUTABLE);

	/* Root domain memory region end */
	vic7100_root_memregs[ROOT_END_REGION].order = 0;
	return vic7100_root_memregs;
}

static int vic7100_system_reset_check(u32 type, u32 reason)
{
	return 1;
}

static void vic7100_system_reset(u32 type, u32 reason)
{
	/* Reset system by GPIO */

	sbi_printf("vic7100_system_reset_0\n");

	u32 value = readq_relaxed((volatile u32 *)VIC7100_RESET_DOEN_ADDR);
	value &= ~((0x1 << 31) + 0xFF);
	writeq_relaxed(value, (volatile u32 *)VIC7100_RESET_DOEN_ADDR);

	value = readq_relaxed((volatile u32 *)VIC7100_RESET_DOUT_ADDR);
	value &= ~((0x1 << 31) + 0xFF);
	value |= 0x1;
	writeq_relaxed(value, (volatile u32 *)VIC7100_RESET_DOUT_ADDR);

	sbi_printf("vic7100_system_reset_1\n");

	while(1);
}

/* Vendor-Specific SBI handler, for test */
static int vic7100_vendor_ext_provider(long extid, long funcid,
	const struct sbi_trap_regs *regs, unsigned long *out_value,
	struct sbi_trap_info *out_trap)
{
	int ret = 0;
	switch (funcid) {
	case SBI_EXT_STARFIVE_RDTIME:
		*out_value = csr_read(CSR_TIME);
		break;
	case SBI_EXT_STARFIVE_GET_MTIME:
		ret = mcall_print_mtime(0x0200BFF8);
		break;
	default:
		sbi_printf("Unsupported vendor sbi call : %ld\n", funcid);
		asm volatile("ebreak");
	}
	return ret;
}

const struct sbi_platform_operations platform_ops = {
	.final_init		= vic7100_final_init,
	.console_putc		= uart8250_putc,
	.console_getc		= uart8250_getc,
	.console_init		= vic7100_console_init,
	.irqchip_init		= vic7100_irqchip_init,
	.ipi_send		= clint_ipi_send,
	.ipi_clear		= clint_ipi_clear,
	.ipi_init		= vic7100_ipi_init,
	.get_tlbr_flush_limit	= vic7100_get_tlbr_flush_limit,
	.timer_value		= clint_timer_value,
	.timer_event_stop	= clint_timer_event_stop,
	.timer_event_start	= clint_timer_event_start,
	.timer_init		= vic7100_timer_init,
	.domains_root_regions	= vic7100_domains_init,
	.system_reset_check	= vic7100_system_reset_check,
	.system_reset		= vic7100_system_reset,
	.vendor_ext_provider	= vic7100_vendor_ext_provider
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "StarFive VIC7100",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count		= (VIC7100_HART_COUNT),
	.hart_stack_size	= SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
