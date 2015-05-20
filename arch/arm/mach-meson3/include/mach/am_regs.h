/*
 *
 * arch/arm/mach-meson3/include/mach/am_regs.h
 *
 *  Copyright (C) 2011 AMLOGIC, INC.
 *
 * License terms: GNU General Public License (GPL) version 2
 * Basic register address definitions in physical memory and
 * some block defintions for core devices like the timer.
 */

#ifndef __MACH_MESSON3_REGS_H
#define __MACH_MESSON3_REGS_H

#ifndef __ASSEMBLY__

#include <asm/io.h>
#include "reg_addr.h"
#include <plat/io.h>
#include "cpu.h"
#define WRITE_CBUS_REG(reg, val) aml_write_reg32(CBUS_REG_ADDR(reg),val)
#define READ_CBUS_REG(reg) (aml_read_reg32(CBUS_REG_ADDR(reg)))
#define WRITE_CBUS_REG_BITS(reg, val, start, len) \
    WRITE_CBUS_REG(reg,	(READ_CBUS_REG(reg) & ~(((1L<<(len))-1)<<(start)) )| ((unsigned)((val)&((1L<<(len))-1)) << (start)))
#define READ_CBUS_REG_BITS(reg, start, len) \
    ((READ_CBUS_REG(reg) >> (start)) & ((1L<<(len))-1))
#define CLEAR_CBUS_REG_MASK(reg, mask) WRITE_CBUS_REG(reg, (READ_CBUS_REG(reg)&(~(mask))))
#define SET_CBUS_REG_MASK(reg, mask)   WRITE_CBUS_REG(reg, (READ_CBUS_REG(reg)|(mask)))

#define WRITE_AXI_REG(reg, val) aml_write_reg32( AXI_REG_ADDR(reg),val)
#define READ_AXI_REG(reg) (aml_read_reg32(AXI_REG_ADDR(reg)))
#define WRITE_AXI_REG_BITS(reg, val, start, len) \
    WRITE_AXI_REG(reg,	(READ_AXI_REG(reg) & ~(((1L<<(len))-1)<<(start)) )| ((unsigned)((val)&((1L<<(len))-1)) << (start)))
#define READ_AXI_REG_BITS(reg, start, len) \
    ((READ_AXI_REG(reg) >> (start)) & ((1L<<(len))-1))
#define CLEAR_AXI_REG_MASK(reg, mask) WRITE_AXI_REG(reg, (READ_AXI_REG(reg)&(~(mask))))
#define SET_AXI_REG_MASK(reg, mask)   WRITE_AXI_REG(reg, (READ_AXI_REG(reg)|(mask)))

#define WRITE_AHB_REG(reg, val) aml_write_reg32(AHB_REG_ADDR(reg),val,)
#define READ_AHB_REG(reg) (aml_read_reg32(AHB_REG_ADDR(reg)))
#define WRITE_AHB_REG_BITS(reg, val, start, len) \
    WRITE_AHB_REG(reg,	(READ_AHB_REG(reg) & ~(((1L<<(len))-1)<<(start)) )| ((unsigned)((val)&((1L<<(len))-1)) << (start)))
#define READ_AHB_REG_BITS(reg, start, len) \
    ((READ_AHB_REG(reg) >> (start)) & ((1L<<(len))-1))
#define CLEAR_AHB_REG_MASK(reg, mask) WRITE_AHB_REG(reg, (READ_AHB_REG(reg)&(~(mask))))
#define SET_AHB_REG_MASK(reg, mask)   WRITE_AHB_REG(reg, (READ_AHB_REG(reg)|(mask)))

#define WRITE_APB_REG(reg, val) aml_write_reg32( APB_REG_ADDR(reg),val)
#define READ_APB_REG(reg) (aml_read_reg32(APB_REG_ADDR(reg)))
#define WRITE_APB_REG_BITS(reg, val, start, len) \
    WRITE_APB_REG(reg,	(READ_APB_REG(reg) & ~(((1L<<(len))-1)<<(start)) )| ((unsigned)((val)&((1L<<(len))-1)) << (start)))
#define READ_APB_REG_BITS(reg, start, len) \
    ((READ_APB_REG(reg) >> (start)) & ((1L<<(len))-1))
#define CLEAR_APB_REG_MASK(reg, mask) WRITE_APB_REG(reg, (READ_APB_REG(reg)&(~(mask))))
#define SET_APB_REG_MASK(reg, mask)   WRITE_APB_REG(reg, (READ_APB_REG(reg)|(mask)))

#define WRITE_AOBUS_REG(reg, val) aml_write_reg32(AOBUS_REG_ADDR(reg),val)
#define READ_AOBUS_REG(reg) (aml_read_reg32(AOBUS_REG_ADDR(reg)))
#define WRITE_AOBUS_REG_BITS(reg, val, start, len) \
    WRITE_AOBUS_REG(reg,	(READ_AOBUS_REG(reg) & ~(((1L<<(len))-1)<<(start)) )| ((unsigned)((val)&((1L<<(len))-1)) << (start)))
#define READ_AOBUS_REG_BITS(reg, start, len) \
    ((READ_AOBUS_REG(reg) >> (start)) & ((1L<<(len))-1))
#define CLEAR_AOBUS_REG_MASK(reg, mask) WRITE_AOBUS_REG(reg, (READ_AOBUS_REG(reg)&(~(mask))))
#define SET_AOBUS_REG_MASK(reg, mask)   WRITE_AOBUS_REG(reg, (READ_AOBUS_REG(reg)|(mask)))
/* for back compatible alias */
#define WRITE_MPEG_REG(reg, val) \
	WRITE_CBUS_REG(reg, val)
#define READ_MPEG_REG(reg) \
	READ_CBUS_REG(reg)
#define WRITE_MPEG_REG_BITS(reg, val, start, len) \
	WRITE_CBUS_REG_BITS(reg, val, start, len)
#define READ_MPEG_REG_BITS(reg, start, len) \
	READ_CBUS_REG_BITS(reg, start, len)
#define CLEAR_MPEG_REG_MASK(reg, mask) \
	CLEAR_CBUS_REG_MASK(reg, mask)
#define SET_MPEG_REG_MASK(reg, mask) \
	SET_CBUS_REG_MASK(reg, mask)

#endif

#include "io.h"

#include "c_stb_define.h"
#include "regs.h"
#include "ao_regs.h"
#include "pctl.h"
#include "dmc.h"
#include "am_eth_reg.h"
#endif //__MACH_MESSON3_REGS_H
