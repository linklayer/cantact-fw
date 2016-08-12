#ifndef _LED_H
#define _LED_H


#define LED_BLUE_Pin GPIO_PIN_1
#define LED_BLUE_Port GPIOB
#define LED_BLUE LED_BLUE_Port , LED_BLUE_Pin


#define LED_DURATION 50

void led_init();
void led_on(void);
void led_process(void);

#endif
