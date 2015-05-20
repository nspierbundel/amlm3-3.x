/*
 *
 * arch/arm/mach-meson/meson.c
 *
 *  Copyright (C) 2010 AMLOGIC, INC.
 *
 * License terms: GNU General Public License (GPL) version 2
 * Platform machine definition.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
#include <linux/device.h>
#include <linux/spi/flash.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>
#include <asm/memory.h>
#include <asm/mach/map.h>
#include <plat/platform_data.h>
#include <plat/platform.h>
#include <plat/lm.h>
#include <mach/am_regs.h>
#include <mach/i2c_aml.h>
#include <mach/usbclock.h>
#include <mach/map.h>
#ifdef CONFIG_CACHE_L2X0
#include <asm/hardware/cache-l2x0.h>
#endif
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/amlogic/of_lm.h>
#include <mach/gpio.h>
extern void __init meson_timer_init(void);
//extern void meson_common_restart(char mode,const char *cmd);
extern void __init meson_init_irq(void);
extern void meson_common_restart(char mode,const char *cmd);
#if 0
static void __init meson_cache_init(void)
{
#ifdef CONFIG_CACHE_L2X0
    l2x0_init((void __iomem *)IO_PL310_BASE, 0x7c420001, 0xff800fff);
#endif
}

static __init void meson_init_machine(void)
{
    meson_cache_init();
}
#endif
/***********************************************************************
 * IO Mapping
 **********************************************************************/
static __initdata struct map_desc meson_io_desc[] = {
    {
        .virtual    = IO_CBUS_BASE,
        .pfn        = __phys_to_pfn(IO_CBUS_PHY_BASE),
        .length     = SZ_2M,
        .type       = MT_DEVICE,
    } , {
        .virtual    = IO_AXI_BUS_BASE,
        .pfn        = __phys_to_pfn(IO_AXI_BUS_PHY_BASE),
        .length     = SZ_1M,
        .type       = MT_DEVICE,
    } , {
        .virtual    = IO_PL310_BASE,
        .pfn        = __phys_to_pfn(IO_PL310_PHY_BASE),
        .length     = SZ_4K,
        .type       = MT_DEVICE,
    } , {
        .virtual    = IO_PERIPH_BASE,
        .pfn        = __phys_to_pfn(IO_PERIPH_PHY_BASE),
        .length     = SZ_4K,
        .type       = MT_DEVICE,
    } , {
        .virtual    = IO_APB_BUS_BASE,
        .pfn        = __phys_to_pfn(IO_APB_BUS_PHY_BASE),
        .length     = SZ_512K,
        .type       = MT_DEVICE,
    } , {
        .virtual    = IO_AOBUS_BASE,
        .pfn        = __phys_to_pfn(IO_AOBUS_PHY_BASE),
        .length     = SZ_1M,
        .type       = MT_DEVICE,
    } , {
        .virtual    = IO_AHB_BUS_BASE,
        .pfn        = __phys_to_pfn(IO_AHB_BUS_PHY_BASE),
        .length     = SZ_16M,
        .type       = MT_DEVICE,
    } , {
        .virtual    = IO_APB2_BUS_BASE,
        .pfn        = __phys_to_pfn(IO_APB2_BUS_PHY_BASE),
        .length     = SZ_512K,
        .type       = MT_DEVICE,
    },
#ifdef CONFIG_MESON_SUSPEND
	{
        .virtual    = PAGE_ALIGN(0xdff00000),
        .pfn        = __phys_to_pfn(0x1ff00000),
        .length     = SZ_1M,
        .type       = MT_MEMORY,
	},
#endif
};
static struct of_device_id mxs_of_platform_bus_ids[] = {
	{.compatible = "simple-bus",},
	{},
};

static struct of_device_id mxs_of_lm_bus_ids[] = {
		{.compatible = "logicmodule-bus",},  
		{},
};

static __init void meson_init_machine_devicetree(void)
{

	struct device *parent;	
	parent = get_device(&platform_bus);
	
	of_platform_populate(NULL,mxs_of_platform_bus_ids,NULL,parent);
	of_lm_populate(NULL,mxs_of_lm_bus_ids,NULL,NULL);

	//mmc_lp_suspend_init();
//		of_platform_populate(NULL, of_default_bus_match_table,
//		aml_meson6_auxdata_lookup, NULL);
      // pm_power_off = power_off;

}

static  void __init meson_map_io(void)
{
    iotable_init(meson_io_desc, ARRAY_SIZE(meson_io_desc));
}

static __init void m3_irq_init(void)
{
    meson_init_irq();
}

static __init void meson_init_early(void)
{
	/*
	 * Mali or some USB devices allocate their coherent buffers from atomic
	 * context. Increase size of atomic coherent pool to make sure such
	 * the allocations won't fail.
	 */
	//init_dma_coherent_pool_size(SZ_4M);
	//meson_cpu_version_init();
	return ;
}

static const char *m3_common_board_compat[] __initdata = {
	"AMLOGIC,8726_M3",
	NULL,
};

#ifdef CONFIG_USE_OF
DT_MACHINE_START(AML8726_M3, "Amlogic Meson3")
	//.reserve	= meson6_reserve,
	.map_io		= meson_map_io,/// dt - 1
	.init_early	= meson_init_early,/// dt -2
	.init_irq		= m3_irq_init,/// dt - 3
	.init_time		= meson_timer_init, /// dt - 4
	.init_machine	= meson_init_machine_devicetree,
	.restart	= meson_common_restart,
	.dt_compat	= m3_common_board_compat,
MACHINE_END

#else
#define MACH_TYPE_M3_SKT 3882
extern void __init meson_timer_init(void);
MACHINE_START(M3_SKT, "Meson 3 socket board")
    .map_io         = meson_map_io,
    .init_irq       = m3_irq_init,
    .init_time          = meson_timer_init,
    .init_machine   = meson_init_machine,
MACHINE_END
#endif
