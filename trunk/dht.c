/*
 * dht.c
 *
 *  Created on: 14.06.2013
 *      Author: Felix
 */

#include "global.h"

#ifdef DHT_H_

uint8_t dht_read(int16_t* temperatur_mal_zehn, int16_t* feuchtigkeit_mal_zehn) {
	uint8_t ergebnisfeld[6] = { 0, 0, 0, 0, 0x80, '\0' };
	uint8_t cycles = (DHT_TYPE > 15) ? 1 : 10, temperature_sign;
	uint8_t chksm;
	uint8_t timeoutcounter = 0, errorcounter = 1;
	int16_t humid;
	int16_t temperat;

	TEMPHUMDDR &= ~(1 << TEMPHUM);
	TEMPHUMPORT &= ~(1 << TEMPHUM);
	_delay_us(50);
	if (!(TEMPHUMPIN & (1 << TEMPHUM))) return errorcounter;
	errorcounter++;
	timeoutcounter = 0;

	// DATAPORT low setzen und bauteilspezifisch warten
	TEMPHUMDDR |= (1 << TEMPHUM);
	for (uint8_t i = 0; i < cycles; i++) {
		_delay_ms(2);
	}
	TEMPHUMDDR &= ~(1 << TEMPHUM); // DATAPORT zu Eingang machen (wird durch ext. Pullup high gezogen)
	timeoutcounter = 0;

	_delay_us(10);

	// High-Pegel abwarten (20-200us)
	while ((TEMPHUMPIN & (1 << TEMPHUM)) && timeoutcounter < 30) {
		_delay_us(10);
		timeoutcounter++;
	}
	if (timeoutcounter > 24) return errorcounter;
	errorcounter++;
	timeoutcounter = 0;

	// Low-Pegel abwarten (80us lt. Datenblatt)
	while (!(TEMPHUMPIN & (1 << TEMPHUM)) && timeoutcounter < 25) {
		_delay_us(10);
		timeoutcounter++;
	}
	if (timeoutcounter > 24) return errorcounter;
	errorcounter++;
	timeoutcounter = 0;

	// High-Pegel abwarten (80us lt. Datenblatt)
	while ((TEMPHUMPIN & (1 << TEMPHUM)) && timeoutcounter < 25) {
		_delay_us(10);
		timeoutcounter++;
	}
	if (timeoutcounter > 24) return errorcounter;
	errorcounter = 100;
	timeoutcounter = 0;

	// 40 Datenbits (0...39) auswerten
	for (uint8_t byte = 0; byte < 5; byte++) {
		for (uint8_t bitcntr = 8; bitcntr; bitcntr--) {
			timeoutcounter = 50;

			// Low-Pegel abwarten (50us lt. Datenblatt)
			while ((!(TEMPHUMPIN & (1 << TEMPHUM))) && timeoutcounter--) {
				_delay_us(5);
			}
			ergebnisfeld[byte] <<= 1;

			// Nach 30us den Pinzustand abfragen (High = 1, Low = 0)
			_delay_us(30);
			if ((TEMPHUMPIN & (1 << TEMPHUM))) ergebnisfeld[byte] |= 1;

			led_blue_toggle();

			// Evtl. restlichen High-Pegel abwarten
			timeoutcounter = 50;
			while ( (TEMPHUMPIN & (1 << TEMPHUM)) && timeoutcounter--) {
				_delay_us(5);
			}
		}
	}

	errorcounter = 200;
	chksm = (ergebnisfeld[0] + ergebnisfeld[1] + ergebnisfeld[2] + ergebnisfeld[3]) & 0x00FF;

	if (ergebnisfeld[4] != chksm) return errorcounter;
	errorcounter++;

	switch (DHT_TYPE > 15) {
		case 0: {
			humid = (ergebnisfeld[0] << 3) + (ergebnisfeld[0] << 1);
			temperat = (ergebnisfeld[2] << 3) + (ergebnisfeld[2] << 1);
			break;
		}
		default: {
			temperature_sign = (ergebnisfeld[2] & 0x80) ? 2 : 0;
			ergebnisfeld[2] &= 0x7F;
			humid = (ergebnisfeld[0] << 8) + ergebnisfeld[1];
			temperat = (ergebnisfeld[2] << 8) + ergebnisfeld[3];
			temperat = temperat - temperature_sign * temperat;
			break;
		}
	}
	if(humid<0 || humid>1000 || temperat < -400 || temperat > 850) return errorcounter;

	*temperatur_mal_zehn = temperat;
	*feuchtigkeit_mal_zehn = humid;
	return 0;
}

int16_t calc_average(int16_t* feld, int16_t latest_value, uint8_t vals) {
	uint8_t a;
	int32_t temp = 0;
	if (vals == 0) vals = 1;

	for (a = 0; a < vals - 1; a++) {
		temp += feld[a + 1];
		feld[a] = feld[a + 1];
	}
	feld[vals - 1] = latest_value;
	temp += feld[vals - 1];
	temp /= vals;
	return temp;
}

void temphumprint(char* tempfield, char* humfield, int16_t temp_l, int16_t hum_l, uint8_t digit) {
	uint8_t mod, ganz;

	if (temp_l < 0) {
		tempfield[0] = '-';
		temp_l = -temp_l;
	}
	else tempfield[0] = '+';

	if(digit) ganz = temp_l / 10;
	else {
		ganz = (temp_l + 5) /10;
		if(!ganz) tempfield[0] = ' ';
	}

	if(ganz>99) ganz = 99;

	if (ganz < 10) {
		tempfield[1] = ' ';
		tempfield[2] = ganz + 0x30;
	}
	else {
		tempfield[1] = ganz / 10 + 0x30;
		tempfield[2] = ganz % 10 + 0x30;
	}

	if (digit) {
		mod = temp_l % 10;
		tempfield[3] = '.';
		tempfield[4] = mod + 0x30;
		tempfield[5] = '\0';
	}
	else tempfield[3] = '\0';

	if(digit) ganz = hum_l / 10;
	else ganz = (hum_l + 5) / 10;

	if(ganz>99) ganz = 99;

	if (ganz < 10) {
		humfield[0] = ' ';
		humfield[1] = ganz + 0x30;
	}
	else {
		humfield[0] = ganz / 10 + 0x30;
		humfield[1] = ganz % 10 + 0x30;
	}
	if (digit) {
		mod = hum_l % 10;
		humfield[2] = '.';
		humfield[3] = mod + 0x30;
		humfield[4] = '\0';
	}
	else humfield[2] = '\0';
}


#endif
