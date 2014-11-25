/*
 * leds.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

void led_yellow_on(void) {
	LED_YELLOW_PORT |= 1 << LED_YELLOW_POS;
}

void led_yellow_toggle(void) {
	LED_YELLOW_PIN = 1 << LED_YELLOW_POS;
}

void led_yellow_off(void) {
	LED_YELLOW_PORT &= ~(1 << LED_YELLOW_POS);
}

void led_red_on(void) {
	LED_RED_PORT |= 1 << LED_RED_POS;
}

void led_red_toggle(void) {
	LED_RED_PIN = 1 << LED_RED_POS;
}

void led_red_off(void) {
	LED_RED_PORT &= ~(1 << LED_RED_POS);
}

void led_green_on(void) {
	LED_GREEN_PORT |= 1 << LED_GREEN_POS;
}

void led_green_toggle(void) {
	LED_GREEN_PIN = 1 << LED_GREEN_POS;
}

void led_green_off(void) {
	LED_GREEN_PORT &= ~(1 << LED_GREEN_POS);
}

void led_blue_on(void) {
	LED_BLUE_PORT |= 1 << LED_BLUE_POS;
}

void led_blue_toggle(void) {
	LED_BLUE_PIN = 1 << LED_BLUE_POS;
}

void led_blue_off(void) {
	LED_BLUE_PORT &= ~(1 << LED_BLUE_POS);
}

void leds_off(void) {
	LED_BLUE_PORT &= ~(1 << LED_BLUE_POS);
	LED_GREEN_PORT &= ~(1 << LED_GREEN_POS);
	LED_RED_PORT &= ~(1 << LED_RED_POS);
	LED_YELLOW_PORT &= ~(1 << LED_YELLOW_POS);
}

void leds_on(void) {
	LED_BLUE_PORT |= (1 << LED_BLUE_POS);
	LED_GREEN_PORT |= (1 << LED_GREEN_POS);
	LED_RED_PORT |= (1 << LED_RED_POS);
	LED_YELLOW_PORT |= (1 << LED_YELLOW_POS);
}

uint8_t leds_status(void) {
	uint8_t status = 0;

	if(LED_YELLOW_PIN & (1<<LED_YELLOW_POS)) status |= 1;
	if(LED_GREEN_PIN & (1<<LED_GREEN_POS)) status |= 2;
	if(LED_BLUE_PIN & (1<<LED_BLUE_POS)) status |= 4;
	if(LED_RED_PIN & (1<<LED_RED_POS)) status |= 8;

	return status;
}

void led_init(void) {
	led_yellow_off();
	LED_YELLOW_DDR |= 1 << LED_YELLOW_POS;
	led_red_off();
	LED_RED_DDR |= 1 << LED_RED_POS;
	led_green_off();
	LED_GREEN_DDR |= 1 << LED_GREEN_POS;
	led_blue_off();
	LED_BLUE_DDR |= 1 << LED_BLUE_POS;
}
