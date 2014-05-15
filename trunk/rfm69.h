/*
 * rfm69.h
 *
 * Definitionen und Funktionen zur Steuerung des Funkmoduls RFM69
 *
 */

#ifndef RFM69_H_
#define RFM69_H_

/* Carrierfrequenz in Hz */
#define FREQUENCY			867595000LL

/* Bitrate in bps (1200 ... 300000) */#define BITRATE				9600L
#define RFM12COMP			1

/* Ausgangsleistung in dBm (-18...13) */
#define P_OUT_DBM			0			// Ausgangsleistung in dBm

/* Anschlussbelegung */#define RFM_P				B
#define NSEL				2
#define SDI					3
#define SDO					4
#define SCK					5

/* Hardware-SPI verwenden, falls vorhanden? */
#define USE_HARDWARE_SPI	1

// Ab hier nichts mehr anpassen
#define XTALFREQ			32000000LL	// Angabe in Hz
#define RFM_PORT			PORT(RFM_P)
#define RFM_DDR				DDR(RFM_P)
#define RFM_PIN				PIN(RFM_P)
#define ACTIVATE_RFM		RFM_PORT	&=	~(1<<NSEL)
#define DEACTIVATE_RFM		RFM_PORT	|=	 (1<<NSEL)

#define FRF					((FREQUENCY*524288LL + (XTALFREQ/2)) / XTALFREQ)
#define FRF_MSB				((FRF>>16) & 0xFF)
#define FRF_MID				((FRF>>8) & 0xFF)
#define FRF_LSB				((FRF) & 0xFF)

#if RFM12COMP
#define BR_VAL				(((((( 344828L - (BITRATE<2694L)*301725L ) + (BITRATE/2))/BITRATE) - 1) | ( (BITRATE<2694) * 0x80 )))
#if BR_VAL<128
#define BR                  ((10000000L + (29*BR_VAL/2))/(29*(BR_VAL+1)))
#else
#define BR_VAL_S			((BR_VAL-127)*8)
#define BR					((10000000L + (29*BR_VAL_S/2))/(29*(BR_VAL_S)))
#endif
#else
#define BR                  BITRATE
#endif

#define DATARATE			((XTALFREQ + (BR/2)) / BR)
#define DATARATE_MSB		(DATARATE>>8)
#define DATARATE_LSB		(DATARATE & 0xFF)

#define P_OUT				( ( P_OUT_DBM + 18 )*(P_OUT_DBM>-19)*(P_OUT_DBM<14) + 31*(P_OUT_DBM>18) )

#ifndef MAX_ARRAYSIZE
#define MAX_ARRAYSIZE 30
#endif

#ifdef SPDR
#define HASHARDSPI	1
#else
#define HASHARDSPI 0
#endif

#define HARDWARE_SPI	(USE_HARDWARE_SPI && HASHARDSPI)


uint8_t rfm_cmd(uint16_t command, uint8_t wnr); 	// Direktes Schreib-/Lesekommando
uint8_t rfm_receiving(void);						// Wurde gültiges Datenpaket empfangen?
uint8_t rfm_get_rssi_dbm(void);						// RSSI-Wert zurückgeben. RSSI = -1dBm * Rückgabewert
uint16_t rfm_status(void);							// Inhalt der Statusregister 0x27 (15:8) und 0x28 (7:0)

void rfm_rxon(void);								// Empfänger einschalten
void rfm_rxoff(void);								// Empfänger ausschalten
void rfm_txon(void);								// Sender einschalten
void rfm_txoff(void);								// Sender ausschalten

void rfm_init(void);								// Initialisierung
uint8_t rfm_transmit(char *data, uint8_t length);   // Datenpaket senden
uint8_t rfm_receive(char *data, uint8_t *length);	// Empfangenes Paket auslesen

#endif
