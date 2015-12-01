/*
 * rfm12.h
 *
 * Definitionen und Funktionen zur Steuerung des Funkmoduls RFM
 * Vielen Dank an Benedikt K., dessen herausragende Vorarbeit Vieles vereinfacht hat!
 */

#ifndef RFM12_H_
#define RFM12_H_

#define FREQUENCY           867595000L // Value in Hz
#define BITRATE             9600L      // Value in baud

#define NSELPORT            B
#define NSEL                2
#define SDIPORT             B
#define SDI                 3
#define SDOPORT             B
#define SDO                 4
#define SCKPORT             B
#define SCK                 5

#define USE_NIRQ            0
#define NIRQPORT            B
#define NIRQ                9

#define RFM12_USE_HARDWARE_SPI    1

// Don't change anything from here on

#define NSEL_PORT         PORT(NSELPORT)
#define NSEL_DDR          DDR(NSELPORT)
#define NSEL_PIN          PIN(NSELPORT)

#define SDI_PORT          PORT(SDIPORT)
#define SDI_DDR           DDR(SDIPORT)
#define SDI_PIN           PIN(SDIPORT)

#define SDO_PORT          PORT(SDOPORT)
#define SDO_DDR           DDR(SDOPORT)
#define SDO_PIN           PIN(SDOPORT)

#define SCK_PORT          PORT(SCKPORT)
#define SCK_DDR           DDR(SCKPORT)
#define SCK_PIN           PIN(SCKPORT)

#define NIRQ_PORT         PORT(NIRQPORT)
#define NIRQ_DDR          DDR(NIRQPORT)
#define NIRQ_PIN          PIN(NIRQPORT)

#define ACTIVATE_RFM      NSEL_PORT &= ~(1 << NSEL)
#define DEACTIVATE_RFM    NSEL_PORT |= (1 << NSEL)

#define DATARATE  \
	(((((344828L - \
	     (BITRATE < 2694L) * 301725L) + (BITRATE / 2)) / BITRATE) - 1) | ((BITRATE < 2694) * 0x80))
#define FR_CONST_1        (1 + (FREQUENCY > 860000000L) + (FREQUENCY > 900000000L))
#define FR_CONST_2        (43 - 12 * (FREQUENCY < 400000000L) - 13 * (FREQUENCY > 900000000L))
#define FREQ_CHANNEL      (FREQUENCY / (2500L * FR_CONST_1) - (4000L * FR_CONST_2))

#ifndef MAX_ARRAYSIZE
 #define MAX_ARRAYSIZE    30
#endif

// Value for input timeout
#ifndef RFM12_TIMEOUTVAL
 #define RFM12_TIMEOUTVAL    (F_CPU / 32)
#endif

#ifdef SPDR
 #define HASHARDSPI12    1
#else
 #define HASHARDSPI12    0
#endif


#define HARDWARE_SPI_12                                                                   \
	(RFM12_USE_HARDWARE_SPI && HASHARDSPI12 && (NSELPORT == SDOPORT) && (NSELPORT == SDIPORT) && \
	 (NSELPORT == SCKPORT) && (NSELPORT == B) && (SDI == 3) && (SDO == 4) && (SCK == 5))

uint16_t rfm_cmd(uint16_t command);                               // Immediate access to register
uint8_t rfm_receiving(void);                                      // FIFO not empty?
uint16_t rfm_status(void);                                        // Query the 16 status bits

uint8_t rfm_rxon(void);                                           // Turn on Receiver
uint8_t rfm_rxoff(void);                                          // Turn off Receiver
uint8_t rfm_txon(void);                                           // Turn on Transmitter
uint8_t rfm_txoff(void);                                          // Turn off Transmitter

void rfm_init(void);                                              // Initialise module
uint8_t rfm_transmit(char *data, uint8_t length);                 // Send data
uint8_t rfm_receive(char *data, uint8_t *length);                 // Receive data

void rfm_nirq_clear(void);                                        // Clear NIRQ-Interrupts
void rfm_wake_up_init(void);                                      // Initialise Wake-Up-Timer
void rfm_wake_up_clear(void);                                     // Turn off Wake-Up-Timer
void rfm_set_timer_and_sleep(uint8_t mantissa, uint8_t exponent); // Activate Wake-Up-Timer

// Convert 16-bit to 2*8-bit
typedef union {
	uint16_t zahl;
	uint8_t  bytes[2];
} bitconverter;
#endif
