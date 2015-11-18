/*
 * shiftregister.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// Initialize
void sr_init(void) {
	// Activate pullup at /OE, so /OE stays high when switching from input to output
	// and outputs of the 74HC595 stay in high-Z-mode!
	OE_PORT |= (1 << OE);

	// Configure pins as outputs
	SER_IN_DDR |= 1 << SER_IN;
	OE_DDR     |= 1 << OE;
	RCLOCK_DDR |= 1 << RCLOCK;
	SCLOCK_DDR |= 1 << SCLOCK;

	// All outputs except /OE to low
	SER_IN_PORT &= ~(1 << SER_IN);
	RCLOCK_PORT &= ~(1 << RCLOCK);
	SCLOCK_PORT &= ~(1 << SCLOCK);

	// Set shift-register values to 0...
	for (uint8_t j = 3; j; j--) {
		for (uint8_t i = (SR_CHANNELS + 4); i; i--) {
			SCLOCK_PIN = (1 << SCLOCK); // Pin high after toggling
			SCLOCK_PIN = (1 << SCLOCK); // Pin low after toggling
		}

		// ... and apply shift-register to output register.
		RCLOCK_PIN = (1 << RCLOCK); // Pin durch Toggling high
		RCLOCK_PIN = (1 << RCLOCK); // Pin durch Toggling low
	}

	_delay_ms(1);

	// Now outputs are all low and can be activated!
	OE_PORT &= ~(1 << OE);
}

void sr_enable(void) {
	OE_PORT &= ~(1 << OE);
}

void sr_disable(void) {
	OE_PORT |= (1 << OE);
}

// Transfer 16 bit pattern to outputs
void sr_shiftout(uint16_t scheme) {
	uint8_t i;
	uint16_t mask = 1 << (SR_CHANNELS - 1);

	SER_IN_PORT &= ~(1 << SER_IN);
	SCLOCK_PORT &= ~(1 << SCLOCK);
	RCLOCK_PORT &= ~(1 << RCLOCK);

	for (i = SR_CHANNELS; i; i--) {
		if (scheme & mask) {
			SER_IN_PORT |= 1 << SER_IN;
		}

		SCLOCK_PIN   = (1 << SCLOCK); // Pin high after toggling
		SCLOCK_PIN   = (1 << SCLOCK); // Pin low after toggling
		SER_IN_PORT &= ~(1 << SER_IN);
		mask       >>= 1;
	}

	RCLOCK_PIN = (1 << RCLOCK);    // Pin high after toggling
	RCLOCK_PIN = (1 << RCLOCK);    // Pin low after toggling
}
