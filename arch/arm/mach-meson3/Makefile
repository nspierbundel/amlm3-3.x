#
# Makefile for the linux kernel.
#

# Common support
obj-y :=  cpu.o time.o irq.o clock.o power_gate.o usbclock.o
#obj-y := clock.o   clk_set.o gpio.o time.o 
obj-m :=
obj-n :=
obj-  :=

# gpio support
obj-$(CONFIG_GPIO_AMLOGIC) += gpio.o

#pinctrl support
obj-$(CONFIG_PINCTRL_AMLOGIC) += pinctrl.o


# Cache config
obj-$(CONFIG_CACHE_L2X0) += cache.o

obj-$(CONFIG_MACH_M3_SKT)	+= board-meson3-common.o
# Power Management
obj-$(CONFIG_SUSPEND)			+= pm.o
obj-$(CONFIG_MESON_SUSPEND) += power_suspend.o
