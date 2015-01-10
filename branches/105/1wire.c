/************************************************************************/
/*																		*/
/*			Access Dallas 1-Wire Devices								*/
/*                                                                      */
/*                                                                      */
/************************************************************************/

#include "global.h"

#ifdef ONEWIRE_H_

uint8_t w1_reset(void) {
	W1_DDR |= (1 << W1);
	_delay_us(500);
	W1_DDR &= ~(1 << W1);
	_delay_us(66);
	if (W1_PIN & (1 << W1)) return 1;
	_delay_us(434);
	return 0;
}

uint8_t w1_bit_io(uint8_t val) {
	W1_DDR |= (1 << W1);
	_delay_us(5);
	if (val) W1_DDR &= ~(1 << W1);
	_delay_us(14);
	if (!(W1_PIN & (1 << W1))) val = 0;
	_delay_us(41);
	W1_DDR &= ~(1 << W1);
	return val;
}

uint8_t w1_byte_wr(uint8_t byte) {
	uint8_t j;
	for (uint8_t i = 0; i < 8; i++) {
		j = w1_bit_io(byte & 1);
		byte >>= 1;
		if (j) byte |= 0x80;
	}
	return byte;
}

uint8_t w1_byte_rd(void) {
	return w1_byte_wr(0xFF);
}

void w1_command(uint8_t command, uint8_t *id) {
	uint8_t i;
	w1_reset();

	if (id) {
		w1_byte_wr(MATCH_ROM); // to a single device
		i = 8;
		do {
			w1_byte_wr(*id++);
		} while (--i);
	}
	else {
		w1_byte_wr(SKIP_ROM); // to all devices
	}

	w1_byte_wr(command);
}

uint8_t w1_rom_search(uint8_t last_discrepancy, uint8_t *id) {
	uint8_t id_bit_number, j, last_zero = LAST_DEVICE;
	uint8_t id_bit, cmp_id_bit, crcw = 1;

	while (crcw) {
		crcw = 0;
		if (w1_reset()) return PRESENCE_ERR; // error, no device found
		w1_byte_wr(SEARCH_ROM); // issue ROM search command

		last_zero = LAST_DEVICE; // set last position where zero was written to 0

		id_bit_number = 64; // 8 bytes (64 bits)
		while (id_bit_number) {
			for (j = 8; j > 0; j--) { // 8 bits can be stored in 1 byte
				id_bit = w1_bit_io(1); // read bit
				cmp_id_bit = w1_bit_io(1); // read complement bit

				if (id_bit && cmp_id_bit) return DATA_ERR; // data error if bit AND complement are 1

				if (!id_bit && !cmp_id_bit) { // Discrepancy if bit AND complement are 0

					// Go the '1'-direction if last_discrepancy is bigger than id_bit_number.
					// Go the '0'-direction if last_discrepancy is equal to id_bit_number.
					// Go the same direction as last time if last_discrepancy is smaller than id_bit_number
					if ((last_discrepancy > id_bit_number)
							|| ((*id & 1) && (last_discrepancy != id_bit_number))) {
						id_bit = 1; // now 1
						last_zero = id_bit_number; // next pass 0
					}

				}

				w1_bit_io(id_bit); // write bit
				*id >>= 1;
				if (id_bit) *id |= 0x80; // store bit
				id_bit_number--;
			}
			crcw = crc8(crcw, *id);
			id++; // go to next byte
		}
	}
	return last_zero; // to continue search
}

uint8_t w1_get_sensor_ids(uint8_t id_field[][8]) {
	uint8_t devnum, diff, id[9];

	devnum = 0;
	diff = SEARCH_FIRST;
	while (diff != LAST_DEVICE && devnum < 25) {
		diff = w1_rom_search(diff, id);
		for (uint8_t i = 0; i < 8; i++) {
			id_field[devnum][i] = id[i];
		}
		devnum++;
	}
	return devnum;
}

void w1_temp_conf(int8_t th, int8_t tl, uint8_t res) {
	res -= 9;
	res <<= 5;
	res &= 0x7F;
	res |= 0x1F;

	w1_command(WRITE, NULL);
	w1_byte_wr(th);
	w1_byte_wr(tl);
	w1_byte_wr(res);
}

uint16_t w1_read_temp(uint8_t *id) {
	uint8_t scratchpad[10], value, crcw = 1;

	while (crcw) {
		crcw = 0;
		w1_command(READ, id);
		for (uint8_t i = 0; i < 9; i++) {
			value = w1_byte_rd();
			scratchpad[i] = value;
			crcw = crc8(crcw, value);
		}
		if (scratchpad[7] == 0xFF) return DEVICE_REMOVED;
	}
	return (scratchpad[0] + (scratchpad[1] << 8));
}

int16_t w1_tempread_to_celsius(uint16_t temp) {
	int16_t celsius = (temp & 0xF800) ? -1 : 1;

	if (celsius < 0) temp = -temp;
	celsius *= (((temp << 2) + temp + 4) >> 3);

	return celsius;
}

int16_t w1_tempmeas(void) {
	uint8_t diff, id[9];
	uint16_t temp_hex;
	int16_t temp;

	w1_command(CONVERT_T, NULL);
	while (!w1_bit_io(1))
		;
	diff = SEARCH_FIRST;
	diff = w1_rom_search(diff, id);
	w1_command(READ, id);
	temp_hex = w1_byte_rd();
	temp_hex += (w1_byte_rd()) << 8;
	temp = w1_tempread_to_celsius(temp_hex);
	return temp;
}

void w1_temp_to_array(int32_t tempmalzehn, char* tempfield, uint8_t signdigit) {
	/* 0 < signdigit < 3
	 * 0 ... show no sign, no digit
	 * 1 ... show no sign, digit
	 * 2 ... show sign, no digit
	 * 3 ... show sign, digit
	 */
	signdigit &= 0x03;
	uint8_t fieldcntr = 0, neededlength = 1;

	int8_t temp_int_loc = tempmalzehn / 10;
	uint8_t temp_digit_loc = tempmalzehn % 10;

	if(tempmalzehn<0) {
		tempfield[fieldcntr++] = '-';
		tempmalzehn = -tempmalzehn;
	}
	int8_t zahlkopie = temp_int_loc;
	while (zahlkopie /= 10) {
		neededlength++;
	}
	if((signdigit & 0x02) && !fieldcntr) {
		tempfield[fieldcntr++] = '+';
	}
	for (uint8_t i = neededlength + fieldcntr; (i - fieldcntr); i--) {
		tempfield[i-1] = (temp_int_loc % 10) + 0x30;
		temp_int_loc /= 10;
	}
	if(signdigit & 1) {
		tempfield[neededlength + fieldcntr] = '.';
		tempfield[neededlength + fieldcntr + 1] = (temp_digit_loc % 10) + 0x30;
		tempfield[neededlength + fieldcntr + 2] = '\0';
	}
	else {
		tempfield[neededlength + fieldcntr] = '\0';
	}
}


#endif
