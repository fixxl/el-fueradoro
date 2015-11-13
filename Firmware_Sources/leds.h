/*
 * leds.h
 *
 * Definitionen und Funktionen für die Status-LEDs
 */

#ifndef LED_H_
#define LED_H_

#define LED_YELLOW_P    B
#define LED_YELLOW_NUM    0
#define LED_RED_P     D
#define LED_RED_NUM     6
#define LED_GREEN_P     D
#define LED_GREEN_NUM   7
#define LED_ORANGE_P    D
#define LED_ORANGE_NUM    5

// DO NOT CHANGE ANYTHING BELOW THIS LINE

void led_yellow_on(void);
void led_yellow_off(void);
void led_yellow_toggle(void);
void led_red_on(void);
void led_red_off(void);
void led_red_toggle(void);
void led_green_on(void);
void led_green_off(void);
void led_green_toggle(void);
void led_orange_on(void);
void led_orange_off(void);
void led_orange_toggle(void);
void led_init(void);
void leds_off(void);
void leds_on(void);
uint8_t leds_status(void);

#define LED_YELLOW_PORT   PORT(LED_YELLOW_P)
#define LED_YELLOW_PIN    PIN(LED_YELLOW_P)
#define LED_YELLOW_POS    LED_YELLOW_NUM
#define LED_YELLOW_DDR    DDR(LED_YELLOW_P)

#define LED_RED_PORT    PORT(LED_RED_P)
#define LED_RED_PIN     PIN(LED_RED_P)
#define LED_RED_POS     LED_RED_NUM
#define LED_RED_DDR     DDR(LED_RED_P)

#define LED_GREEN_PORT    PORT(LED_GREEN_P)
#define LED_GREEN_PIN   PIN(LED_GREEN_P)
#define LED_GREEN_POS   LED_GREEN_NUM
#define LED_GREEN_DDR   DDR(LED_GREEN_P)

#define LED_ORANGE_PORT   PORT(LED_ORANGE_P)
#define LED_ORANGE_PIN    PIN(LED_ORANGE_P)
#define LED_ORANGE_POS    LED_ORANGE_NUM
#define LED_ORANGE_DDR    DDR(LED_ORANGE_P)




#endif
