/*
 * leds.h
 *
 * Definitionen und Funktionen für die Status-LEDs
 */

#ifndef LED_H_
#define LED_H_

#define LED_YELLOW_P		B
#define LED_YELLOW_NUM		0
#define LED_RED_P			D
#define LED_RED_NUM			7
#define LED_GREEN_P			D
#define LED_GREEN_NUM		6
#define LED_BLUE_P			D
#define LED_BLUE_NUM		5

void led_yellow_on(void);
void led_yellow_off(void);
void led_yellow_toggle(void);
void led_red_on(void);
void led_red_off(void);
void led_red_toggle(void);
void led_green_on(void);
void led_green_off(void);
void led_green_toggle(void);
void led_blue_on(void);
void led_blue_off(void);
void led_blue_toggle(void);
void led_init(void);
void leds_off(void);

#define LED_YELLOW_PORT		PORT(LED_YELLOW_P)
#define LED_YELLOW_PIN		PIN(LED_YELLOW_P)
#define LED_YELLOW_POS		LED_YELLOW_NUM
#define LED_YELLOW_DDR		DDR(LED_YELLOW_P)

#define LED_RED_PORT		PORT(LED_RED_P)
#define LED_RED_PIN			PIN(LED_RED_P)
#define LED_RED_POS			LED_RED_NUM
#define LED_RED_DDR			DDR(LED_RED_P)

#define LED_GREEN_PORT		PORT(LED_GREEN_P)
#define LED_GREEN_PIN		PIN(LED_GREEN_P)
#define LED_GREEN_POS		LED_GREEN_NUM
#define LED_GREEN_DDR		DDR(LED_GREEN_P)

#define LED_BLUE_PORT		PORT(LED_BLUE_P)
#define LED_BLUE_PIN		PIN(LED_BLUE_P)
#define LED_BLUE_POS		LED_BLUE_NUM
#define LED_BLUE_DDR		DDR(LED_BLUE_P)




#endif
