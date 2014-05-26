/*
 * rfm12.h
 *
 * Definitionen und Funktionen zur Steuerung des Funkmoduls RFM
 * Vielen Dank an Benedikt K., dessen herausragende Vorarbeit Vieles vereinfacht hat!
 */

#ifndef RFM12_H_
#define RFM12_H_

#define FREQUENCY			867595000L 	// Value in Hz
#define BITRATE				9600L		// Value in baud
#define RFM_P				B
#define NSEL				2
#define SDI					3
#define SDO					4
#define SCK					5

#define USE_NIRQ			0
#define NIRQ				9

#define USE_HARDWARE_SPI	0

// Don't change anything from here
#define RFM_PORT			PORT(RFM_P)
#define RFM_DDR				DDR(RFM_P)
#define RFM_PIN				PIN(RFM_P)
#define ACTIVATE_RFM		RFM_PORT	&=	~(1<<NSEL)
#define DEACTIVATE_RFM		RFM_PORT	|=	 (1<<NSEL)
#define DATARATE			((((( 344828L - (BITRATE<2694L)*301725L ) + (BITRATE/2))/BITRATE) - 1) | ( (BITRATE<2694) * 0x80 ))
#define FR_CONST_1			( 1 + (FREQUENCY>860000000L) + (FREQUENCY>900000000L) )
#define FR_CONST_2			( 43 - 12*(FREQUENCY<400000000L) - 13*(FREQUENCY>900000000L) )
#define FREQ_CHANNEL 		( FREQUENCY/(2500L * FR_CONST_1) - (4000L*FR_CONST_2) )

#ifndef MAX_ARRAYSIZE
#define MAX_ARRAYSIZE 30
#endif

uint16_t rfm_cmd(uint16_t command);									// Immediate access to register
uint8_t rfm_receiving(void);										// FIFO not empty?
uint16_t rfm_status(void);											// Query the 16 status bits

void rfm_rxon(void);												// Turn on Receiver
void rfm_rxoff(void);												// Turn off Receiver
void rfm_txon(void);												// Turn on Transmitter
void rfm_txoff(void);												// Turn off Transmitter

void rfm_nirq_clear(void);											// Clear NIRQ-Interrupts
void rfm_wake_up_init(void);										// Initialise Wake-Up-Timer
void rfm_wake_up_clear(void);										// Turn off Wake-Up-Timer
void rfm_set_timer_and_sleep(uint8_t mantissa, uint8_t exponent);	// Activate Wake-Up-Timer

void rfm_init(void);												// Initialise module
uint8_t rfm_transmit(char *data, uint8_t length);					// Send data
uint8_t rfm_receive(char *data, uint8_t *length);					// Receive data

// Convert 16-bit to 2*8-bit
typedef union {
	uint16_t zahl;
	uint8_t bytes[2];
} bitconverter;

#endif
