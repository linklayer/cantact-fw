#ifndef _LED_H
#define _LED_H


#define LED_BLUE_Pin GPIO_PIN_1
#define LED_BLUE_Port GPIOB
#define LED_BLUE LED_BLUE_Port , LED_BLUE_Pin

#define LED_GREEN_Pin GPIO_PIN_0
#define LED_GREEN_Port GPIOB
#define LED_GREEN LED_GREEN_Port , LED_GREEN_Pin



#define LED_DURATION 25 

void led_init();
void led_blue_blink(uint8_t numblinks);
void led_green_on(void);
void led_green_off(void);
void led_blue_on(void);
void led_process(void);

#endif
