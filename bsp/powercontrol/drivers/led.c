/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */
#include <rtthread.h>
#include <stm32f10x.h>
#include "led.h"

#define STM32_SIMULATOR
// led define
#ifdef  STM32_SIMULATOR
#define led1_rcc                    RCC_APB2Periph_GPIOA
#define led1_gpio                   GPIOA
#define led1_pin                    (GPIO_Pin_15)

#define PowCtr_rcc                    RCC_APB2Periph_GPIOA
#define PowCtr_gpio                   GPIOA
#define PowCtr_pin                    (GPIO_Pin_1)

#define fdog_rcc                    RCC_APB2Periph_GPIOC
#define fdog_gpio                   GPIOC
#define fdog_pin                    (GPIO_Pin_4)

#else

#define led1_rcc                    RCC_APB2Periph_GPIOE
#define led1_gpio                   GPIOE
#define led1_pin                    (GPIO_Pin_2)

#define led2_rcc                    RCC_APB2Periph_GPIOE
#define led2_gpio                   GPIOE
#define led2_pin                    (GPIO_Pin_3)

#endif // led define #ifdef STM32_SIMULATOR

void rt_hw_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(led1_rcc | PowCtr_rcc | fdog_rcc,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = led1_pin;
    GPIO_Init(led1_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = PowCtr_pin;
    GPIO_Init(PowCtr_gpio, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin   = fdog_pin;
    GPIO_Init(fdog_gpio, &GPIO_InitStructure);
}

void rt_hw_led_on(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
        GPIO_SetBits(led1_gpio, led1_pin);
        break;
    case 1:
        GPIO_SetBits(PowCtr_gpio, PowCtr_pin);
        break;
		case 2:
        GPIO_SetBits(fdog_gpio, fdog_pin);
        break;
    default:
        break;
    }
}

void rt_hw_led_off(rt_uint32_t n)
{
    switch (n)
    {
    case 0:
        GPIO_ResetBits(led1_gpio, led1_pin);
        break;
    case 1:
        GPIO_ResetBits(PowCtr_gpio, PowCtr_pin);
        break;
		case 2:
        GPIO_ResetBits(fdog_gpio, fdog_pin);
        break;
    default:
        break;
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
static rt_uint8_t led_inited = 0;
void led(rt_uint32_t led, rt_uint32_t value)
{
    /* init led configuration if it's not inited. */
    if (!led_inited)
    {
        rt_hw_led_init();
        led_inited = 1;
    }

    if ( led == 0 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(0);
            break;
        case 1:
            rt_hw_led_on(0);
            break;
        default:
            break;
        }
    }

    if ( led == 1 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(1);
            break;
        case 1:
            rt_hw_led_on(1);
            break;
        default:
            break;
        }
    }
}
FINSH_FUNCTION_EXPORT(led, set led[0 - 1] on[1] or off[0].)
#endif

