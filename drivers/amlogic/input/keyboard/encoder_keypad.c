/*
 * drivers/amlogic/input/adc_keypad/endcoder_keypad.c
 *
 * Encoder Keypad Driver
 *
 * Copyright (C) 2014 Amlogic, Inc.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * author :   Geng Li
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/irq.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <mach/gpio.h>
#include <uapi/linux/input.h>
#include <mach/am_regs.h>
#include <mach/pinmux.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/of.h>
#include <linux/amlogic/aml_gpio_consumer.h>
#include <linux/switch.h>
#include <plat/wakeup.h>

#define DEBUG
#define MOD_NAME       "encoder_keypad"

struct encoder_key{
    int plus_code;       /* plus key code */
    int minus_code;      /* minus key code */
    int A_pin;           /* PA pin */
    int B_pin;           /* PB pin */
    int A_pin_level;     /* PA pin level*/
    int B_pin_level;     /* PB pin level*/
    int rising_irq;      /* Rising IRQ number */
    int falling_irq;     /* Falling IRQ number */
};

struct encoder_kp_platform_data{
    struct encoder_key *keys;
    int key_num;
    int repeat_delay;
    int repeat_period;
};

struct kp {
    struct input_dev *input;
    struct timer_list timer;
    int config_major;
    char config_name[20];
    struct class *config_class;
    struct device *config_dev;
    int key_num;
    struct encoder_key *keys;
    struct work_struct work_update1;
    struct work_struct work_update2;
    struct work_struct work_update3;
    struct work_struct work_update4;
    int edge_flag[4];
};

#ifdef DEBUG
static struct work_struct work_debug;
static int error_code;
#define dbg_print(...) {printk("%s %d : ",__FUNCTION__,__LINE__);printk(__VA_ARGS__);}
#else
#define dbg_print(...)
#endif

#ifndef CONFIG_OF
#define CONFIG_OF
#endif
static struct kp *encoder_kp;

static int encoder_key_config_open(struct inode *inode, struct file *file)
{
    file->private_data = encoder_kp;
    return 0;
}

static int encoder_key_config_release(struct inode *inode, struct file *file)
{
    file->private_data=NULL;
    return 0;
}

static const struct file_operations keypad_fops = {
    .owner      = THIS_MODULE,
    .open       = encoder_key_config_open,
    .release    = encoder_key_config_release,
};

#ifdef DEBUG
static void debug_info_func(struct work_struct *work)
{
    printk("%s: error_code = %d\n", __func__, error_code);
}
#endif /* DEBUG */

static void report_key_func(struct kp *kp_data, int i)
{
    int code;

    if(kp_data->edge_flag[i])
        code = kp_data->keys[i].minus_code;
    else
        code = kp_data->keys[i].plus_code;
    input_report_key(kp_data->input, code, 1);
    input_sync(kp_data->input);
    input_report_key(kp_data->input, code, 0);
    input_sync(kp_data->input);
    dbg_print("%d key\n", code);
}

static void update_work_func1(struct work_struct *work)
{
    struct kp *kp_data = container_of(work, struct kp, work_update1);

    report_key_func(kp_data, 0);
}

static void update_work_func2(struct work_struct *work)
{
    struct kp *kp_data = container_of(work, struct kp, work_update2);

    report_key_func(kp_data, 1);
}

static void update_work_func3(struct work_struct *work)
{
    struct kp *kp_data = container_of(work, struct kp, work_update3);

    report_key_func(kp_data, 2);
}

static void update_work_func4(struct work_struct *work)
{
    struct kp *kp_data = container_of(work, struct kp, work_update4);

    report_key_func(kp_data, 3);
}

static irqreturn_t kp_isr_rising1(int irq, void *data)
{
    disable_irq_nosync(irq);
    if(encoder_kp){
        if(encoder_kp->keys){
            if(!amlogic_get_value(encoder_kp->keys[0].A_pin, MOD_NAME)){
                encoder_kp->edge_flag[0] = 1;
                schedule_work(&(encoder_kp->work_update1));
                }
            }
#ifdef DEBUG
        else{
            error_code = -1;
            schedule_work(&work_debug);
            }
#endif /*DEBUG*/
        }
#ifdef DEBUG
    else{
        error_code = -2;
        schedule_work(&work_debug);
        }
#endif /* DEBUG*/
    enable_irq(irq);
    return IRQ_HANDLED;
}

static irqreturn_t kp_isr_falling1(int irq, void *data)
{
    disable_irq_nosync(irq);
    if(encoder_kp){
        if(encoder_kp->keys){
            if(!amlogic_get_value(encoder_kp->keys[0].A_pin, MOD_NAME)){
                encoder_kp->edge_flag[0] = 0;
                schedule_work(&(encoder_kp->work_update1));
                }
            }
#ifdef DEBUG
        else{
            error_code = -1;
            schedule_work(&work_debug);
            }
#endif /* DEBUG */
        }
#ifdef DEBUG
    else{
        error_code = -2;
        schedule_work(&work_debug);
        }
#endif /* DEBUG*/
    enable_irq(irq);
    return IRQ_HANDLED;
}

static irqreturn_t kp_isr_rising2(int irq, void *data)
{
    disable_irq_nosync(irq);
    if(encoder_kp){
        if(encoder_kp->keys){
            if(!amlogic_get_value(encoder_kp->keys[1].A_pin, MOD_NAME)){
                encoder_kp->edge_flag[1] = 1;
                schedule_work(&(encoder_kp->work_update2));
                }
            }
#ifdef DEBUG
        else{
            error_code = -1;
            schedule_work(&work_debug);
            }
#endif /*DEBUG*/
        }
#ifdef DEBUG
    else{
        error_code = -2;
        schedule_work(&work_debug);
        }
#endif /* DEBUG*/
    enable_irq(irq);
    return IRQ_HANDLED;
}

static irqreturn_t kp_isr_falling2(int irq, void *data)
{
    disable_irq_nosync(irq);
    if(encoder_kp){
        if(encoder_kp->keys){
            if(!amlogic_get_value(encoder_kp->keys[1].A_pin, MOD_NAME)){
                encoder_kp->edge_flag[1] = 0;
                schedule_work(&(encoder_kp->work_update2));
                }
            }
#ifdef DEBUG
        else{
            error_code = -1;
            schedule_work(&work_debug);
            }
#endif /* DEBUG */
        }
#ifdef DEBUG
    else{
        error_code = -2;
        schedule_work(&work_debug);
        }
#endif /* DEBUG*/
    enable_irq(irq);
    return IRQ_HANDLED;
}

static irqreturn_t kp_isr_rising3(int irq, void *data)
{
    disable_irq_nosync(irq);
    if(encoder_kp){
        if(encoder_kp->keys){
            if(!amlogic_get_value(encoder_kp->keys[2].A_pin, MOD_NAME)){
                encoder_kp->edge_flag[2] = 1;
                schedule_work(&(encoder_kp->work_update3));
                }
            }
#ifdef DEBUG
        else{
            error_code = -1;
            schedule_work(&work_debug);
            }
#endif /*DEBUG*/
        }
#ifdef DEBUG
    else{
        error_code = -2;
        schedule_work(&work_debug);
        }
#endif /* DEBUG*/
    enable_irq(irq);
    return IRQ_HANDLED;
}

static irqreturn_t kp_isr_falling3(int irq, void *data)
{
    disable_irq_nosync(irq);
    if(encoder_kp){
        if(encoder_kp->keys){
            if(!amlogic_get_value(encoder_kp->keys[2].A_pin, MOD_NAME)){
                encoder_kp->edge_flag[2] = 0;
                schedule_work(&(encoder_kp->work_update3));
                }
            }
#ifdef DEBUG
        else{
            error_code = -1;
            schedule_work(&work_debug);
            }
#endif /* DEBUG */
        }
#ifdef DEBUG
    else{
        error_code = -2;
        schedule_work(&work_debug);
        }
#endif /* DEBUG*/
    enable_irq(irq);
    return IRQ_HANDLED;
}

static irqreturn_t kp_isr_rising4(int irq, void *data)
{
    disable_irq_nosync(irq);
    if(encoder_kp){
        if(encoder_kp->keys){
            if(!amlogic_get_value(encoder_kp->keys[3].A_pin, MOD_NAME)){
                encoder_kp->edge_flag[3] = 1;
                schedule_work(&(encoder_kp->work_update4));
                }
            }
#ifdef DEBUG
        else{
            error_code = -1;
            schedule_work(&work_debug);
            }
#endif /*DEBUG*/
        }
#ifdef DEBUG
    else{
        error_code = -2;
        schedule_work(&work_debug);
        }
#endif /* DEBUG*/
    enable_irq(irq);
    return IRQ_HANDLED;
}

static irqreturn_t kp_isr_falling4(int irq, void *data)
{
    disable_irq_nosync(irq);
    if(encoder_kp){
        if(encoder_kp->keys){
            if(!amlogic_get_value(encoder_kp->keys[3].A_pin, MOD_NAME)){
                encoder_kp->edge_flag[3] = 0;
                schedule_work(&(encoder_kp->work_update4));
                }
            }
#ifdef DEBUG
        else{
            error_code = -1;
            schedule_work(&work_debug);
            }
#endif /* DEBUG */
        }
#ifdef DEBUG
    else{
        error_code = -2;
        schedule_work(&work_debug);
        }
#endif /* DEBUG*/
    enable_irq(irq);
    return IRQ_HANDLED;
}

static int encoder_key_probe(struct platform_device *pdev)
{
    int ret, i;
    struct encoder_kp_platform_data *pdata = NULL;
    int *key_param = NULL;
    const char *gpioname;

    printk("Enter %s\n", __FUNCTION__);
#ifdef CONFIG_OF
    if(!pdev->dev.of_node){
        printk("%s: pdev->dev.of_node == NULL!\n", __func__);
        ret = -EINVAL;
        goto get_key_node_fail;
        }
    pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
    if(!pdata){
        dev_err(&pdev->dev, "%s: platform data is required!\n", __func__);
        ret = -EINVAL;
        goto get_key_node_fail;
        }
    ret = of_property_read_u32(pdev->dev.of_node,"key_num",&(pdata->key_num));
    if(ret){
        printk("%s: faild to get key_num!\n", __func__);
        goto get_key_mem_fail;
        }
    if((pdata->key_num%2)||(pdata->key_num==0)||(pdata->key_num > 8)){
        printk("%s: wrong key_num, it must be an even number, it can't more than 8!\n", __func__);
        ret = -EINVAL;
        goto get_key_mem_fail;
        }
    pdata->keys = kzalloc(sizeof(*(pdata->keys))*pdata->key_num/2, GFP_KERNEL);
    if(!(pdata->keys)){
        dev_err(&pdev->dev, "%s: out of memory!\n", __func__);
        ret = -EINVAL;
        goto get_key_mem_fail;
        }
    key_param = kzalloc(sizeof(*key_param)*(pdata->key_num), GFP_KERNEL);
    if(!key_param){
        printk("%s: key_param can not get mem\n", __func__);
        ret = -EINVAL;
        goto get_param_mem_fail;
        }
    ret = of_property_read_u32_array(pdev->dev.of_node,"key_code_list", key_param, pdata->key_num);
    if(ret){
        printk("%s: faild to get key_code!\n", __func__);
        kfree(key_param);
        goto get_param_mem_fail;
        }
    for(i = 0; i < pdata->key_num/2; i++){
        pdata->keys[i].plus_code = *(key_param+(i*2));
        pdata->keys[i].minus_code = *(key_param+(i*2)+1);
        }
    ret = of_property_read_u32_array(pdev->dev.of_node,"irq_num_list", key_param, pdata->key_num);
    if(ret){
        printk("%s: faild to get irq_num!\n", __func__);
        kfree(key_param);
        goto get_param_mem_fail;
        }
    for(i = 0; i < pdata->key_num/2; i++){
        pdata->keys[i].falling_irq = *(key_param+(i*2));
        pdata->keys[i].rising_irq = *(key_param+(i*2)+1);
        }
    kfree(key_param);
    for(i = 0; i < pdata->key_num/2; i++){
        ret = of_property_read_string_index(pdev->dev.of_node, "key_pin_list", i*2, &gpioname);
        if(ret){
            printk("%s: faild to get pin_list!\n", __func__);
            goto get_param_mem_fail;
            }
        ret = amlogic_gpio_name_map_num(gpioname);
        if(ret < 0){
            ret= -EINVAL;
            dev_err(&pdev->dev, "%s: %s change name to num error\n", __func__, gpioname);
            goto get_param_mem_fail;
            }
        pdata->keys[i].A_pin = ret;
        ret = of_property_read_string_index(pdev->dev.of_node, "key_pin_list", i*2+1, &gpioname);
        if(ret){
            printk("%s: faild to get pin_list!\n", __func__);
            goto get_param_mem_fail;
            }
        ret = amlogic_gpio_name_map_num(gpioname);
        if(ret < 0){
            ret= -EINVAL;
            dev_err(&pdev->dev, "%s: %s change name to num error\n", __func__, gpioname);
            goto get_param_mem_fail;
            }
        pdata->keys[i].B_pin = ret;
        amlogic_gpio_request(pdata->keys[i].A_pin, MOD_NAME);
        amlogic_gpio_direction_input(pdata->keys[i].A_pin, MOD_NAME);
        amlogic_set_pull_up_down(pdata->keys[i].A_pin, 1, MOD_NAME);
        amlogic_gpio_request(pdata->keys[i].B_pin, MOD_NAME);
        amlogic_gpio_direction_input(pdata->keys[i].B_pin, MOD_NAME);
        amlogic_set_pull_up_down(pdata->keys[i].B_pin, 1, MOD_NAME);
        pdata->keys[i].A_pin_level = amlogic_get_value(pdata->keys[i].A_pin, MOD_NAME);
        pdata->keys[i].B_pin_level = amlogic_get_value(pdata->keys[i].B_pin, MOD_NAME);
        amlogic_gpio_to_irq(pdata->keys[i].B_pin, MOD_NAME,
                    AML_GPIO_IRQ(pdata->keys[i].rising_irq, FILTER_NUM7, GPIO_IRQ_RISING));
        amlogic_gpio_to_irq(pdata->keys[i].B_pin, MOD_NAME,
                    AML_GPIO_IRQ(pdata->keys[i].falling_irq, FILTER_NUM7, GPIO_IRQ_FALLING));
        }
#ifdef DEBUG
    printk("%s: key_num = %d\n", __func__, pdata->key_num);
    for(i=0; i<pdata->key_num/2;i++){
        printk("%s: key[%d]->plus_code = %d\n", __func__, i, pdata->keys[i].plus_code);
        printk("%s: key[%d]->minus_code = %d\n", __func__, i, pdata->keys[i].minus_code);
        printk("%s: key[%d]->A_pin = %d\n", __func__, i, pdata->keys[i].A_pin);
        printk("%s: key[%d]->B_pin = %d\n", __func__, i, pdata->keys[i].B_pin);
        printk("%s: key[%d]->rising_irq = %d\n", __func__, i, pdata->keys[i].rising_irq);
        printk("%s: key[%d]->falling_irq = %d\n", __func__, i, pdata->keys[i].falling_irq);
        }
#endif /* DEBUG */
#endif /* CONFIG_OF */

    encoder_kp = kzalloc(sizeof(struct kp), GFP_KERNEL);
    if(!encoder_kp){
        ret = -ENOMEM;
        goto get_param_mem_fail;
        }
    encoder_kp->input = input_allocate_device();
    if(!encoder_kp->input){
        ret = -ENOMEM;
        goto set_input_dev_failed;
        }
    encoder_kp->keys = pdata->keys;
    encoder_kp->key_num = pdata->key_num/2;

    platform_set_drvdata(pdev, pdata);
    switch(pdata->key_num/2){
        case 4 :
            INIT_WORK(&(encoder_kp->work_update4), update_work_func4);
            if(request_irq(pdata->keys[3].rising_irq + INT_GPIO_0, kp_isr_rising4, IRQF_DISABLED, "irq_rising", encoder_kp)){
                printk("%s: Failed to request gpio key up irq.\n", __func__);
                ret = -EINVAL;
                goto set_input_dev_failed;
                }
            if(request_irq(pdata->keys[3].falling_irq + INT_GPIO_0, kp_isr_falling4, IRQF_DISABLED, "irq_falling", encoder_kp)){
                printk("%s: Failed to request gpio key down irq.\n", __func__);
                ret = -EINVAL;
                goto set_input_dev_failed;
                }
        case 3 :
            INIT_WORK(&(encoder_kp->work_update3), update_work_func3);
            if(request_irq(pdata->keys[2].rising_irq + INT_GPIO_0, kp_isr_rising3, IRQF_DISABLED, "irq_rising", encoder_kp)){
                printk("%s: Failed to request gpio key up irq.\n", __func__);
                ret = -EINVAL;
                goto set_input_dev_failed;
                }
            if(request_irq(pdata->keys[2].falling_irq + INT_GPIO_0, kp_isr_falling3, IRQF_DISABLED, "irq_falling", encoder_kp)){
                printk("%s: Failed to request gpio key down irq.\n", __func__);
                ret = -EINVAL;
                goto set_input_dev_failed;
                }
        case 2 :
            INIT_WORK(&(encoder_kp->work_update2), update_work_func2);
            if(request_irq(pdata->keys[1].rising_irq + INT_GPIO_0, kp_isr_rising2, IRQF_DISABLED, "irq_rising", encoder_kp)){
                printk("%s: Failed to request gpio key up irq.\n", __func__);
                ret = -EINVAL;
                goto set_input_dev_failed;
                }
            if(request_irq(pdata->keys[1].falling_irq + INT_GPIO_0, kp_isr_falling2, IRQF_DISABLED, "irq_falling", encoder_kp)){
                printk("%s: Failed to request gpio key down irq.\n", __func__);
                ret = -EINVAL;
                goto set_input_dev_failed;
                }
        case 1 :
        default :
            INIT_WORK(&(encoder_kp->work_update1), update_work_func1);
            if(request_irq(pdata->keys[0].rising_irq + INT_GPIO_0, kp_isr_rising1, IRQF_DISABLED, "irq_rising", encoder_kp)){
                printk("%s: Failed to request gpio key up irq.\n", __func__);
                ret = -EINVAL;
                goto set_input_dev_failed;
                }
            if(request_irq(pdata->keys[0].falling_irq + INT_GPIO_0, kp_isr_falling1, IRQF_DISABLED, "irq_falling", encoder_kp)){
                printk("%s: Failed to request gpio key down irq.\n", __func__);
                ret = -EINVAL;
                goto set_input_dev_failed;
                }
        }
#ifdef DEBUG
    error_code = 0;
    INIT_WORK(&work_debug, debug_info_func);
#endif
    /* setup input device */
    set_bit(EV_KEY, encoder_kp->input->evbit);
    set_bit(EV_REP, encoder_kp->input->evbit);

    for(i = 0; i < encoder_kp->key_num; i++){
        set_bit(pdata->keys[i].plus_code, encoder_kp->input->keybit);
        set_bit(pdata->keys[i].minus_code, encoder_kp->input->keybit);
        dbg_print("key(%d) registed.\n", pdata->keys[i].plus_code);
        dbg_print("key(%d) registed.\n", pdata->keys[i].minus_code);
        }

    encoder_kp->input->name = "encoder_keypad";
    encoder_kp->input->phys = "encoder_keypad/input0";
    encoder_kp->input->dev.parent = &pdev->dev;

    encoder_kp->input->id.bustype = BUS_ISA;
    encoder_kp->input->id.vendor = 0x0001;
    encoder_kp->input->id.product = 0x0001;
    encoder_kp->input->id.version = 0x0100;

    encoder_kp->input->rep[REP_DELAY]=0xffffffff;
    encoder_kp->input->rep[REP_PERIOD]=0xffffffff;

    encoder_kp->input->keycodesize = sizeof(unsigned short);
    encoder_kp->input->keycodemax = 0x1ff;

    ret = input_register_device(encoder_kp->input);
    if (ret < 0) {
        printk("%s: Unable to register keypad input device.\n", __func__);
        ret = -EINVAL;
        goto set_input_dev_failed;
        }
    strcpy(encoder_kp->config_name, "encoder_keypad");
    ret = register_chrdev(0, encoder_kp->config_name, &keypad_fops);
    if(ret <= 0){
        printk("%s: register char device error\n", __func__);
        ret = -EINVAL;
        input_unregister_device(encoder_kp->input);
        goto set_input_dev_failed;
        }
    encoder_kp->config_major = ret;
    printk("%s: Encoder keypad major:%d\n", __func__, ret);
    encoder_kp->config_class = class_create(THIS_MODULE, encoder_kp->config_name);
    encoder_kp->config_dev = device_create(encoder_kp->config_class,      NULL,
                MKDEV(encoder_kp->config_major, 0), NULL, encoder_kp->config_name);
    return 0;
set_input_dev_failed:
    input_free_device(encoder_kp->input);
    kfree(encoder_kp);
get_param_mem_fail:
    kfree(pdata->keys);
get_key_mem_fail:
    kfree(pdata);
get_key_node_fail:
    return ret;
}

static int encoder_key_remove(struct platform_device *pdev)
{
    struct encoder_kp_platform_data *pdata = platform_get_drvdata(pdev);

    printk("Enter %s\n", __FUNCTION__);
    input_unregister_device(encoder_kp->input);
    input_free_device(encoder_kp->input);
    unregister_chrdev(encoder_kp->config_major,encoder_kp->config_name);
    if(encoder_kp->config_class){
        if(encoder_kp->config_dev)
            device_destroy(encoder_kp->config_class,MKDEV(encoder_kp->config_major,0));
        class_destroy(encoder_kp->config_class);
        }
    kfree(encoder_kp);
#ifdef CONFIG_OF
    kfree(pdata->keys);
    kfree(pdata);
#endif
    return 0;
}

static int encoder_key_suspend(struct platform_device *dev, pm_message_t state)
{
    printk("Enter %s\n", __FUNCTION__);
    return 0;
}

static int encoder_key_resume(struct platform_device *dev)
{
    printk("Enter %s\n", __FUNCTION__);
    return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id key_dt_match[]={
    {   .compatible = "amlogic,encoder_keypad",
    },
    {},
};
#else
#define key_dt_match NULL
#endif

static struct platform_driver encoder_driver = {
    .probe      = encoder_key_probe,
    .remove     = encoder_key_remove,
    .suspend    = encoder_key_suspend,
    .resume     = encoder_key_resume,
    .driver     = {
        .name   = "encoder-key",
        .of_match_table = key_dt_match,
    },
};

static int __init encoder_init(void)
{
    printk(KERN_INFO "Encoder Keypad Driver init.\n");
    return platform_driver_register(&encoder_driver);
}

static void __exit encoder_exit(void)
{
    printk(KERN_INFO "Encoder Keypad Driver exit.\n");
    platform_driver_unregister(&encoder_driver);
}

module_init(encoder_init);
module_exit(encoder_exit);

MODULE_AUTHOR("Geng Li");
MODULE_DESCRIPTION("Encoder Keypad Driver");
MODULE_LICENSE("GPL");
