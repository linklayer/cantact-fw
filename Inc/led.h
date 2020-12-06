#ifndef _LED_H
#define _LED_H

#define LED_DURATION 50

#ifdef LINKLAYER
// LINKLAYER CANTACT-HW
#define LED_GPIO        GPIOB
#define LED_GPIO_CLK    __GPIOB_CLK_ENABLE
#define LED_GREEN       (GPIO_PIN_0)
#define LED_RED         (GPIO_PIN_1)

#else

// STM32F072B-DISCOVERY

#define LED_GPIO        GPIOC
#define LED_GPIO_CLK    __GPIOC_CLK_ENABLE
#define LED_GREEN       (GPIO_PIN_9)
#define LED_RED         (GPIO_PIN_8)

#endif

void led_on(void);
void led_process(void);

#endif
