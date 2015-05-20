/*
 *  arch/arm/mach-meson/include/mach/io.h
 *
 *  Copyright (C) 2010 AMLOGIC, INC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __ASM_ARCH_IO_H
#define __ASM_ARCH_IO_H

//#define IO_SPACE_LIMIT 0xffffffff

//#define __io(a)     __typesafe_io(a)
#define __mem_pci(a)    (a)


/*
 * Clear and set bits in one shot. These macros can be used to clear and
 * set multiple bits in a register using a single call. These macros can
 * also be used to set a multiple-bit bit pattern using a mask, by
 * specifying the mask in the 'clear' parameter and the new bit pattern
 * in the 'set' parameter.
 */

#define out_arch(type,endian,a,v)	__raw_write##type(cpu_to_##endian(v),a)
#define in_arch(type,endian,a)		endian##_to_cpu(__raw_read##type(a))

#define out_le32(a,v)	out_arch(l,le32,a,v)
#define out_le16(a,v)	out_arch(w,le16,a,v)

#define in_le32(a)	in_arch(l,le32,a)
#define in_le16(a)	in_arch(w,le16,a)

#define out_be32(a,v)	out_arch(l,be32,a,v)
#define out_be16(a,v)	out_arch(w,be16,a,v)

#define in_be32(a)	in_arch(l,be32,a)
#define in_be16(a)	in_arch(w,be16,a)

#define out_8(a,v)	__raw_writeb(v,a)
#define in_8(a)		__raw_readb(a)

#define clrbits(type, addr, clear) \
	out_##type((addr), in_##type(addr) & ~(clear))

#define setbits(type, addr, set) \
	out_##type((addr), in_##type(addr) | (set))

#define clrsetbits(type, addr, clear, set) \
	out_##type((addr), (in_##type(addr) & ~(clear)) | (set))

#define clrbits_be32(addr, clear) clrbits(be32, addr, clear)
#define setbits_be32(addr, set) setbits(be32, addr, set)
#define clrsetbits_be32(addr, clear, set) clrsetbits(be32, addr, clear, set)

#define clrbits_le32(addr, clear) clrbits(le32, addr, clear)
#define setbits_le32(addr, set) setbits(le32, addr, set)
#define clrsetbits_le32(addr, clear, set) clrsetbits(le32, addr, clear, set)

#define clrbits_be16(addr, clear) clrbits(be16, addr, clear)
#define setbits_be16(addr, set) setbits(be16, addr, set)
#define clrsetbits_be16(addr, clear, set) clrsetbits(be16, addr, clear, set)

#define clrbits_le16(addr, clear) clrbits(le16, addr, clear)
#define setbits_le16(addr, set) setbits(le16, addr, set)
#define clrsetbits_le16(addr, clear, set) clrsetbits(le16, addr, clear, set)

#define clrbits_8(addr, clear) clrbits(8, addr, clear)
#define setbits_8(addr, set) setbits(8, addr, set)
#define clrsetbits_8(addr, clear, set) clrsetbits(8, addr, clear, set)

#ifdef CONFIG_VMSPLIT_3G
#define IO_USB_A_BASE           0xf9040000
#define IO_USB_B_BASE           0xf90C0000
#define IO_WIFI_BASE            0xf9300000
#define IO_SATA_BASE            0xf9400000
#define IO_ETH_BASE             0xf9410000

#define IO_CBUS_PHY_BASE        0xc1100000
#define IO_AXI_BUS_PHY_BASE     0xc1300000
#define IO_PL310_PHY_BASE       0xc4200000
#define IO_PERIPH_PHY_BASE      0xc4300000
#define IO_APB_BUS_PHY_BASE     0xc8000000
#define IO_AOBUS_PHY_BASE       0xc8100000
#define IO_AHB_BUS_PHY_BASE     0xc9000000
#define IO_APB2_BUS_PHY_BASE    0xd0000000

#define IO_CBUS_BASE            0xf1100000
#define IO_AXI_BUS_BASE         0xf1300000
#define IO_PL310_BASE           0xf4200000
#define IO_PERIPH_BASE          0xf4300000
#define IO_APB_BUS_BASE         0xf8000000
#define IO_AOBUS_BASE           0xf8100000
#define IO_AHB_BUS_BASE         0xf9000000
#define IO_APB2_BUS_BASE        0xfa000000

#define MESON_PERIPHS1_VIRT_BASE    0xf81004C0
#define MESON_PERIPHS1_PHYS_BASE    0xc81004C0
#endif

#ifdef CONFIG_VMSPLIT_2G
#define IO_AOBUS_BASE           0xc8100000
#define IO_USB_A_BASE           0xc9040000
#define IO_USB_B_BASE           0xc90C0000
#define IO_WIFI_BASE            0xc9300000
#define IO_SATA_BASE            0xc9400000
#define IO_ETH_BASE             0xc9410000

#define IO_CBUS_PHY_BASE        0xc1100000
#define IO_AXI_BUS_PHY_BASE     0xc1300000
#define IO_PL310_PHY_BASE       0xc4200000
#define IO_APB_BUS_PHY_BASE     0xc8000000
#define IO_AOBUS_PHY_BASE       0xc8100000
#define IO_AHB_BUS_PHY_BASE     0xc9000000

#define IO_CBUS_BASE            0xc1100000
#define IO_AXI_BUS_BASE         0xc1300000
#define IO_PL310_BASE           0xc4200000
#define IO_AHB_BUS_BASE         0xc9000000
#define IO_APB_BUS_BASE			0xc8000000

#define MESON_PERIPHS1_VIRT_BASE    0xc81004C0
#define MESON_PERIPHS1_PHYS_BASE    0xc81004C0
#endif

#ifdef CONFIG_VMSPLIT_1G
#error Unsupported Memory Split Type
#endif

#define CBUS_REG_OFFSET(reg) ((reg) << 2)
#define CBUS_REG_ADDR(reg)	 (IO_CBUS_BASE + CBUS_REG_OFFSET(reg))

#define AXI_REG_OFFSET(reg)  ((reg) << 2)
#define AXI_REG_ADDR(reg)	 (IO_AXI_BUS_BASE + AXI_REG_OFFSET(reg))

#define AHB_REG_OFFSET(reg)  ((reg) << 2)
#define AHB_REG_ADDR(reg)	 (IO_AHB_BUS_BASE + AHB_REG_OFFSET(reg))

#define APB_REG_OFFSET(reg)  (reg)
#define APB_REG_ADDR(reg)	 (IO_APB_BUS_BASE + APB_REG_OFFSET(reg))
#define APB_REG_ADDR_VALID(reg) (((unsigned long)(reg) & 3) == 0)

#define AOBUS_REG_OFFSET(reg) ((reg) )
#define AOBUS_REG_ADDR(reg)	 (IO_AOBUS_BASE + AOBUS_REG_OFFSET(reg))

#endif
