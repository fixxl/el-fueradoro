/*
 * Steuerprogramm Funkzündanlage
 *         EL FUERADORO
 *
 *         Felix Pflaum
 *
 */

#include "global.h"

// Globale Variablen
volatile uint8_t transmit_flag = 0, key_flag = 0;
volatile uint16_t clear_lcd_tx_flag = 0, clear_lcd_rx_flag = 0, hist_del_flag = 0;

void wdt_init(void) {
	MCUSR = 0;
	wdt_disable();
	return;
}

// Schlüsselschalter initialisieren
void key_init(void) {
	KEYPORT |= (1 << KEY);
	KEYDDR &= ~(1 << KEY);

	// Pin-Change Interrupt aktivieren
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

// Schlüsselschalter de-initialisieren
void key_deinit(void) {
	KEYPORT &= ~(1 << KEY);
	KEYDDR &= ~(1 << KEY);

	// Pin-Change Interrupt deaktivieren
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

	// Timer 0 zurücksetzen
	TCNT0 = 0;
	TIFR0 = (1 << TOV0);
	TCNT0 = 160;
	TCCR0B = (1 << CS02 | 1 << CS00);

	// Schalterzustand solange abfragen, bis er 8-mal hintereinander gleich ist
	// Abfrage erfolgt alle 10 ms
	while (((keystate != 0x00) && (keystate != 0xFF)) || (ctr < 8)) {
		keystate <<= 1;
		keystate |= ((*port & (1 << pin)) > 0);
		while (!(TIFR0 & (1 << TOV0)))
			;
		TIFR0 = (1 << TOV0);
		TCNT0 = 160;
		ctr++;
	}
	TCCR0B = timer0_regb;

	// 1 zurückgeben wenn Schalter geschlossen, 0 wenn nicht
	return (keystate == 0);
}

// Symbole erzeugen
void create_symbols(void) {
	// Auf CGRAM wechseln
	lcd_send(1 << 6, 0);

	// Feuersymbol erzeugen
	lcd_cgwrite(4);
	lcd_cgwrite(4);
	lcd_cgwrite(10);
	lcd_cgwrite(10);
	lcd_cgwrite(21);
	lcd_cgwrite(17);
	lcd_cgwrite(10);
	lcd_cgwrite(4);

	// ° erzeugen
	lcd_cgwrite(8);
	lcd_cgwrite(20);
	lcd_cgwrite(8);
	lcd_cgwrite(0);
	lcd_cgwrite(0);
	lcd_cgwrite(0);
	lcd_cgwrite(0);
	lcd_cgwrite(0);

	// Auf DDRAM zurückschalten
	lcd_send(1 << 7, 0);
}

// ------------------------------------------------------------------------------------------------------------------------

// Temperaturmessung
uint8_t tempident(void) {
	int16_t var1 = 0, var2 = 0;
	uint8_t checkup = dht_read(&var1, &var2);

	switch (checkup) {
	case 0: {
		return DHT22;
		break;
	}
	case 5: {
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

int8_t tempmeas(uint8_t type) {
	int16_t temperature = -128;
	uint16_t temp_hex;
	uint8_t tempfail = 1;

	switch (type) {
	case DS18B20: {
		tempfail = w1_reset();

		if (!tempfail) {
			w1_command(CONVERT_T, NULL);
			while (!w1_bit_io(1))
				;
			w1_command(READ, NULL);
			temp_hex = w1_byte_rd();
			temp_hex += (w1_byte_rd()) << 8;
			temperature = w1_tempread_to_celsius(temp_hex);
		}
		break;
	}

	case DHT22: {
		int16_t humidity;
		tempfail = dht_read(&temperature, &humidity);
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

uint8_t uart_valid(const char *field) {
	return ((field[0] == 0xFF) && (field[1] > 0) && (field[1] < 31) && (field[2] > 0) && (field[2] < 17)
			&& (field[3] == crc8(crc8(0, field[1]), field[2])));
}

// ------------------------------------------------------------------------------------------------------------------------

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

// Hauptprogramm
int main(void) {

	wdt_disable();

	// Variablendefinitionen
	uint16_t scheme = 0;
	uint8_t i, nr, inp, tmp, uart_char;
	uint8_t tx_length = 2, rx_length = 0;
	uint8_t temp_sreg;
	uint8_t slave_id = 30, unique_id = 30;
	uint8_t success = 0, loopcount = 5;
	uint8_t anzspalte = 1, anzzeile = 3, lastspalte = 15, lastzeile = 4;
	uint8_t armed = 0;
	uint8_t changes = 0;
	uint8_t iderrors = 0;
	uint8_t tempsenstype = 0;
	uint8_t rssi = 0;
	int8_t temperature = -128;

	bitfeld_t flags;
	flags.complete = 0;

	char uart_field[MAX_ARRAYSIZE + 1] = { 0 };
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

	/* Initialisierung der Schieberegister. Erfolgt aus Sicherheitsgründen ganz am Anfang, um in jedem Fall
	 * als Anfangszustand am Gate der MOSFETs "low" zu erhalten und ein Auslösen beim Einschalten zu verhindern.
	 * Die verwendeten SIPO-Schieberegister 74HC595, welche die 16 Ausgänge zur Verfügung stellen, besitzen intern ein
	 * Schieberegister (gesteuert durch SCLOCK) sowie ein Ausgangsregister, in welches die Schieberegisterdaten durch eine
	 * steigende Flanke an RCLOCK übernommen werden. Anders gesagt: Veränderungen im Schieberegister werden erst durch
	 * eine steigende Flanke an RCLOCK an den Ausgängen sichtbar.
	 *
	 * Sobald der Pin /OE (Inverted output enable) auf "low"-Level liegt, sind die Ausgänge der Schieberegister aktiv und
	 * nehmen beim Einschalten nach Belieben low- oder high-Pegel an, ohne dass dies vom Anwender kontrolliert werden kann!
	 * Zwar existiert ein "Master Reset"-Pin, dieser wirkt aber lediglich auf das Schieberegister und beeinflusst die
	 * Ausgänge und deren Einschaltzustand nicht.
	 *
	 * Einzige Möglichkeit einen definierten Einschaltzustand zu gewährleisten ist daher, den /OE-Pin so lange
	 * auf "high"-Pegel zu halten, bis die gewünschten Werte für alle 16 Kanäle ins Schieberegister geschrieben und per RCLOCK
	 * an die Ausgänge übergeben wurden.
	 *
	 * Durch externe Beschaltung (Pullup an /OE-Pin) sind die Ausgänge zu Beginn inaktiv (Tri-State) und werden
	 * durch Widerstandsarrays auf "low"-Pegel gehalten. Es müssen, um Zündungen beim Einschalten zu vermeiden, UNBEDINGT
	 * zunächst nacheinander 16 Nullen in die beiden Schieberegister geschrieben und deren Inhalt durch steigende Flanke
	 * an RCLOCK in die Ausgänge übernommen werden (Prozedur gerne nach Belieben wiederholen), damit alle MOSFETs bei
	 * Aktivierung der Ausgänge sperren. Erst dann können die Ausgänge durch das Setzen von /OE auf "low" gefahrlos
	 * aktiviert werden!
	 *
	 * Es ist daher empfohlen, keine Änderungen an der Routine sr_init() vorzunehmen und sie im Hauptprogramm  als erste
	 * Tätigkeit direkt nach der Deklaration der Variablen aufzurufen! Hierfür muss noch keine Unterscheidung
	 * Transmitter/Zündbox getroffen sein. Zwar ist die Routine beim Transmitter wirkungslos, richtet aber auch keinen
	 * Schaden an.
	 */
	sr_init();

	// Nicht verwendete Funktionen abschalten (2-wire-Interface, Timer 2, Analoger Komparator)
	PRR |= (1 << PRTWI) | (1 << PRTIM2);
	ACSR |= 1 << ACD;

	// Timer initialisieren, um Timeouts zu erkennen
	timer_on();

	// Aktions-LEDs initialisieren
	led_init();

	// Felder initialisieren
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

	// Slave- und Unique-ID aus EEPROM holen
	update_addresses(&unique_id, &slave_id);

	// UART initialisieren und empfangsbereit schalten
	uart_init(BAUD);
	allow_uart_sending();

	// Temperatur messen
	tempsenstype = tempident();
	temperature = tempmeas(tempsenstype);

	// Funkmodul initialisieren
	rfm_init();

	if (SENDERBOX) {
		tx_length = 3;
		tx_field[0] = IDENT;
		tx_field[1] = 'd';
		tx_field[2] = '0';
		flags.b.transmit = 1;
		TIMSK0 |= (1 << TOIE0);
	} else {
		tx_field[0] = PARAMETERS;
		tx_field[1] = slave_id;
		tx_field[2] = unique_id;
		tx_field[3] = adc_read(5);
		armed = debounce(&KEYPIN, KEY);
		if (armed) led_yellow_on();
		else led_yellow_off();
		tx_field[4] = armed;
		tx_field[5] = tempmeas(tempsenstype);
		tx_length = 6;
	}
	flags.b.transmit = 1;

	// Interrupts erlauben
	sei();

	// Programm-Hauptschleife
	/*
	 * In der Hauptschleife wird permanent der Zustand der einzelnen Flags überprüft und dann die entspechende
	 * Routine ausgeführt. Zu Beginn der Routinen wird das Statusregister zwischengespeichert, Interrupts deaktiviert
	 * und das entspechende Flag gelöscht, am Ende das Statusregister wieder zurückgeschrieben
	 */
	while (1) {

// -------------------------------------------------------------------------------------------------------

// Schlüsselschalter überprüfen (key_flag wird durch Interrupt bei Erkennen eines Pegelwechsels gesetzt)
		if (key_flag) {
			temp_sreg = SREG;
			cli();
			key_flag = 0;

			// Box scharf: armed = 1, Box nicht scharf: armed = 0

			armed = debounce(&KEYPIN, KEY);
			if (armed) led_yellow_on();
			else led_yellow_off();
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// UART-Routine
		flags.b.uart_active = ((UCSR0A & (1 << RXC0)) ? 1 : 0);
		if (flags.b.uart_active) {
			temp_sreg = SREG;
			cli();
			flags.b.uart_active = 0;

			led_yellow_on();

			// Receive first char
			uart_char = uart_getc();

			// React according to first char (ignition command or not?)
			if (uart_char == 0xFF) {
				uart_field[0] = uart_char;
				uart_field[1] = uart_getc();
				uart_field[2] = uart_getc();
				uart_field[3] = uart_getc();
			} else {
				uart_gets(uart_field);
				tmp = 0;
				while (uart_field[tmp] && (tmp < MAX_ARRAYSIZE - 1)) {
					tmp++;
				}
				for (uint8_t i = tmp; i; i--) {
					uart_field[i] = uart_field[i - 1];
				}
				uart_field[0] = uart_char;
			}

			// Verschiedene Eingaben auswerten
			// "conf" für Konfigurationsprogramm
			if (uart_strings_equal(uart_field, "conf")) {
				flags.b.uart_config = 1;
				flags.b.transmit = 0;
			}

			// "clearlist" um Boxenliste zu leeren
			if (uart_strings_equal(uart_field, "clearlist") && SENDERBOX) {
				flags.b.clear_list = 1;
			}

			// "send" für manuelles Senden
			if (uart_strings_equal(uart_field, "send") && SENDERBOX) {
				flags.b.send = 1;
				flags.b.transmit = 0;
			}

			// "list" für Auflistung der Zündboxen
			if (uart_strings_equal(uart_field, "list") && SENDERBOX) {
				flags.b.list = 1;
				flags.b.transmit = 0;
			}

			// "orders", um letzte gesendete und empfangene Pattern auf LCD auszugeben
			if (uart_strings_equal(uart_field, "orders") && SENDERBOX) {
				flags.b.rx_post = 1;
				flags.b.tx_post = 1;
				flags.b.show_only = 1;
				flags.b.lcd_update = 1;
			}

			// "zero", um alle Kanäle als nicht abgefeuert zu kennzeichnen
			if (uart_strings_equal(uart_field, "zero") && !SENDERBOX) {
				flags.b.reset_fired = 1;
			}

			// "cls", um Terminalausgabe zu löschen
			if (uart_strings_equal(uart_field, "cls")) {
				terminal_reset();
			}

			// "kill", um den Controller zu resetten
			if (uart_strings_equal(uart_field, "kill")) {
				flags.b.reset_device = 1;
			}

			// "temp", um Temperatur zu messen
			if (uart_strings_equal(uart_field, "temp")) {
				flags.b.temp = 1;
			}

			// "int1" führt zu periodischem Senden des letzten gesendeten Befehls
			if (uart_strings_equal(uart_field, "int1") && SENDERBOX) {
				uart_puts_P(PSTR("\n\n\rWiederholtes Senden des letzten Befehls EIN\n\r"));
				TIMSK1 |= (1 << TOIE1);
			}

			// "int0" schaltet periodisches Senden des letzten Befehls ab
			if (uart_strings_equal(uart_field, "int0") && SENDERBOX) {
				uart_puts_P(PSTR("\n\n\rWiederholtes Senden des letzten Befehls AUS\n\r"));
				transmit_flag = 0;
				TIMSK1 &= ~(1 << TOIE1);
			}

			// Beschreiben des Senderegisters, wenn gültiger Zündbefehl empfangen wurde
			if (uart_valid(uart_field)) {
				tx_field[0] = FIRE;
				tx_field[1] = uart_field[1];
				tx_field[2] = uart_field[2];
				tx_field[3] = FIREREPEATS;
				tx_length = 4;
				flags.b.transmit = 1;
			}

			led_yellow_off();
			if (uart_field[0] != 0xFF) uart_puts_P(PSTR("\n\r"));

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Temperaturmessung
		if (flags.b.temp) {
			temp_sreg = SREG;
			cli();
			flags.b.temp = 0;

			temperature = tempmeas(tempsenstype);

			uart_puts_P(PSTR("Temperatur: "));
			if (temperature == -128) {
				uart_puts_P(PSTR("n.a."));
			} else {
				fixedspace(temperature, 'd', 4);
				uart_puts_P(PSTR("°C"));
			}
			uart_puts_P(PSTR("\n\r\n\r"));

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Einstellungen zu Slave- und Unique-ID
		if (flags.b.uart_config) {
			temp_sreg = SREG;
			cli();
			flags.b.uart_config = 0;

			timer_reset();
			timer_off();

			changes = configprog();
			if (changes) flags.b.reset_device = 1;
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Feuern
		if (flags.b.fire) {
			temp_sreg = SREG;
			cli();
			flags.b.fire = 0;

			for (i = loopcount - 1; i > 0; i--) {
				_delay_ms(11);
			}

			if (rx_field[2] > 0 && rx_field[2] < 17 && armed) {
				tmp = rx_field[2];
				scheme = 0;
				for (uint8_t i = 16; i > 0; i--) {
					scheme <<= 1;
					if (i == tmp) scheme |= 1;
				}
				sr_shiftout(scheme);

				/* Um im Falle einen Kurzschlusses nach der Zündung den MOSFET nicht zu beschädigen/zerstören
				 * wird der Kanal nach 11ms automatisch wieder gesperrt. Gemäß Spezifikation müssen die Zünder
				 * bis dahin ausgelöst haben
				 */
				_delay_ms(11);
				sr_shiftout(0);
				channel_fired[rx_field[2] - 1] = 1;
			}
			rfm_rxon();
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Geschossene Kanäle zurücksetzen
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

// Manuelles Senden
		if (flags.b.send) {
			temp_sreg = SREG;
			cli();

			flags.b.send = 0;

			nr = 0;
			tmp = 0;

			tx_length = 3;
			tx_field[0] = IDENT;
			tx_field[1] = 'd';
			tx_field[2] = '0';

			timer_reset();
			timer_off();

			uart_puts_P(PSTR("\n\n\rModus(f/i): "));
			inp = uart_getc();
			uart_putc(inp);
			tx_field[0] = inp;

			if (tx_field[0] == FIRE || tx_field[0] == IDENT) tmp = 1;

			if (tx_field[0] == FIRE) {
				uart_puts_P(PSTR("\n\rSlave-ID:   "));
				for (i = 0; i < 2; i++) {
					inp = uart_getc();
					uart_putc(inp);
					nr += (inp - '0');
					if (!i) nr *= 10;
				}
				uart_puts_P(PSTR(" = "));
				if (nr > 0 && nr < 31) {
					uart_shownum(nr, 'd');
					tx_field[1] = nr;
				} else {
					uart_puts_P(PSTR("Ungültige Eingabe"));
					tmp = 0;
				}
				nr = 0;
				uart_puts_P(PSTR("\n\rKanal:      "));
				for (i = 0; i < 2; i++) {
					inp = uart_getc();
					uart_putc(inp);
					nr += (inp - '0');
					if (!i) nr *= 10;
				}
				uart_puts_P(PSTR(" = "));
				if (nr > 0 && nr < 31) {
					uart_shownum(nr, 'd');
					tx_field[2] = nr;
					tx_field[3] = FIREREPEATS;
					tx_length = 4;
				} else {
					uart_puts_P(PSTR("Ungültige Eingabe"));
					tmp = 0;
				}
			}

			uart_puts_P(PSTR("\n\n\r"));
			if (tmp) flags.b.transmit = 1;
			else flags.b.transmit = 0;

			while (UCSR0A & (1 << RXC0))
				inp = UDR0;

			timer_on();
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Auflisten
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

// Reset auslösen
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

// Vor Identifikationsaufforderung Liste löschen
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

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Senderoutine
		if (flags.b.transmit || transmit_flag > 3) { // Wenn Befehl gesendet werden soll...
			temp_sreg = SREG; // Speichere Statusregister
			cli();

			flags.b.transmit = 0; // Lösche Sendeaufforderung
			transmit_flag = 0;

			led_green_on();
			rfm_transmit(tx_field, tx_length); // Übertrage Nachricht
			if (tx_field[0] == FIRE) while (--tx_field[3])
				rfm_transmit(tx_field, tx_length);
			led_green_off();

			flags.b.lcd_update = 1;
			flags.b.tx_post = 1;

			SREG = temp_sreg; // Stelle Statusregisterzustand wieder her
		}

// -------------------------------------------------------------------------------------------------------

// Empfangsroutine
		flags.b.receive = (rfm_receiving() ? 1 : 0);
		if (flags.b.receive) { // Wenn Byte/Paket angekommen
			temp_sreg = SREG; // Speichere Statusregister
			cli();

			flags.b.receive = 0;

			led_blue_on();
#ifdef RFM69_H_
			rssi = rfm_get_rssi_dbm(); // Senderbox misst Signalstärke
#endif
			success = rfm_receive(rx_field, &rx_length); // Hole Nachricht ab
			led_blue_off();

			if (!success) rx_field[0] = ERROR; // Werte aus
			else {
				switch (rx_field[0]) { // Weiteres Vorgehen anhand der Art der Nachricht

				// Zündbefehl erhalten (nur für Zündboxen relevant)
				case FIRE: {
					if ((rx_field[1] == slave_id) && !SENDERBOX) {
						tmp = rx_field[2] - 1;
						if (!(channel_fired[tmp]) && !flags.b.fire) {
							flags.b.fire = 1;
							rfm_rxoff();
							loopcount = rx_field[3];
						}
					}
					break;
				}

					// Aufforderung zur Identifizierung erhalten (nur für Zündboxen relevant)
				case IDENT: {
					rfm_rxoff();
					tx_field[0] = PARAMETERS;
					tx_field[1] = slave_id;
					tx_field[2] = unique_id;
					tx_field[3] = (SENDERBOX ? 50 : adc_read(5));
					tx_field[4] = (SENDERBOX ? 0 : armed);
					tx_field[5] = tempmeas(tempsenstype);
					tx_length = 6;

					flags.b.transmit = 1;
					flags.b.reset_fired = 1;
					break;
				}

					// Parameter erhalten (nur für Senderbox relevant)
				case PARAMETERS: {
					if (SENDERBOX) {
						if (rx_field[2] == 'E' || !rx_field[2]) {
							iderrors++;
						} else {
							tmp = rx_field[2] - 1; // Index = unique_id-1 (nullbasiertes Indexing)
							boxes[tmp] = rx_field[1];
							batteries[tmp] = rx_field[3];
							sharpness[tmp] = (rx_field[4] ? 'j' : 'n');
							temps[tmp] = rx_field[5];
							rssis[tmp] = rssi;
						}
					}
					break;
				}

					// Standardaktion
				default: {
					break;
				}
				}

				if (flags.b.transmit) {
					for (i = 0; i < unique_id; i++) {
						_delay_ms(25);
					}
				} else {
					rfm_rxon();
				}
				flags.b.lcd_update = 1;
				flags.b.rx_post = 1;
			}
			SREG = temp_sreg; // Stelle Statusregisterzustand wieder her
		}

// -------------------------------------------------------------------------------------------------------

// LCD löschen, falls Timeoutzeiten überschritten
		if ((clear_lcd_tx_flag > DEL_THRES || clear_lcd_tx_flag > DEL_THRES) && SENDERBOX) {
			temp_sreg = SREG; // Speichere Statusregister
			cli();

			if (lcd_cursorread() && (clear_lcd_tx_flag > DEL_THRES)) {
				clear_lcd_tx_flag = 0;
				lcd_cursorset(1, 1);
				for (i = 0; i < 20; i++) {
					lcd_puts(" ");
				}
			}

			if (lcd_cursorread() && (clear_lcd_rx_flag > DEL_THRES)) {
				clear_lcd_rx_flag = 0;
				lcd_cursorset(2, 1);
				for (i = 0; i < 20; i++) {
					lcd_puts(" ");
				}
			}

			if (hist_del_flag > (DEL_THRES * 3)) {
				lcd_clear();
				anzzeile = 3;
				anzspalte = 1;
			}

			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

// Anzeige aktualisieren
		if (SENDERBOX && flags.b.lcd_update) {
			temp_sreg = SREG; // Speichere Statusregister
			cli();

			flags.b.lcd_update = 0;

			// SENDER (1. Zeile + 3./4. Zeile)
			lcd_cursorset(1, 1);
			lcd_puts("Tx: ");
			if (flags.b.tx_post) {
				switch (tx_field[0]) {
				case FIRE: {
					lcd_send(0, 1);
					lcd_puts(" S");
					lcd_arrize(tx_field[1], lcd_array, 2, 0);
					lcd_puts(lcd_array);
					lcd_puts(" CH");
					lcd_arrize(tx_field[2], lcd_array, 2, 0);
					lcd_puts(lcd_array);
					flags.b.tx_post = 0;

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
						hist_del_flag = 0;
					}
					flags.b.show_only = 0;
					break;
				}
				case IDENT: {
					lcd_puts("Identify  ");
					flags.b.tx_post = 0;

					if (!flags.b.show_only) {
						lcd_cursorset(lastzeile, lastspalte);
						lcd_puts(" ");
						lcd_cursorset(anzzeile, anzspalte);
						lcd_puts("x");
						lcd_puts("IDENT");

						cursor_x_shift(&lastzeile, &lastspalte, &anzzeile, &anzspalte);
						hist_del_flag = 0;
					}
					flags.b.show_only = 0;
					break;
				}
				case ACKNOWLEDGED: {
					lcd_puts("OK");
					flags.b.tx_post = 0;
					break;
				}
				case PARAMETERS: {
					lcd_puts("S");									// Slave-ID
					lcd_arrize(tx_field[1], lcd_array, 2, 0);
					lcd_puts(lcd_array);
					lcd_puts(" U");									// Unique-ID
					lcd_arrize(tx_field[2], lcd_array, 2, 0);
					lcd_puts(lcd_array);
					lcd_puts(" ");
					lcd_arrize(tx_field[3] / 10, lcd_array, 1, 0);	// Batteriespannung
					lcd_puts(lcd_array);
					lcd_puts(".");
					lcd_arrize(tx_field[3] % 10, lcd_array, 1, 0);
					lcd_puts(lcd_array);
					lcd_puts("V ");
					lcd_send(tx_field[4] ? 'j' : 'n', 1);			// Scharf geschaltet?

					flags.b.tx_post = 0;
					break;
				}
				case REPEAT: {
					lcd_puts("REPEAT");
					flags.b.tx_post = 0;
					break;
				}
				default:
					break;
				}
				clear_lcd_tx_flag = 0;
			}

			// EMPFÄNGER (2. Zeile)
			lcd_cursorset(2, 1);
			lcd_puts("Rx: ");
			if (flags.b.rx_post) {
				switch (rx_field[0]) {
				case ACKNOWLEDGED: {
					lcd_puts("OK ");
					flags.b.rx_post = 0;
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
					flags.b.rx_post = 0;
					break;
				}
				case IDENT: {
					lcd_puts("Identify");
					flags.b.rx_post = 0;
					break;
				}
				case ERROR: {
					lcd_puts("ERR");
					flags.b.rx_post = 0;
					break;
				}
				case REPEAT: {
					lcd_puts("REP");
					flags.b.rx_post = 0;
					break;
				}
				case PARAMETERS: {
					lcd_puts("S");
					lcd_arrize(rx_field[1], lcd_array, 2, 0);
					lcd_puts(lcd_array);
					lcd_puts(" U");
					lcd_arrize(rx_field[2], lcd_array, 2, 0);
					lcd_puts(lcd_array);
					lcd_puts(" ");
					if (rx_field[2] < 100) lcd_puts(" ");
					lcd_arrize(rx_field[3] / 10, lcd_array, 1, 0);
					lcd_puts(lcd_array);
					lcd_puts(".");
					lcd_arrize(rx_field[3] % 10, lcd_array, 1, 0);
					lcd_puts(lcd_array);
					lcd_puts("V ");
					lcd_send(rx_field[4] ? 'j' : 'n', 1);

					flags.b.rx_post = 0;
					break;
				}
				default:
					break;
				}
				clear_lcd_rx_flag = 0;
			}
			SREG = temp_sreg;
		}

// -------------------------------------------------------------------------------------------------------

	}
	return 0;
}

// Interruptroutinen
ISR(TIMER1_OVF_vect) {
	transmit_flag++;
}

ISR(TIMER0_OVF_vect) {
	clear_lcd_tx_flag++;
	clear_lcd_rx_flag++;
	hist_del_flag++;
	if (clear_lcd_tx_flag > 1000) clear_lcd_tx_flag = 0;
	if (clear_lcd_rx_flag > 1000) clear_lcd_rx_flag = 0;
	if (hist_del_flag > 10000) hist_del_flag = 0;
}

ISR(KEYINT) {
	key_flag = 1;
}
