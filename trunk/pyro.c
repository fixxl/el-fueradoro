/*
 *           Firmware
 *         EL FUERADORO
 *
 *         Felix Pflaum
 *
 */

// Everything to be included is in global.h
#include "global.h"

// Global Variables
volatile uint8_t transmit_flag = 0, key_flag = 0;
volatile uint16_t clear_lcd_tx_flag = 0, clear_lcd_rx_flag = 0, hist_del_flag = 0;

void wdt_init(void) {
	MCUSR = 0;
	wdt_disable();
	return;
}

// Initialise Key-Switch
void key_init(void) {
	KEYPORT |= (1 << KEY);
	KEYDDR &= ~(1 << KEY);

	// Activate Pin-Change Interrupt
	if (KEYPORT == PORTB) {
		PCICR |= (1 << PCIE0);
		PCMSK0 |= (1 << KEY);
	}

	if (KEYPORT == PORTD) {
		PCICR |= (1 << PCIE2);
		PCMSK2 |= (1 << KEY);
	}
	if (KEYPORT == PORTC) {
		PCICR |= (1 << PCIE1);
		PCMSK1 |= (1 << KEY);
	}
}

// Un-initialise Key-Switch (needed only if a device configured as ignition device gets configured as transmitter while on)
void key_deinit(void) {
	KEYPORT &= ~(1 << KEY);
	KEYDDR &= ~(1 << KEY);

	// Deactivate Pin-Change Interrupt
	if (KEYPORT == PORTB) {
		PCICR &= ~(1 << PCIE0);
		PCMSK0 &= ~(1 << KEY);
	}

	if (KEYPORT == PORTD) {
		PCICR &= ~(1 << PCIE2);
		PCMSK2 &= ~(1 << KEY);
	}
	if (KEYPORT == PORTC) {
		PCICR &= ~(1 << PCIE1);
		PCMSK1 &= ~(1 << KEY);
	}
}

// Schalterentprellung
uint8_t debounce(volatile uint8_t *port, uint8_t pin) {
	uint8_t keystate = 0x55, ctr = 0, timer0_regb = TCCR0B;

	// Timer 0 zur�cksetzen
	TCNT0 = 0;
	TIFR0 = (1 << TOV0);
	TCNT0 = 160;
	TCCR0B = (1 << CS02 | 1 << CS00);

	// Query switch state every 10 ms until 8 identical states in a row are received
	while (((keystate != 0x00) && (keystate != 0xFF)) || (ctr < 8)) {
		keystate <<= 1; // Shift left
		keystate |= ((*port & (1 << pin)) > 0); // Write 0 or 1 to LSB
		while (!(TIFR0 & (1 << TOV0)))
			// Wait for timer overflow
			;
		TIFR0 = (1 << TOV0); // Clear interrupt flag
		TCNT0 = 160; // Preload timer
		if (ctr < 8) ctr++; // Make sure at least 8 queries are executed
	}
	TCCR0B = timer0_regb;

	// return 1 for active switch, 0 for inactive
	return (keystate == 0);
}

// Create special symbols for LCD
void create_symbols(void) {
	// Switch to CGRAM
	lcd_send(1 << 6, 0);

	// Symbol "fire"
	lcd_cgwrite(4);
	lcd_cgwrite(4);
	lcd_cgwrite(10);
	lcd_cgwrite(10);
	lcd_cgwrite(21);
	lcd_cgwrite(17);
	lcd_cgwrite(10);
	lcd_cgwrite(4);

	// Symbol "�"
	lcd_cgwrite(2);
	lcd_cgwrite(5);
	lcd_cgwrite(2);
	lcd_cgwrite(0);
	lcd_cgwrite(0);
	lcd_cgwrite(0);
	lcd_cgwrite(0);
	lcd_cgwrite(0);

	// Switch back to DDRAM
	lcd_send(1 << 7, 0);
}

// ------------------------------------------------------------------------------------------------------------------------

// Temperature sensor detection
uint8_t tempident(void) {
	int16_t var1 = 0, var2 = 0;
	uint8_t checkup = dht_read(&var1, &var2);

	// Type of temp. sensor is detected by return value
	switch (checkup) {
		case 0: {
			return DHT22;
			break;
		}
		case 4: {
			return DS18B20;
			break;
		}
		default: {
			return 0;
			break;
		}
	}
	return 0;
}

// Temperature measurement
int8_t tempmeas(uint8_t type) {
	int16_t temperature = -128;
	uint16_t temp_hex;

	switch (type) {
		case DS18B20: {
			w1_command(CONVERT_T, NULL);
			while (!w1_bit_io(1))
				;
			w1_command(READ, NULL);
			temp_hex = w1_byte_rd();
			temp_hex += (w1_byte_rd()) << 8;
			temperature = w1_tempread_to_celsius(temp_hex);
			break;
		}

		case DHT22: {
			int16_t humidity;
			dht_read(&temperature, &humidity);
			break;
		}
		default: {
			return -128;
		}
	}
	temperature += (temperature < 0) ? -5 : 5;
	temperature /= 10;
	return (int8_t) temperature;
}

// ------------------------------------------------------------------------------------------------------------------------

// Check if received uart-data are a valid ignition command
uint8_t uart_valid(const char *field) {
	return ((field[0] == 0xFF) && (field[1] > 0) && (field[1] < 31) && (field[2] > 0) && (field[2] < 17)
			&& (field[3] == crc8(crc8(0, field[1]), field[2])));
}

// ------------------------------------------------------------------------------------------------------------------------

// Shift LCD-cursor (line 3 and 4)
void cursor_x_shift(uint8_t* last_zeile, uint8_t* last_spalte, uint8_t* anz_zeile, uint8_t* anz_spalte) {
	uint8_t lastzeile = *last_zeile, lastspalte = *last_spalte, anzzeile = *anz_zeile,
			anzspalte = *anz_spalte;

	lastzeile = anzzeile;
	lastspalte = anzspalte;
	anzspalte += 7;
	anzzeile = anzzeile + (anzspalte > 21);
	anzspalte %= 21;
	if (anzzeile > 4) anzzeile = 3;

	*last_zeile = lastzeile, *last_spalte = lastspalte, *anz_zeile = anzzeile, *anz_spalte = anzspalte;
}

// Main programme
int main(void) {

	wdt_disable();

// Variablendefinitionen
	uint16_t scheme = 0;
	uint8_t i, nr, inp, tmp;
	uint8_t tx_length = 2, rx_length = 0;
	uint8_t temp_sreg;
	uint8_t slave_id = 30, unique_id = 30, rem_sid = 30, rem_uid = 30;
	uint8_t rfm_rx_error = 1, loopcount = 5, transmission_allowed = 1;
	uint8_t anzspalte = 1, anzzeile = 3, lastspalte = 15, lastzeile = 4;
	uint8_t armed = 0;
	uint8_t changes = 0;
	uint8_t iderrors = 0;
	uint8_t tempsenstype = 0;
	uint8_t rssi = 0;
	uint8_t ledstatus = 0;
	int8_t temperature = -128;

	bitfeld_t flags;
	flags.complete = 0;

	char uart_field[MAX_ARRAYSIZE + 2] = { 0 };
	char rx_field[MAX_ARRAYSIZE + 1] = { 0 };
	char tx_field[MAX_ARRAYSIZE + 1] = { 0 };
	char boxes[MAX_ARRAYSIZE + 1] = { 0 };
	char quantity[MAX_ARRAYSIZE + 1] = { 0 };
	char batteries[MAX_ARRAYSIZE + 1] = { 0 };
	char sharpness[MAX_ARRAYSIZE + 1] = { 0 };
	int8_t temps[MAX_ARRAYSIZE + 1] = { 0 };
	int8_t rssis[MAX_ARRAYSIZE + 1] = { 0 };
	char channel_fired[MAX_ARRAYSIZE + 1] = { 0 };
	char lcd_array[MAX_ARRAYSIZE + 1] = { 0 };

	/* For security reasons the shift registers are initialised right at the beginning to guarantee a low level at the
	 * gate pins of the MOSFETs and beware them from conducting directly after turning on the device.
	 *
	 * The SIPO-registers 74HC595 providing the 16 channels for ignition internally possess a shift register and
	 * an output register. Into the latter data from the shift register are transferred by a rising edge at the RCLOCK-pin.
	 * This leads to the situation that changes within the shift register are only visible at the outputs after a rising
	 * edge at the RCLOCK-pin.
	 *
	 * As soon as the PIN /OE (Inverted output enable) is pulled low, the outputs are enabled. The problem is that the initial
	 * output states are random and cannot be predicted nor controlled. The available "Master Reset" does only affect the internal
	 * shift register but has no influence on the outputs unless a rising edge at RCLOCK happens.
	 *
	 * To guarantee a defined power-on-state the /OE-pin needs to be pulled high until all 16 channels are set to low and
	 * written into the output register. Therefore an external pullup-resistor is connected to /OE which makes the outputs tri-state.
	 * Additional pulldown-resistor-arrays pull the shift register outputs to "low". Only after 16 zeros have been clocked into the
	 * shift register and its values have been written to the output registers /OE may be set to "low".
	 *
	 * Therefore it is strcitly recommended not to change sr_init() and to call sr_init() immediately as the first function right at
	 * the beginning of the main-programme. sr_init() takes care of a safe start for the ignition devices and doesn't do any harm to
	 * the transmitter.
	 *
	 */
	sr_init();

// Disable unused controller parts (2-wire-Interface, Timer 2, Analogue Comparator)
	PRR |= (1 << PRTWI) | (1 << PRTIM2);
	ACSR |= 1 << ACD;

// Initialise Timer
	timer1_on();

// Initialise LEDs
	led_init();

// Initialise arrays
	for (uint16_t warten = 0; warten < MAX_ARRAYSIZE; warten++) {
		led_green_toggle();
		uart_field[warten] = 1;
		tx_field[warten] = 0;
		rx_field[warten] = 0;
		boxes[warten] = 0;
		batteries[warten] = 0;
		sharpness[warten] = 0;
		temps[warten] = 0;
		channel_fired[warten] = 0;
		temps[warten] = -128;
		_delay_ms(50);
	}

// Get Slave- und Unique-ID from EEPROM, initialise device accordingly
	update_addresses(&unique_id, &slave_id);

// Initialise UART and tell the PC we're ready
	uart_init(BAUD);
	allow_uart_sending();

// Detect temperature sensor and measure temperature if possible
	tempsenstype = tempident();
	temperature = tempmeas(tempsenstype);

// Initialise radio
	rfm_init();

	if (SENDERBOX) {
		tx_length = 3;
		tx_field[0] = IDENT;
		tx_field[1] = 'd';
		tx_field[2] = '0';
		TIMSK0 |= (1 << TOIE0);

		// Transmit something to make other devices adjust frequency
		for (uint8_t j = 0; j < 5; j++) {
			led_green_on();
			rfm_transmit("UUUUU", 5);
			led_green_off();
			_delay_ms(1000);
		}
		lcd_clear();
	}
	else {
		tx_field[0] = PARAMETERS;
		tx_field[1] = slave_id;
		tx_field[2] = unique_id;
		tx_field[3] = adc_read(5);
		armed = debounce(&KEYPIN, KEY);
		if (armed) led_red_on();
		else led_red_off();
		tx_field[4] = armed;
		tx_field[5] = temperature;
		tx_length = 6;
	}
	flags.b.transmit = 1;

// Enable Interrupts
	sei();

// Main loop
	/*
	 * Within the main loop the status of all the flags is continuously monitored and upon detection of a set flag
	 * the corresponding routine is executed. Before the execution of the routine the status register is written to a local
	 * variable, interrupts are disabled and the flag gets cleared, after the routine interrupts are re-enabled
	 *
	 */
	while (1) {

// -------------------------------------------------------------------------------------------------------

// Control key switch (key_flag gets set via pin-change-interrupt)
		if (key_flag) {
			temp_sreg = SREG;
			cli();
			key_flag = 0;

			// Box armed: armed = 1, Box not armed: armed = 0

			armed = debounce(&KEYPIN, KEY);
			if (armed) led_red_on();
			else led_red_off();
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// UART-Routine
		flags.b.uart_active = ((UCSR0A & (1 << RXC0)) && 1);
		if (flags.b.uart_active) {
			temp_sreg = SREG;
			cli();
			flags.b.uart_active = 0;

			led_yellow_on();

			// Receive first char
			uart_field[0] = uart_getc();

			// React according to first char (ignition command or not?)
			// Ignition command is always 4 chars long
			if (uart_field[0] == 0xFF) {
				uart_field[1] = uart_getc();
				uart_field[2] = uart_getc();
				uart_field[3] = uart_getc();
				uart_field[4] = '\0';
			}

			// Any other command is received as long as it doesn't start with enter or backspace
			else if ((uart_field[0] == 8) || (uart_field[0] == 10) || (uart_field[0] == 13)
					|| (uart_field[0] == 127)) {
				uart_field[0] = '\0';
			}

			else {
#if !CASE_SENSITIVE
				uart_field[0] = uart_lower_case(uart_field[0]);
#endif
				uart_putc(uart_field[0]); // Show first char so everything looks as it should
				if (!uart_gets(uart_field + 1)) {
					uart_puts_P(PSTR("\033[1D"));
					uart_puts_P(PSTR(" "));
					uart_puts_P(PSTR("\033[1D"));
					uart_field[0] = '\0';
				}
				uart_puts_P(PSTR("\033]0;EL FUERADORO\007"));
			}

			// Evaluate inputs
			// "conf" starts ID configuration
			if (uart_strings_equal(uart_field, "conf")) {
				flags.b.uart_config = 1;
				flags.b.transmit = 0;
			}

			// "remote" starts remote ID configuration
			if (uart_strings_equal(uart_field, "remote")) {
				flags.b.remote = 1;
				flags.b.transmit = 0;
			}

			// "clearlist" empties list of boxes
			if (uart_strings_equal(uart_field, "clearlist")) {
				flags.b.clear_list = 1;
			}

			// "send" allows to manually send a command
			if (uart_strings_equal(uart_field, "send") || uart_strings_equal(uart_field, "fire")
					|| uart_strings_equal(uart_field, "ident") || uart_strings_equal(uart_field, "temp")) {
				flags.b.send = 1;
				flags.b.transmit = 0;
			}

			// "list" gives a overview over connected boxes
			if (uart_strings_equal(uart_field, "list")) {
				flags.b.list = 1;
				flags.b.transmit = 0;
			}

			// "orders" shows last transmitted and received command on the LCD
			if (uart_strings_equal(uart_field, "orders") && SENDERBOX) {
				flags.b.rx_post = 1;
				flags.b.tx_post = 1;
				flags.b.show_only = 1;
				flags.b.lcd_update = 1;
			}

			// "zero" deletes all "already fired channel"-flags
			if (uart_strings_equal(uart_field, "zero") && !SENDERBOX) {
				flags.b.reset_fired = 1;
			}

			// "cls" clears terminal screen
			if (uart_strings_equal(uart_field, "cls")) {
				terminal_reset();
			}

			// "kill" resets the controller
			if (uart_strings_equal(uart_field, "kill")) {
				flags.b.reset_device = 1;
			}

			// "hardware" or "hw" outputs for uC- and rfm-type
			if (uart_strings_equal(uart_field, "hw") || uart_strings_equal(uart_field, "hardware")) {
				flags.b.hw = 1;
			}

			// "int1" last transmitted command gets re-transmitted periodically
			if (uart_strings_equal(uart_field, "int1")) {
				uart_puts_P(PSTR("\n\n\rWiederholtes Senden des letzten Befehls EIN\n\r"));
				timer1_reset();
				timer1_on();
				TIMSK1 |= (1 << TOIE1);
			}

			// "int0" periodic retransmission gets stopped
			if (uart_strings_equal(uart_field, "int0")) {
				uart_puts_P(PSTR("\n\n\rWiederholtes Senden des letzten Befehls AUS\n\r"));
				transmit_flag = 0;
				timer1_off();
				TIMSK1 &= ~(1 << TOIE1);
				timer1_reset();
			}

			// If valid ignition command was received
			if (uart_valid(uart_field)) {
				// Transmit to everybody
				tx_field[0] = FIRE;
				tx_field[1] = uart_field[1];
				tx_field[2] = uart_field[2];
				tx_field[3] = FIREREPEATS;
				tx_length = 4;
				flags.b.transmit = 1;

				// Check if ignition was triggered on device that received the serial command
				if ((slave_id == uart_field[1]) && !SENDERBOX) {
					tmp = uart_field[2] - 1;
					if (!(channel_fired[tmp]) && !flags.b.fire) {
						rx_field[2] = uart_field[2];
						flags.b.fire = 1;
						loopcount = 1;
					}
				}
			}

			led_yellow_off();
			if (uart_field[0] && (uart_field[0] != 0xFF)) uart_puts_P(PSTR("\n\r"));

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Hardware
		if (flags.b.hw) {
			temp_sreg = SREG;
			cli();
			flags.b.hw = 0;

			uart_puts_P(PSTR("\n\rController:   "));
			uart_puts_P(PSTR(STRINGIZE_VALUE_OF(MCU)));
			uart_puts_P(PSTR("\n\rRadio module: RFM"));
			uart_shownum(RFM, 'd');
			uart_puts_P(PSTR("\n\n\r"));

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Slave- and Unique-ID settings
		if (flags.b.uart_config) {
			temp_sreg = SREG;
			cli();
			flags.b.uart_config = 0;

			timer1_reset();
			timer1_off();

			changes = configprog();
			if (changes) flags.b.reset_device = 1;
			changes = 0;
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Remote Slave- and Unique-ID settings
		if (flags.b.remote) {
			temp_sreg = SREG;
			cli();
			flags.b.remote = 0;

			timer1_reset();
			timer1_off();

			changes = 0;
			changes = remote_config(tx_field);
			if (changes) {
				tx_length = 5;
				uart_puts_P(PSTR("\n\rTransmitting remote config command!\n\r"));

				// If the numbers are those of the connected device
				if (!armed && tx_field[1] == unique_id && tx_field[2] == slave_id) {
					savenumber(tx_field[3], tx_field[4]);
					flags.b.reset_device = 1;
				}
				// If not...
				else {
					flags.b.transmit = 1;
				}

				uart_puts_P(PSTR("\n\n\r"));
			}

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Reset fired channels
		if (flags.b.reset_fired) {
			temp_sreg = SREG;
			cli();
			flags.b.reset_fired = 0;

			for (i = 0; i < 16; i++) {
				channel_fired[i] = 0;
			}

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Manual transmission
		if (flags.b.send) {
			temp_sreg = SREG;
			cli();

			flags.b.send = 0;

			timer1_reset();
			timer1_off();

			nr = 0, tmp = 0, inp = 0;

			switch (uart_field[0]) {
				case FIRE:
				case IDENT:
				case TEMPERATURE: {
					inp = uart_field[0];
					break;
				}
				default: {
					uart_puts_P(PSTR("\n\rModus(f/i/t): "));
					while (!inp) {
						inp = uart_getc() | 0x20;
					}
					uart_putc(inp);
					break;
				}
			}
			tx_field[0] = inp;

			if (tx_field[0] == FIRE || tx_field[0] == IDENT || tx_field[0] == TEMPERATURE) {
				// Assume, that a transmission shall take place
				tmp = 1;

				// Handle slave-id and channel input
				switch (tx_field[0]) {
					case FIRE: {
						for (uint8_t round = 1; round < 3; round++) { // Loop twice
							nr = 0;
							uart_puts_P(round < 2 ? PSTR("\n\rSlave-ID:   ") : PSTR("\n\rKanal:      ")); // First for slave-id, then for channel
							for (i = 0; i < 2; i++) { // Get the user to assign the numbers with 2 digits
								inp = 0;
								while (!inp) {
									inp = uart_getc();
								}
								uart_putc(inp);
								nr *= 10;
								nr += (inp - '0');
							}
							uart_puts_P(PSTR(" = "));
							if (nr > 0 && nr < ((round < 2) ? 31 : 17)) { // Slave-ID has to be 1-30, Channel 1-16
								uart_shownum(nr, 'd');
								tx_field[round] = nr;
							}
							else { // Otherwise the input's invalid
								uart_puts_P(PSTR("Ung�ltige Eingabe"));
								tmp = 0; // Sending gets disallowed
								round = 3;
							}
						}
						tx_field[3] = FIREREPEATS;
						tx_length = 4;
						break;
					}
					case IDENT: {
						tx_field[0] = IDENT;
						tx_field[1] = 'd';
						tx_field[2] = '0';
						tx_length = 3;
						break;
					}
					case TEMPERATURE: {
						temperature = tempmeas(tempsenstype);

						uart_puts_P(PSTR("Temperatur: "));
						if (temperature == -128) {
							uart_puts_P(PSTR("n.a."));
						}
						else {
							fixedspace(temperature, 'd', 4);
							uart_puts_P(PSTR("�C"));
						}

						// Request other devices to refresh temperature as well
						tx_field[0] = TEMPERATURE;
						tx_field[1] = 'e';
						tx_field[2] = 'm';
						tx_field[3] = 'p';
						tx_length = 4;
						break;
					}
					default: {
						break;
					}
				}
			}

			uart_puts_P(PSTR("\n\n\r"));

			// Take action after proper command
			if (tmp) {
				flags.b.transmit = 1;
				if ((tx_field[0] == FIRE) && (slave_id == tx_field[1]) && !channel_fired[tx_field[2] - 1]) {
					rx_field[2] = tx_field[2];
					loopcount = 1;
					flags.b.fire = 1;
				}
			}
			else {
				flags.b.transmit = 0;
			}

			while (UCSR0A & (1 << RXC0)) {
				inp = UDR0;
			}

			timer1_on();
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// List network devices
		if (flags.b.list) {
			temp_sreg = SREG;
			cli();

			flags.b.list = 0;

			list_complete(boxes, batteries, sharpness, temps, rssis, iderrors);
			evaluate_boxes(boxes, quantity);
			list_array(quantity);

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Software-Reset via Watchdog
		if (flags.b.reset_device) {
			cli();
			if (SENDERBOX) {
				lcd_clear();
				lcd_puts("Resetting device!");
			}
			terminal_reset();
			wdt_enable(4);
			while (1)
				;
		}

// -------------------------------------------------------------------------------------------------------

// Clear list of ignition devices
		if (flags.b.clear_list || (flags.b.transmit && tx_field[0] == IDENT)) {
			temp_sreg = SREG;
			cli();

			flags.b.clear_list = 0;

			iderrors = 0;
			for (i = 0; i < 30; i++) {
				boxes[i] = 0;
				quantity[i] = 0;
				batteries[i] = 0;
				sharpness[i] = 0;
				temps[i] = -128;
				rssis[i] = 0;
			}

			// Ignition devices have to write themselves in the list
			if (!SENDERBOX) {
				boxes[unique_id - 1] = slave_id;
				quantity[slave_id - 1] = 1;
				batteries[unique_id - 1] = adc_read(5);
				sharpness[unique_id - 1] = (armed ? 'j' : 'n');
				temps[unique_id - 1] = temperature;
				rssis[unique_id - 1] = 0;
			}

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Transmit
		// Check if device has waited long enough (according to unique-id) to be allowed to transmit
		if (!transmission_allowed && (transmit_flag > (unique_id << 2))) transmission_allowed = 1;

		// Transmission process
		if (transmission_allowed && (flags.b.transmit || (transmit_flag > 125))) {
			temp_sreg = SREG;
			cli();

			flags.b.transmit = 0;

			led_green_on();
			rfm_transmit(tx_field, tx_length); // Transmit message
			if (tx_field[0] == FIRE) { // Repeat ignition command a certain amount of times (FIREREPEATS)
				while (--tx_field[3]) {
					rfm_transmit(tx_field, tx_length);
				}
			}
			led_green_off();

			// If transmission was not a cyclical one but a triggered one, turn of timer and its interrupts
			if (transmit_flag < 125) {
				timer1_off();
				timer1_reset();
				TIMSK1 &= ~(1 << TOIE1);
			}
			transmit_flag = 0;

			flags.b.lcd_update = 1;
			flags.b.tx_post = 1;

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Fire
		if (flags.b.fire) {
			temp_sreg = SREG;
			cli();
			flags.b.fire = 0;

			// Wait depending on number of transmission received
			for (i = loopcount - 1; i > 0; i--) {
				_delay_ms(11);
			}

			// Backup LED status
			ledstatus = leds_status();

			// Turn all leds on
			leds_on();

			if (armed && (rx_field[2] > 0) && (rx_field[2] < 17)) { // If channel number is valid
				tmp = rx_field[2]; // Save channel number to variable
				scheme = 0; // Set mask-variable to zero
				for (uint8_t i = 16; i; i--) {
					scheme <<= 1; // Left-shift mask-variable
					if (i == tmp) scheme |= 1; // Set LSB if loop variable equals channel number
				}
				sr_shiftout(scheme); // Write pattern to shift-register

				/*
				 * To avoid demage to the MOSFET in case of a short circuit after ignition, the MOSFET is set to
				 * blocking state again after 11ms. According to specification the detonator must have exploded by then.
				 */
				_delay_ms(11);

				// Lock all MOSFETs
				sr_shiftout(0);

				// Mark fired channel as fired
				channel_fired[rx_field[2] - 1] = 1;
			}

			// Turn all LEDs off
			leds_off();

			// Restore former state
			if (ledstatus & 1) led_yellow_on();
			if (ledstatus & 2) led_green_on();
			if (ledstatus & 4) led_blue_on();
			if (ledstatus & 8) led_red_on();
			ledstatus = 0;

			// Turn receiver back on
			rfm_rxon();

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Receive
		flags.b.receive = rfm_receiving();
		if (flags.b.receive) {
			temp_sreg = SREG;
			cli();

			flags.b.receive = 0;

			led_blue_on();
#ifdef RFM69_H_
			rssi = rfm_get_rssi_dbm(); 							// Measure signal strength (RFM69 only)
#endif
			rfm_rx_error = rfm_receive(rx_field, &rx_length); 	// Get Message
			led_blue_off();

			if (rfm_rx_error) rx_field[0] = ERROR;
			else {
				switch (rx_field[0]) { 							// Act according to type of message received

					// Received ignition command (only relevant for ignition devices)
					case FIRE: {
						if ((rx_field[1] == slave_id) && !SENDERBOX) {
							tmp = rx_field[2] - 1;
							if (!(channel_fired[tmp]) && !flags.b.fire) {
								flags.b.fire = 1;
								loopcount = rx_field[3];
							}
						}
						break;
					}

						// Received temperature-measurement-trigger
					case TEMPERATURE: {
						tempsenstype = tempident();
						temperature = tempmeas(tempsenstype);
						break;
					}

						// Received identification-demand
					case IDENT: {
						tx_field[0] = PARAMETERS;
						tx_field[1] = unique_id;
						tx_field[2] = slave_id;
						tx_field[3] = (SENDERBOX ? 50 : adc_read(5));
						tx_field[4] = (SENDERBOX ? 0 : armed);
						tx_field[5] = temperature;
						tx_length = 6;

						transmission_allowed = 0;
						timer1_reset();
						transmit_flag = 0;
						TIMSK1 |= (1 << TOIE1); 					// Enable timer interrupt
						timer1_on();
						flags.b.transmit = 1;
						flags.b.reset_fired = 1;

						// Ignition devices have to write themselves in the list
						if (!SENDERBOX) {
							boxes[unique_id - 1] = slave_id;
							quantity[slave_id - 1] = 1;
							batteries[unique_id - 1] = adc_read(5);
							sharpness[unique_id - 1] = (armed ? 'j' : 'n');
							temps[unique_id - 1] = temperature;
							rssis[unique_id - 1] = 0;
						}

						break;
					}

						// Received Parameters
					case PARAMETERS: {
						if ((rx_field[1] == 'E') || (!rx_field[1]) || (rx_field[1] == unique_id)) {
							iderrors++;
						}
						else {
							tmp = rx_field[1] - 1; 				// Index = unique_id-1 (zero-based indexing)
							boxes[tmp] = rx_field[2];
							batteries[tmp] = rx_field[3];
							sharpness[tmp] = (rx_field[4] ? 'j' : 'n');
							temps[tmp] = rx_field[5];
							rssis[tmp] = rssi;
						}
						break;
					}

						// Received change command
					case CHANGE: {
						if ((unique_id == rx_field[1]) && (slave_id == rx_field[2])) {
							rem_uid = rx_field[3];
							rem_sid = rx_field[4];
							if (((rem_uid > 0) && (rem_uid < 31)) && ((rem_sid > 0) && (rem_sid < 31))
									&& ((rem_uid != unique_id) || (rem_sid != slave_id))) {
								savenumber(rem_uid, rem_sid);
								flags.b.reset_device = 1;
							}
						}
						break;
					}

						// Default action (do nothing)
					default: {
						break;
					}
				}
				flags.b.lcd_update = 1;
				flags.b.rx_post = 1;
			}
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Clear LCD in case of timeouts
		if (SENDERBOX
				&& (clear_lcd_tx_flag > DEL_THRES || clear_lcd_rx_flag > DEL_THRES
						|| hist_del_flag > (3 * DEL_THRES)) && !flags.b.lcd_update) {
			temp_sreg = SREG; // Speichere Statusregister
			cli();

			if (clear_lcd_tx_flag > DEL_THRES) {
				clear_lcd_tx_flag = 0;
				lcd_cursorset(1, 1);
				for (i = 0; i < 20; i++) {
					lcd_puts(" ");
				}
			}

			if (clear_lcd_rx_flag > DEL_THRES) {
				clear_lcd_rx_flag = 0;
				lcd_cursorset(2, 1);
				for (i = 0; i < 20; i++) {
					lcd_puts(" ");
				}
			}

			if (hist_del_flag > (DEL_THRES * 3)) {
				hist_del_flag = 0;
				lcd_cursorset(3, 1);
				for (i = 0; i < 20; i++) {
					lcd_puts("  ");
				}
				anzzeile = 3;
				anzspalte = 1;
			}

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Refresh LCD
		if (SENDERBOX && flags.b.lcd_update) {
			temp_sreg = SREG;
			cli();

			flags.b.lcd_update = 0;

			// TRANSMITTER (1. Line + 3./4. Line)
			if (flags.b.tx_post) {
				lcd_cursorset(1, 1);
				lcd_puts("Tx: ");
				switch (tx_field[0]) {
					case FIRE: {
						lcd_send(0, 1);
						lcd_puts(" S");
						lcd_arrize(tx_field[1], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" CH");
						lcd_arrize(tx_field[2], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts("      ");

						if (!flags.b.show_only) {
							lcd_cursorset(lastzeile, lastspalte);
							lcd_puts(" ");
							lcd_cursorset(anzzeile, anzspalte);
							lcd_puts("x");
							lcd_arrize(tx_field[1], lcd_array, 2, 0);
							lcd_puts(lcd_array);
							lcd_send(0, 1);
							lcd_arrize(tx_field[2], lcd_array, 2, 0);
							lcd_puts(lcd_array);

							cursor_x_shift(&lastzeile, &lastspalte, &anzzeile, &anzspalte);
							hist_del_flag = 1;
						}
						flags.b.show_only = 0;
						break;
					}
					case IDENT: {
						lcd_puts("Identify        ");

						if (!flags.b.show_only) {
							lcd_cursorset(lastzeile, lastspalte);
							lcd_puts(" ");
							lcd_cursorset(anzzeile, anzspalte);
							lcd_puts("xIDENT");

							cursor_x_shift(&lastzeile, &lastspalte, &anzzeile, &anzspalte);
							hist_del_flag = 1;
						}
						flags.b.show_only = 0;
						break;
					}
					case ACKNOWLEDGED: {
						lcd_puts("OK              ");
						break;
					}
					case PARAMETERS: {
						lcd_puts("U"); 										// Unique-ID
						lcd_arrize(tx_field[1], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" S"); 									// Salve-ID
						lcd_arrize(tx_field[2], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" ");
						if (tx_field[3] < 100) lcd_puts(" ");
						lcd_arrize((tx_field[3] / 10), lcd_array, 1, 0); 	// Battery voltage
						lcd_puts(lcd_array);
						lcd_puts(".");
						lcd_arrize((tx_field[3] % 10), lcd_array, 1, 0);
						lcd_puts(lcd_array);
						lcd_puts("V ");
						lcd_send(tx_field[4] ? 'j' : 'n', 1); // Armed?
						lcd_puts("  ");
						break;
					}
					case REPEAT: {
						lcd_puts("Repeat          ");
						break;
					}
					case TEMPERATURE: {
						lcd_puts("Temperature     ");

						if (!flags.b.show_only) {
							lcd_cursorset(lastzeile, lastspalte);
							lcd_puts(" ");
							lcd_cursorset(anzzeile, anzspalte);
							lcd_puts("xTEMP ");

							cursor_x_shift(&lastzeile, &lastspalte, &anzzeile, &anzspalte);
							hist_del_flag = 1;
						}
						flags.b.show_only = 0;

						break;
					}
					case CHANGE: {
						lcd_puts("U"); 								// Old Unique-ID
						lcd_arrize(tx_field[1], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" S"); 							// Old Slave-ID
						lcd_arrize(tx_field[2], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts("->U");							// New Unique-ID
						lcd_arrize(tx_field[3], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" S"); 							// New Slave-ID
						lcd_arrize(tx_field[4], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						break;
					}
					default:
						break;
				}
				flags.b.tx_post = 0;
				clear_lcd_tx_flag = 1;
			}

			// RECEIVER (2. Line)
			if (flags.b.rx_post) {
				lcd_cursorset(2, 1);
				lcd_puts("Rx: ");
				switch (rx_field[0]) {
					case ACKNOWLEDGED: {
						lcd_puts("OK ");
						break;
					}
					case FIRE: {
						lcd_send(0, 1);
						lcd_puts(" S");
						lcd_arrize(rx_field[1], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" CH");
						lcd_arrize(rx_field[2], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts("      ");
						break;
					}
					case IDENT: {
						lcd_puts("Identify        ");
						break;
					}
					case ERROR: {
						lcd_puts("ERR             ");
						break;
					}
					case REPEAT: {
						lcd_puts("REP             ");
						break;
					}
					case PARAMETERS: {
						lcd_puts("U");
						lcd_arrize(rx_field[1], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" S");
						lcd_arrize(rx_field[2], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" ");
						if (rx_field[3] < 100) lcd_puts(" ");
						lcd_arrize(rx_field[3] / 10, lcd_array, 1, 0);
						lcd_puts(lcd_array);
						lcd_puts(".");
						lcd_arrize(rx_field[3] % 10, lcd_array, 1, 0);
						lcd_puts(lcd_array);
						lcd_puts("V ");
						lcd_send(rx_field[4] ? 'j' : 'n', 1);
						lcd_puts("  ");
						break;
					}

					case CHANGE: {
						lcd_puts("U"); 								// Old Unique-ID
						lcd_arrize(rx_field[1], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" S"); 							// Old Slave-ID
						lcd_arrize(rx_field[2], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts("->U");							// New Unique-ID
						lcd_arrize(rx_field[3], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						lcd_puts(" S"); 							// New Slave-ID
						lcd_arrize(rx_field[4], lcd_array, 2, 0);
						lcd_puts(lcd_array);
						break;
					}
					default:
						break;
				}
				flags.b.rx_post = 0;
				clear_lcd_rx_flag = 1;
			}
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------
	}
	return 0;
}

// Interrupt vectors
ISR(TIMER1_OVF_vect) {
	transmit_flag++;
}

ISR(TIMER0_OVF_vect) {
	if (clear_lcd_tx_flag) clear_lcd_tx_flag++;
	if (clear_lcd_rx_flag) clear_lcd_rx_flag++;
	if (hist_del_flag) hist_del_flag++;
	if (clear_lcd_tx_flag > 1000) clear_lcd_tx_flag = 0;
	if (clear_lcd_rx_flag > 1000) clear_lcd_rx_flag = 0;
	if (hist_del_flag > 10000) hist_del_flag = 0;
}

ISR(KEYINT) {
	key_flag = 1;
}
