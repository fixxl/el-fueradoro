#include "global.h"

#ifndef DHT_H_
#define DHT_H_

#define		DHT_TYPE		22		// 11 = DHT11, 22 = DHT22

#define 	TEMPHUM_PORT	B
#define 	TEMPHUM_NUM		1

uint8_t dht_read(int16_t* temperatur_mal_zehn, int16_t* feuchtigkeit_mal_zehn);
int16_t calc_average(int16_t* feld, int16_t latest_value, uint8_t vals);
void temphumprint(char* tempfield, char* humfield, int16_t temp_l, int16_t hum_l, uint8_t digit);

#define		TEMPHUMPORT			PORT(TEMPHUM_PORT)
#define		TEMPHUMPIN			PIN(TEMPHUM_PORT)
#define		TEMPHUMDDR			DDR(TEMPHUM_PORT)
#define		TEMPHUM				TEMPHUM_NUM

#endif
