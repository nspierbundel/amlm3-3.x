/*
 *
 * arch/arm/mach-meson/clock.c
 *
 *  Copyright (C) 2010 AMLOGIC, INC.
 *
 * License terms: GNU General Public License (GPL) version 2
 * Define clocks in the app platform.
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/device.h>

#include <linux/clkdev.h>
#include <mach/clock.h>
#include <mach/hardware.h>
#include <mach/clk_set.h>
#include <mach/am_regs.h>
#include <mach/power_gate.h>
#define IS_CLK_ERR(a)  (IS_ERR(a) || a == 0)
static DEFINE_SPINLOCK(clockfw_lock);
static DEFINE_MUTEX(clock_ops_lock);
//static unsigned long ddr_pll_clk = 0;
unsigned long clk_get_rate_xtal(struct clk * clkdev)
{
    unsigned long clk;

    clk = READ_CBUS_REG_BITS(PREG_CTLREG0_ADDR, 4, 5);
    clk = clk * 1000 * 1000;
    return clk;
}

static unsigned pll_setting[17][3]={
    {0x20222,0x065e11ff,0x0249a941},
    {0x2022a,0x065e11ff,0x0249a941},
    {0x20232,0x065e11ff,0x0249a941},
    {0x2023a,0x065e31ff,0xbe49a941},
    {0x10221,0x065e11ff,0x0249a941},
    {0x10225,0x065e11ff,0x0249a941},
    {0x1022a,0x065e11ff,0x0249a941},
    {0x1022e,0x065e11ff,0x0249a941},
    {0x10232,0x065e11ff,0x0249a941},
    {0x10236,0x065e11ff,0x0249a941},
    {0x1023a,0x065e31ff,0xbe49a941},
    {0x1023e,0x065e31ff,0xbe49a941},
    {0x00220,0x065e11ff,0x0249a941},
    {0x00220,0x065e11ff,0x0249a941},
    {0x00221,0x065e11ff,0x0249a941},
    {0x00221,0x065e11ff,0x0249a941},
    {0x00222,0x065e11ff,0x0249a941},
};

unsigned get_sys_clkpll_setting(unsigned crystal_freq, unsigned out_freq)
{
    unsigned long crys_M, out_M, i;
    if (!crystal_freq)
        crystal_freq = clk_get_rate_xtal(NULL);
    crys_M = crystal_freq / 1000000;
    out_M = out_freq / 1000000;
    i = (out_M-200)/50;
    if (i>16) i=16;
    return pll_setting[i][0];
}

int sys_clkpll_setting(unsigned crystal_freq, unsigned out_freq)
{
    int i, lock_flag;
    unsigned lock_time=0;
    unsigned long result_freq, target_freq;
    unsigned long crys_M, out_M;
    unsigned long freq_log[64];
    int log_index;
    unsigned target_pll_setting;

    if (!crystal_freq)
        crystal_freq = clk_get_rate_xtal(NULL);
    crys_M = crystal_freq / 1000000;
    out_M = out_freq / 1000000;
    i = (out_M-200)/50;
    if (i>16) i=16;
    target_pll_setting = pll_setting[i][0];
    if (READ_MPEG_REG(HHI_SYS_PLL_CNTL)!=target_pll_setting){
        WRITE_MPEG_REG(HHI_SYS_PLL_CNTL, target_pll_setting); 
        WRITE_MPEG_REG(HHI_SYS_PLL_CNTL2, pll_setting[i][1]); 
        WRITE_MPEG_REG(HHI_SYS_PLL_CNTL3, pll_setting[i][2]);
        WRITE_MPEG_REG(RESET5_REGISTER, (1<<2));        // reset sys pll

        lock_flag = 0;
        log_index = 0;
        target_freq = ((target_pll_setting&0x1ff)*crys_M)>>(target_pll_setting>>16);
        for (i=0;i<64;i++){
            result_freq = clk_util_clk_msr(SYS_PLL_CLK);
            if ((result_freq <= target_freq+1)&&(result_freq >= target_freq-1)){
                lock_flag++;
                if (lock_flag>=1)
                    break;
            }
            if (log_index<64) 
                freq_log[log_index++]=result_freq;
            else 
                break;
            lock_time+=64;
        }
        lock_time-=64;
       
        //printk("sys clk changed");
        //for (i=0;i<log_index;i++)
        //    printk("-%d", freq_log[i]);
        //printk("\ncpu_clk_changed: out_freq=%ld,pll_setting=%x,locktime=%dus\n",out_M,target_pll_setting,lock_time);
    }
    return 0;
}

// -----------------------------------------
// clk_util_clk_msr
// -----------------------------------------
// from twister_core.v
//        .clk_to_msr_in          ( { 18'h0,                      // [63:46]
//                                    cts_pwm_A_clk,              // [45]
//                                    cts_pwm_B_clk,              // [44]
//                                    cts_pwm_C_clk,              // [43]
//                                    cts_pwm_D_clk,              // [42]
//                                    cts_eth_rx_tx,              // [41]
//                                    cts_pcm_mclk,               // [40]
//                                    cts_pcm_sclk,               // [39]
//                                    cts_vdin_meas_clk,          // [38]
//                                    cts_vdac_clk[1],            // [37]
//                                    cts_hdmi_tx_pixel_clk,      // [36]
//                                    cts_mali_clk,               // [35]
//                                    cts_sdhc_clk1,              // [34]
//                                    cts_sdhc_clk0,              // [33]
//                                    cts_audac_clkpi,            // [32]
//                                    cts_a9_clk,                 // [31]
//                                    cts_ddr_clk,                // [30]
//                                    cts_vdac_clk[0],            // [29]
//                                    cts_sar_adc_clk,            // [28]
//                                    cts_enci_clk,               // [27]
//                                    sc_clk_int,                 // [26]
//                                    usb_clk_12mhz,              // [25]
//                                    lvds_fifo_clk,              // [24]
//                                    HDMI_CH3_TMDSCLK,           // [23]
//                                    mod_eth_clk50_i,            // [22]
//                                    mod_audin_amclk_i,          // [21]
//                                    cts_btclk27,                // [20]
//                                    cts_hdmi_sys_clk,           // [19]
//                                    cts_led_pll_clk,            // [18]
//                                    cts_vghl_pll_clk,           // [17]
//                                    cts_FEC_CLK_2,              // [16]
//                                    cts_FEC_CLK_1,              // [15]
//                                    cts_FEC_CLK_0,              // [14]
//                                    cts_amclk,                  // [13]
//                                    vid2_pll_clk,               // [12]
//                                    cts_eth_rmii,               // [11]
//                                    cts_enct_clk,               // [10]
//                                    cts_encl_clk,               // [9]
//                                    cts_encp_clk,               // [8]
//                                    clk81,                      // [7]
//                                    vid_pll_clk,                // [6]
//                                    aud_pll_clk,                // [5]
//                                    misc_pll_clk,               // [4]
//                                    ddr_pll_clk,                // [3]
//                                    sys_pll_clk,                // [2]
//                                    am_ring_osc_clk_out[1],     // [1]
//                                    am_ring_osc_clk_out[0]} ),  // [0]
//
// For Example
//
// unsigend long    clk81_clk   = clk_util_clk_msr( 2,      // mux select 2
//                                                  50 );   // measure for 50uS
//
// returns a value in "clk81_clk" in Hz
//
// The "uS_gate_time" can be anything between 1uS and 65535 uS, but the limitation is
// the circuit will only count 65536 clocks.  Therefore the uS_gate_time is limited by
//
//   uS_gate_time <= 65535/(expect clock frequency in MHz)
//
// For example, if the expected frequency is 400Mhz, then the uS_gate_time should
// be less than 163.
//
// Your measurement resolution is:
//
//    100% / (uS_gate_time * measure_val )
//
//
unsigned int clk_util_clk_msr(unsigned int clk_mux)
{
    unsigned int regval = 0;
    WRITE_CBUS_REG(MSR_CLK_REG0, 0);
    // Set the measurement gate to 64uS
    CLEAR_CBUS_REG_MASK(MSR_CLK_REG0, 0xffff);
    SET_CBUS_REG_MASK(MSR_CLK_REG0, (64 - 1)); //64uS is enough for measure the frequence?
    // Disable continuous measurement
    // disable interrupts
    CLEAR_CBUS_REG_MASK(MSR_CLK_REG0, ((1 << 18) | (1 << 17)));
    CLEAR_CBUS_REG_MASK(MSR_CLK_REG0, (0x1f << 20));
    SET_CBUS_REG_MASK(MSR_CLK_REG0, (clk_mux << 20) | // Select MUX
                                    (1 << 19) |       // enable the clock
									(1 << 16));       //enable measuring
    // Wait for the measurement to be done
    regval = READ_CBUS_REG(MSR_CLK_REG0);
    do {
        regval = READ_CBUS_REG(MSR_CLK_REG0);
    } while (regval & (1 << 31));

    // disable measuring
    CLEAR_CBUS_REG_MASK(MSR_CLK_REG0, (1 << 16));
    regval = (READ_CBUS_REG(MSR_CLK_REG2) + 31) & 0x000FFFFF;
    // Return value in MHz*measured_val
    return (regval >> 6)*1000000;
}
int    clk_measure(char  index )
{
	const char* clk_table[]={
	" CTS_MIPI_PHY_CLK(50)",	
	" AM_RING_OSC_OUT_MALI[1](49)",	
	" AM_RING_OSC_OUT_MALI[0](48)",	
	" AM_RING_OSC_OUT_A9[1](47)",	
	" AM_RING_OSC_OUT_A9[0](46)",		
	" CTS_PWM_A_CLK(45)",
	" CTS_PWM_B_CLK(44)",
	" CTS_PWM_C_CLK(43)",
	" CTS_PWM_D_CLK(42)",
	" CTS_ETH_TX(41)",
	" CTS_PCM_MCLK(40)",
	" CTS_PCM_SCLK(39)",
	" CTS_VDIN_MEAS_CLK(38)",
	" CTS_VDAC_CLK1(37)",
	" CTS_HDMI_TX_PIXEL_CLK(36)",
	" CTS_MALI_CLK (35)",
	" CTS_SDHC_CLK1(34)",
	" CTS_SDHC_CLK0(33)",
	" CTS_VDAC_CLK(32)",
	" Reserved(31)",
	" CTS_SLOW_DDR_CLK(30)",
	" CTS_VDAC_CLK0(29)",
	" CTS_SAR_ADC_CLK(28)",
	" CTS_ENCI_CL(27)",
	" SC_CLK_INT(26)",
	" SYS_PLL_DIV3(25)",
	" LVDS_FIFO_CLK(24)",
	" HDMI_CH0_TMDSCLK(23)",
	" CLK_RMII_FROM_PAD (22)",
	" MOD_AUDIN_AMCLK_I(21)",
	" RTC_OSC_CLK_OUT (20)",
	" CTS_HDMI_SYS_CLK(19)",
	" CTS_LED_PLL_CLK(18)",
	" CTS_VGHL_PLL_CLK (17)",
	" CTS_FEC_CLK_2(16)",
	" CTS_FEC_CLK_1 (15)",
	" CTS_FEC_CLK_0 (14)",
	" CTS_AMCLK(13)",
	" VID2_PLL_CLK(12)",
	" CTS_ETH_RMII(11)",
	" CTS_ENCT_CLK(10)",
	" CTS_ENCL_CLK(9)",
	" CTS_ENCP_CLK(8)",
	" CLK81 (7)",
	" VID_PLL_CLK(6)",
	" USB1_CLK_12MHZ (5)",
	" USB0_CLK_12MHZ (4)",
	" DDR_PLL_CLK(3)",
	" Reserved(2)",
	" AM_RING_OSC_CLK_OUT1(1)",
	" AM_RING_OSC_CLK_OUT0(0)",
	};   
	int  i;
	int len = sizeof(clk_table)/sizeof(char*) - 1;
	if (index  == 0xff)
	{
	 	for(i = 0;i < len;i++)
		{
			printk("[%10d]%s\n",clk_util_clk_msr(i),clk_table[len-i]);
		}
		return 0;
	}	
	printk("[%10d]%s\n",clk_util_clk_msr(index),clk_table[len-index]);
	return 0;
}



static unsigned long clk_get_rate_sys(struct clk * clkdev)
{
	   unsigned long clk;
	   if (clkdev && clkdev->rate)
		   clk = clkdev->rate;
	   else {
		   //using measure sys div3 to get sys pll clock. (25)
		   unsigned long mul, div, od, temp;
		   unsigned long long result;
		   clk = clk_get_rate_xtal(NULL);
		   temp = aml_read_reg32(P_HHI_SYS_PLL_CNTL);
		   mul=temp&((1<<9)-1);
		   div=(temp>>9)&0x3f;
		   od=(temp>>16)&3;
		   result=((u64)clk)*((u64)mul);
		   do_div(result,div);
		   clk = (unsigned long)(result>>od);
	   }
	   return clk;
}
EXPORT_SYMBOL(clk_get_rate_sys);

static int clk_set_rate_a9(struct clk *clk, unsigned long rate)
{
    struct clk *father_clk;
    int ret, divider;
    unsigned long flags;
    unsigned int clk_a9 = 0;
    unsigned long target_clk;
  //  int mali_divider;
	
    father_clk = clk_get_sys("pll_sys", NULL);
	divider = 0;
	while ((rate<<divider)<200000000)
		divider++;
	if (divider>2){
		return -1;
	}

    target_clk = get_sys_clkpll_setting(0, rate<<divider);
    target_clk = (((target_clk&0x1ff)*clk_get_rate_xtal(NULL)/1000000)>>(target_clk>>16))>>divider;
	local_irq_save(flags);
#if 0
    if (!ddr_pll_clk)
    	ddr_pll_clk = clk_util_clk_msr(CTS_DDR_CLK);
    mali_divider = 1;
	while ((mali_divider * target_clk < ddr_pll_clk) || (264 * mali_divider < ddr_pll_clk)) // assume mali max 264M
		mali_divider++;
    
  
    if (((mali_divider-1) != (READ_MPEG_REG(HHI_MALI_CLK_CNTL)&0x7f))&&(READ_CBUS_REG(HHI_MALI_CLK_CNTL)&(1<<8))){ // if mali busy skip change clk
    	local_irq_restore(flags);
    	return -1;
	}

    if ((mali_divider-1) != (READ_MPEG_REG(HHI_MALI_CLK_CNTL)&0x7f)){
	    WRITE_CBUS_REG(HHI_MALI_CLK_CNTL,
	               (3 << 9)    |   		// select ddr pll as clock source
	               ((mali_divider-1) << 0)); // ddr clk / divider
	}
#endif
    CLEAR_CBUS_REG_MASK(HHI_SYS_CPU_CLK_CNTL, 1<<7); // cpu use xtal
    ret = father_clk->set_rate(father_clk, rate<<divider);
	if (divider<2){
	    WRITE_MPEG_REG(HHI_SYS_CPU_CLK_CNTL,
	                   (1 << 0) |  // 1 - sys pll clk
	                   (divider << 2) |  // sys pll div 1 or 2
	                   (1 << 4) |  // APB_CLK_ENABLE
	                   (1 << 5) |  // AT_CLK_ENABLE
	                   (0 << 8)); 
	}
	else
	{
	    WRITE_MPEG_REG(HHI_SYS_CPU_CLK_CNTL,
	                   (1 << 0) |  // 1 - sys pll clk
	                   (3 << 2) |  // sys pll div 4
	                   (1 << 4) |  // APB_CLK_ENABLE
	                   (1 << 5) |  // AT_CLK_ENABLE
	                   (2 << 8)); 
	}
    clk_a9 = READ_MPEG_REG(HHI_SYS_CPU_CLK_CNTL); // read cbus for a short delay
    SET_CBUS_REG_MASK(HHI_SYS_CPU_CLK_CNTL, 1<<7); // cpu use sys pll
    clk->rate = rate;
    local_irq_restore(flags);

    //printk(KERN_INFO "-----------------------------------\n");
    //printk(KERN_INFO "(CTS_MALI_CLK) = %ldMHz\n", ddr_pll_clk/mali_divider);
    printk(KERN_DEBUG "(CTS_A9_CLK) = %ldMHz\n", rate/1000000);
    return 0;
}

static unsigned long clk_get_rate_a9(struct clk * clkdev)
{
	unsigned long clk = 0;
	clk=clk_util_clk_msr(clkdev->msr);
	return clk;
}

static int set_sys_pll(struct clk *clk,  unsigned long rate)
{
	unsigned long r = rate;
	int ret;

    if (r < 1000) {
        r = r * 1000000;
    }

    ret = sys_clkpll_setting(0, r);
    if (ret == 0) {
        clk->rate = r;
    }

    return ret;
}

static int set_fixed_pll(struct clk *clk,  unsigned long dst)
{
	return 0;
}

unsigned int get_fixed_pll_clk(void)
{
	unsigned int freq = 0;
	freq = (clk_util_clk_msr(MISC_PLL_CLK) );
	return freq;
}
EXPORT_SYMBOL(get_fixed_pll_clk);

unsigned int get_ddr_pll_clk(void)
{
    static unsigned int freq = 0;
    if (freq == 0) {
        freq = (clk_util_clk_msr(DDR_PLL_CLK) * 1000000);
    }
    return freq;
}
EXPORT_SYMBOL(get_ddr_pll_clk);

long clk_round_rate(struct clk *clk, unsigned long rate)
{
    if (rate < clk->min) {
        return clk->min;
    }

    if (rate > clk->max) {
        return clk->max;
    }

    return rate;
}
EXPORT_SYMBOL(clk_round_rate);

unsigned long clk_get_rate(struct clk *clk)
{
    if (!clk) {
        return 0;
    }

    if (clk->get_rate) {
        return clk->get_rate(clk);
    }

    return clk->rate;
}
EXPORT_SYMBOL(clk_get_rate);
int clk_set_rate(struct clk *clk, unsigned long rate)
{
    unsigned long flags;
    int ret;

    if (clk == NULL || clk->set_rate == NULL) {
        return -EINVAL;
    }

    spin_lock_irqsave(&clockfw_lock, flags);

    ret = clk->set_rate(clk, rate);

    spin_unlock_irqrestore(&clockfw_lock, flags);

    return ret;
}
EXPORT_SYMBOL(clk_set_rate);




static int clk_in_clocktree(struct clk *clktree, struct clk *clk)
{
	struct clk *p;
	int ret = 0;
	if(IS_CLK_ERR(clk) || IS_CLK_ERR(clktree))
		return 0;
	if(clktree == clk)
		return 1;
	p = (struct clk*)clktree->sibling.next;
	while(p){
		if(p == clk){
			ret = 1;
			break;
		}
		p = (struct clk*)p->sibling.next;
	}
	if(ret == 1)
		return ret;
	return clk_in_clocktree((struct clk*)clktree->child.next, clk);
}

static int meson_clk_register(struct clk* clk, struct clk* parent)
{
	if (clk_in_clocktree(parent,clk))
			return 0;
	clk->parent = parent;
	if (parent->child.next == NULL) {
		parent->child.next = (struct list_head*)clk;
		clk->sibling.next = NULL;
		clk->sibling.prev = NULL;
	}
	else {
		struct clk* p = (       struct clk*)(parent->child.next);
		while(p->sibling.next != NULL)
			p = (       struct clk*)(p->sibling.next);
		p->sibling.next = (struct list_head*)clk;
		clk->sibling.prev = (struct list_head*)p;
		clk->sibling.next = NULL;
	}
	return 0;
}



unsigned long long clkparse(const char *ptr, char **retptr)
{
    char *endptr;   /* local pointer to end of parsed string */

    unsigned long long ret = simple_strtoull(ptr, &endptr, 0);

    switch (*endptr) {
    case 'G':
    case 'g':
        ret *= 1000;
    case 'M':
    case 'm':
        ret *= 1000;
    case 'K':
    case 'k':
        ret *= 1000;
        endptr++;
    default:
        break;
    }

    if (retptr) {
        *retptr = endptr;
    }

    return ret;
}
int clk_get_status(struct clk *clk)
{
	int ret = 2;
	unsigned long flags;

	spin_lock_irqsave(&clockfw_lock, flags);
	if (clk->status)
		ret = clk->status(clk);
	else if (clk->clk_gate_reg_adr != 0)
		ret = ((aml_read_reg32(clk->clk_gate_reg_adr) & clk->clk_gate_reg_mask) ? 1 : 0);
	spin_unlock_irqrestore(&clockfw_lock, flags);

	return ret;
}
EXPORT_SYMBOL(clk_get_status);

int meson_enable(struct clk *clk)
{
	if (IS_CLK_ERR(clk))
		return 0;

	if (clk_get_status(clk) == 1)
		return 0;

	if (meson_enable(clk->parent) == 0) {
			struct clk_ops *p;
			int idx;
			int ops_run_count = 0;
			int ret = 0;
			p = clk->clk_ops;
			while(p){
					ops_run_count++;
					if(p->clk_enable_before)
						ret = p->clk_enable_before(p->privdata);
					if(ret == 1)
						break;
					p = p->next;
			}
	
			if(ret == 0){	
				if(clk->enable)
					ret = clk->enable(clk);
				else if(clk->clk_gate_reg_adr != 0)
					aml_set_reg32_mask(clk->clk_gate_reg_adr,clk->clk_gate_reg_mask);
					ret = 0;
			}
				
			p = clk->clk_ops;
			idx = 0;
			while(p){
				idx++;
				if(idx > ops_run_count)
					break;
				if(p->clk_enable_after)
					 p->clk_enable_after(p->privdata,ret);
				p = p->next;
			}
			
			return ret;
		}
		else
			return 1;
}
int clk_enable(struct clk *clk)
{
		int ret;
		mutex_lock(&clock_ops_lock);
		ret = meson_enable(clk);
		mutex_unlock(&clock_ops_lock);
		return ret;
}
EXPORT_SYMBOL(clk_enable);

int  meson_clk_disable(struct clk *clk)
{
		int ret = 0;
		int ops_run_count = 0;
		if(IS_CLK_ERR(clk))
			return 0;
		if(clk_get_status(clk) == 0)
			return 0;

		if(clk->child.next){
			struct clk* pchild = (struct clk*)(clk->child.next);
			if(meson_clk_disable(pchild) != 0)
				return 1;
			pchild = (struct clk*)pchild->sibling.next;
			while(pchild){
				if(meson_clk_disable(pchild) != 0)
					return 1;
				pchild = (struct clk*)pchild->sibling.next;
			}
		}

		//do clk disable
		//post message before clk disable.
		{
			struct clk_ops *p;
			ret = 0;
			p = clk->clk_ops;
			while(p){
				ops_run_count++;
				if(p->clk_disable_before)
					ret = p->clk_disable_before(p->privdata);
				if(ret != 0)
					break;
				p = p->next;
			}
		}
		
		//do clock gate disable
		if(ret == 0){
			if(clk->disable)
				ret = clk->disable(clk);
			else if(clk->clk_gate_reg_adr != 0){
					aml_clr_reg32_mask(clk->clk_gate_reg_adr,clk->clk_gate_reg_mask);
					ret = 0;
			}
		}
		
		//post message after clk disable.
		{
			struct clk_ops *p;
			int idx = 0;
			p = clk->clk_ops;
			while(p){
				idx++;
				if(idx > ops_run_count)
					break;
				if(p->clk_disable_after)
						p->clk_disable_after(p->privdata,ret);																	
				p = p->next;
			}
		}
		
		return ret;
}

void clk_disable(struct clk *clk)
{
		mutex_lock(&clock_ops_lock);
		meson_clk_disable(clk);
		mutex_unlock(&clock_ops_lock);
}
EXPORT_SYMBOL(clk_disable);

static unsigned long clk_msr_get(struct clk * clk)
{
	uint32_t temp;
	uint32_t cnt = 0;
	if(clk->rate>0)
	{
		return clk->rate;
	}
	if(clk->msr>0)
	{
		clk->rate = clk_util_clk_msr(clk->msr);
	}else if (clk->parent){
		cnt=clk_get_rate(clk->parent);
		cnt /= 1000000;
		clk->msr_mul=clk->msr_mul?clk->msr_mul:1;
		clk->msr_div=clk->msr_div?clk->msr_div:1;
		temp=cnt*clk->msr_mul;
		clk->rate=temp/clk->msr_div;
		clk->rate *= 1000000;
	}
	return clk->rate;
}

#define PLL_CLK_DEFINE(name,msr)    		\
	static unsigned pll_##name##_data[10];	\
    CLK_DEFINE(pll_##name,xtal,msr,set_##name##_pll, \
    		clk_msr_get,NULL,NULL,&pll_##name##_data)
_Pragma("GCC diagnostic ignored \"-Wdeclaration-after-statement\"");
#define PLL_RELATION_DEF(child,parent) meson_clk_register(&clk_pll_##child,&clk_##parent)
#define CLK_PLL_CHILD_DEF(child,parent) meson_clk_register(&clk_##child,&clk_pll_##parent)

#define CLK_DEFINE(devid,conid,msr_id,setrate,getrate,en,dis,privdata)  \
    static struct clk clk_##devid={                                     \
        .set_rate=setrate,.get_rate=getrate,.enable=en,.disable=dis,    \
        .priv=privdata,.parent=&clk_##conid ,.msr=msr_id                \
    };                                                                  \
    static struct clk_lookup clk_lookup_##devid={                       \
        .dev_id=#devid,.con_id=#conid,.clk=&clk_##devid                 \
    };clkdev_add(&clk_lookup_##devid)



static struct clk clk_xtal = {
	.rate		= -1,
	.get_rate	= clk_get_rate_xtal,
};

static struct clk_lookup clk_lookup_xtal = {
	.dev_id		= "xtal",
	.con_id		= NULL,
	.clk		= &clk_xtal
};
// pll_fixed is the pll_misc
static int __init meson_clock_init(void)
{
		clkdev_add(&clk_lookup_xtal);
		CLK_DEFINE(pll_ddr,xtal,3,NULL,clk_msr_get,NULL,NULL,NULL);
		PLL_CLK_DEFINE(sys,(unsigned long)-1);
		PLL_CLK_DEFINE(fixed,MISC_PLL_CLK);
		clk_pll_fixed.msr_mul = 125 *2;// need fix
		clk_pll_fixed.msr_div = 3;
		
		clk_pll_sys.get_rate = clk_get_rate_sys;
		clk_pll_sys.max = 1600000000;
		clk_pll_sys.min = 200000000;
		
		clk_pll_ddr.max = 1512000000;//1.5G
		clk_pll_ddr.min = 187500000;//187M
		
		clk_pll_fixed.max = 800000000; 
		clk_pll_fixed.min = 400000000;
	
		//create pll tree
		PLL_RELATION_DEF(sys,xtal);
		PLL_RELATION_DEF(ddr,xtal);
		PLL_RELATION_DEF(fixed,xtal);
		
		// Add clk81
#ifdef CONFIG_CLK81_DFS
		CLK_DEFINE(clk81, pll_fixed, 7, clk_set_rate_clk81, clk_msr_get, NULL, NULL, NULL);
#else
		CLK_DEFINE(clk81, pll_fixed, 7, NULL, clk_msr_get, NULL, NULL, NULL);
#endif 
	
		// Add clk81 as pll_fixed's child
		CLK_PLL_CHILD_DEF(clk81, fixed);
	
		clk_clk81.clk_gate_reg_adr = P_HHI_MPEG_CLK_CNTL;
		clk_clk81.clk_gate_reg_mask = (1<<7);
		clk_clk81.open_irq = 1;
	
		// Add CPU clock
		CLK_DEFINE(a9_clk, pll_sys,31, clk_set_rate_a9, clk_get_rate_a9, NULL, NULL, NULL);
		clk_a9_clk.min = 100000000;
		clk_a9_clk.max = 800000000;
		//clk_a9_clk.open_irq = 1;
		CLK_PLL_CHILD_DEF(a9_clk,sys);
	
			
		{
			// Dump clocks
			char *clks[] = { 
					"xtal",
					"pll_sys",
					"pll_fixed",
					"pll_ddr",
					"a9_clk",
					"clk81",
			};
			int i;
			int count = ARRAY_SIZE(clks);
			struct clk *clk;
	
			for (i = 0; i < count; i++) {
				char *clk_name = clks[i];
	
				clk = clk_get_sys(clk_name, NULL);
				if (!IS_CLK_ERR(clk))
					printk("clkrate [ %s \t] : %lu\n", clk_name, clk_get_rate(clk));
			}
		}

    return 0;
}

/* initialize clocking early to be available later in the boot */
core_initcall(meson_clock_init);

