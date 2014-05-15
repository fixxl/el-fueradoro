/*
 * rfm12.h
 *
 * Definitionen und Funktionen zur Steuerung des Funkmoduls RFM
 * Vielen Dank an Benedikt K., dessen herausragende Vorarbeit Vieles vereinfacht hat!
 */

#ifndef RFM12_H_
#define RFM12_H_

#define FREQUENCY			867595000L 	// Angabe in Hz
#define BITRATE				9600L		// Angabe in baud
#define RFM_P				B
#define NSEL				2
#define SDI					3
#define SDO					4
#define SCK					5

#define USE_NIRQ			0
#define NIRQ				9

#define USE_HARDWARE_SPI	0

// Ab hier nichts mehr anpassen
#define RFM_PORT			PORT(RFM_P)
#define RFM_DDR				DDR(RFM_P)
#define RFM_PIN				PIN(RFM_P)
#define ACTIVATE_RFM		RFM_PORT	&=	~(1<<NSEL);
#define DEACTIVATE_RFM		RFM_PORT	|=	 (1<<NSEL);
#define DATARATE			((((( 344828L - (BITRATE<2694L)*301725L ) + (BITRATE/2))/BITRATE) - 1) | ( (BITRATE<2694) * 0x80 ))
#define FR_CONST_1			( 1 + (FREQUENCY>860000000L) + (FREQUENCY>900000000L) )
#define FR_CONST_2			( 43 - 12*(FREQUENCY<400000000L) - 13*(FREQUENCY>900000000L) )
#define FREQ_CHANNEL 		( FREQUENCY/(2500L * FR_CONST_1) - (4000L*FR_CONST_2) )

#ifndef MAX_ARRAYSIZE
#define MAX_ARRAYSIZE 30
#endif

uint16_t rfm_cmd(uint16_t command);									// Direktes Schreib-/Lesekommando
uint8_t rfm_receiving(void);										// Byte im FIFO?
uint16_t rfm_status(void);											// 16 Statusbits abfragen

void rfm_rxon(void);												// Empfänger einschalten
void rfm_rxoff(void);												// Empfänger ausschalten
void rfm_txon(void);												// Sender einschalten
void rfm_txoff(void);												// Sender ausschalten

void rfm_nirq_clear(void);											// NIRQ-Interrupts löschen
void rfm_wake_up_init(void);										// Wake-Up-Timer initialisieren
void rfm_wake_up_clear(void);										// Wake-Up-Timer abschalten
void rfm_set_timer_and_sleep(uint8_t mantissa, uint8_t exponent);	// Wake-Up-Timer aktivieren

void rfm_init(void);												// Modul initialisieren
uint8_t rfm_transmit(char *data, uint8_t length);					// Daten senden
uint8_t rfm_receive(char *data, uint8_t *length);					// Daten empfangen

// Einfache 16-bit zu 2*8-bit Wandlung
typedef union {
	uint16_t zahl;
	uint8_t bytes[2];
} bitconverter;

#endif
