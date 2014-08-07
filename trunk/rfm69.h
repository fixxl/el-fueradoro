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

/* Bitrate in bps (1200 ... 300000) and shall it be calculated as for RFM12
 * in order to get equal bitrate for RFM12 and RFM69 if used together? */
#define BITRATE				9600L
#define RFM12COMP			1

/* Output power in dBm (-18...13) */
#define P_OUT_DBM			0			// Output power in dBm

/* Pin assignment */
#define NSELPORT			B
#define NSEL				2
#define SDIPORT				B
#define SDI					3
#define SDOPORT				B
#define SDO					4
#define SCKPORT				B
#define SCK					5

/* Use Hardware-SPI if available? */
#define USE_HARDWARE_SPI	0

// Don't change anything from here
#define XTALFREQ			32000000UL

#define NSEL_PORT			PORT(NSELPORT)
#define NSEL_DDR			DDR(NSELPORT)
#define NSEL_PIN			PIN(NSELPORT)

#define SDI_PORT			PORT(SDIPORT)
#define SDI_DDR				DDR(SDIPORT)
#define SDI_PIN				PIN(SDIPORT)

#define SDO_PORT			PORT(SDOPORT)
#define SDO_DDR				DDR(SDOPORT)
#define SDO_PIN				PIN(SDOPORT)

#define SCK_PORT			PORT(SCKPORT)
#define SCK_DDR				DDR(SCKPORT)
#define SCK_PIN				PIN(SCKPORT)

#define ACTIVATE_RFM		NSEL_PORT	&=	~(1<<NSEL)
#define DEACTIVATE_RFM		NSEL_PORT	|=	 (1<<NSEL)

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

// Value for input timeout
#ifndef TIMEOUTVAL
#define TIMEOUTVAL			(F_CPU>>2)
#endif

#ifdef SPDR
#define HASHARDSPI	1
#else
#define HASHARDSPI 0
#endif

#define HARDWARE_SPI	(USE_HARDWARE_SPI && HASHARDSPI && (NSELPORT==SDOPORT) && (NSELPORT==SDIPORT) && (NSELPORT==SCKPORT) && (NSELPORT==B) && (SDI==3) && (SDO==4) && (SCK==5))

uint8_t rfm_cmd(uint16_t command, uint8_t wnr); 	// Immediate access to register
uint8_t rfm_receiving(void);						// Valid data received?
uint16_t rfm_status(void);							// Contents of status registers 0x27 (15:8) and 0x28 (7:0)

uint8_t rfm_rxon(void);								// Turn on Receiver
uint8_t rfm_rxoff(void);							// Turn off Receiver
uint8_t rfm_txon(void);								// Turn on Transmitter
uint8_t rfm_txoff(void);							// Turn off Transmitter

void rfm_init(void);								// Initialisation
uint8_t rfm_transmit(char *data, uint8_t length);   // Transmit data
uint8_t rfm_receive(char *data, uint8_t *length);	// Get received data

uint8_t rfm_get_rssi_dbm(void);						// Return RSSI-Value. Real RSSI = -1dBm * returned value

#endif
