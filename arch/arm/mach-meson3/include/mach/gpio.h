#ifndef __ARCH_MACH_GPIO_H__
#define __ARCH_MACH_GPIO_H__
typedef enum {
	GPIOY_0=0,
	GPIOY_1=1,
	GPIOY_2=2,
	GPIOY_3=3,
	GPIOY_4=4,
	GPIOY_5=5,
	GPIOY_6=6,
	GPIOY_7=7,
	GPIOY_8=8,
	GPIOY_9=9,
	GPIOY_10=10,
	GPIOY_11=11,
	GPIOY_12=12,
	GPIOY_13=13,
	GPIOY_14=14,
	GPIOY_15=15,
	GPIOY_16=16,
	GPIOY_17=17,
	GPIOY_18=18,
	GPIOY_19=19,
	GPIOY_20=20,
	GPIOY_21=21,
	GPIOY_22=22,
	GPIOX_0=23,
	GPIOX_1=24,
	GPIOX_2=25,
	GPIOX_3=26,
	GPIOX_4=27,
	GPIOX_5=28,
	GPIOX_6=29,
	GPIOX_7=30,
	GPIOX_8=31,
	GPIOX_9=32,
	GPIOX_10=33,
	GPIOX_11=34,
	GPIOX_12=35,
	GPIOX_13=36,
	GPIOX_14=37,
	GPIOX_15=38,
	GPIOX_16=39,
	GPIOX_17=40,
	GPIOX_18=41,
	GPIOX_19=42,
	GPIOX_20=43,
	GPIOX_21=44,
	GPIOX_22=45,
	GPIOX_23=46,
	GPIOX_24=47,
	GPIOX_25=48,
	GPIOX_26=49,
	GPIOX_27=50,
	GPIOX_28=51,
	GPIOX_29=52,
	GPIOX_30=53,
	GPIOX_31=54,
	GPIOX_32=55,
	GPIOX_33=56,
	GPIOX_34=57,
	GPIOX_35=58,
	BOOT_0=59,
	BOOT_1=60,
	BOOT_2=61,
	BOOT_3=62,
	BOOT_4=63,
	BOOT_5=64,
	BOOT_6=65,
	BOOT_7=66,
	BOOT_8=67,
	BOOT_9=68,
	BOOT_10=69,
	BOOT_11=70,
	BOOT_12=71,
	BOOT_13=72,
	BOOT_14=73,
	BOOT_15=74,
	BOOT_16=75,
	BOOT_17=76,
	GPIOD_0=77,
	GPIOD_1=78,
	GPIOD_2=79,
	GPIOD_3=80,
	GPIOD_4=81,
	GPIOD_5=82,
	GPIOD_6=83,
	GPIOD_7=84,
	GPIOD_8=85,
	GPIOD_9=86,
	GPIOC_0=87,
	GPIOC_1=88,
	GPIOC_2=89,
	GPIOC_3=90,
	GPIOC_4=91,
	GPIOC_5=92,
	GPIOC_6=93,
	GPIOC_7=94,
	GPIOC_8=95,
	GPIOC_9=96,
	GPIOC_10=97,
	GPIOC_11=98,
	GPIOC_12=99,
	GPIOC_13=100,
	GPIOC_14=101,
	GPIOC_15=102,
	CARD_0=103,
	CARD_1=104,
	CARD_2=105,
	CARD_3=106,
	CARD_4=107,
	CARD_5=108,
	CARD_6=109,
	CARD_7=110,
	CARD_8=111,
	GPIOB_0=112,
	GPIOB_1=113,
	GPIOB_2=114,
	GPIOB_3=115,
	GPIOB_4=116,
	GPIOB_5=117,
	GPIOB_6=118,
	GPIOB_7=119,
	GPIOB_8=120,
	GPIOB_9=121,
	GPIOB_10=122,
	GPIOB_11=123,
	GPIOB_12=124,
	GPIOB_13=125,
	GPIOB_14=126,
	GPIOB_15=127,
	GPIOB_16=128,
	GPIOB_17=129,
	GPIOB_18=130,
	GPIOB_19=131,
	GPIOB_20=132,
	GPIOB_21=133,
	GPIOB_22=134,
	GPIOB_23=135,
	GPIOA_0=136,
	GPIOA_1=137,
	GPIOA_2=138,
	GPIOA_3=139,
	GPIOA_4=140,
	GPIOA_5=141,
	GPIOA_6=142,
	GPIOA_7=143,
	GPIOA_8=144,
	GPIOA_9=145,
	GPIOA_10=146,
	GPIOA_11=147,
	GPIOA_12=148,
	GPIOA_13=149,
	GPIOA_14=150,
	GPIOA_15=151,
	GPIOA_16=152,
	GPIOA_17=153,
	GPIOA_18=154,
	GPIOA_19=155,
	GPIOA_20=156,
	GPIOA_21=157,
	GPIOA_22=158,
	GPIOA_23=159,
	GPIOA_24=160,
	GPIOA_25=161,
	GPIOA_26=162,
	GPIOA_27=163,
	GPIOAO_0=164,
	GPIOAO_1=165,
	GPIOAO_2=166,
	GPIOAO_3=167,
	GPIOAO_4=168,
	GPIOAO_5=169,
	GPIOAO_6=170,
	GPIOAO_7=171,
	GPIOAO_8=172,
	GPIOAO_9=173,
	GPIOAO_10=174,
	GPIOAO_11=175,
	TEST_N=176,
	GPIO_MAX=177,
}gpio_t;

enum {
	GPIO_IRQ0=0,
	GPIO_IRQ1,
	GPIO_IRQ2,
	GPIO_IRQ3,
	GPIO_IRQ4,
	GPIO_IRQ5,
	GPIO_IRQ6,
	GPIO_IRQ7,
};

enum {
	GPIO_IRQ_HIGH=0,
	GPIO_IRQ_LOW,
	GPIO_IRQ_RISING,
	GPIO_IRQ_FALLING,
};

enum {
	FILTER_NUM0=0,
	FILTER_NUM1,
	FILTER_NUM2,
	FILTER_NUM3,
	FILTER_NUM4,
	FILTER_NUM5,
	FILTER_NUM6,
	FILTER_NUM7,
};
#endif
