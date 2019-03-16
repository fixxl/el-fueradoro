/*
 * shiftregister.h
 * Einstellungen und Funktionen f�r die Schieberegister
 */

#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_

// Adjust Ports and Pins here
#define SER_IN_P      B
#define SER_IN_NUM    3
#define OE_P          D
#define OE_NUM        4
#define RCLOCK_P      C
#define RCLOCK_NUM    2
#define SCLOCK_P      B
#define SCLOCK_NUM    5

// How many channels? (8 or 16)
// For 16 the SER_IN of the 74HC595 for channels 9-16
// has to be connected to Q7S of the 74HC595 for channels 1-8
#define SR_CHANNELS            16

/* Use Hardware-SPI if available? */
#define SR_USE_HARDWARE_SPI    1

// DO NOT CHANGE ANYTHING BELOW THIS LINE!

void sr_init(void);
void sr_enable(void);
void sr_disable(void);
void sr_shiftout(uint16_t scheme);

// Generation of names
#define SER_IN_PORT      PORT(SER_IN_P)
#define SER_IN_PIN       PIN(SER_IN_P)
#define SER_IN_DDR       DDR(SER_IN_P)
#define SER_IN_NUMERIC   NUMPORT(SER_IN_P)
#define SER_IN           SER_IN_NUM

#define OE_PORT          PORT(OE_P)
#define OE_PIN           PIN(OE_P)
#define OE_DDR           DDR(OE_P)
#define OE_NUMERIC       NUMPORT(OE_P)
#define OE               OE_NUM

#define RCLOCK_PORT      PORT(RCLOCK_P)
#define RCLOCK_PIN       PIN(RCLOCK_P)
#define RCLOCK_DDR       DDR(RCLOCK_P)
#define RCLOCK_NUMERIC   NUMPORT(RCLOCK_P)
#define RCLOCK           RCLOCK_NUM

#define SCLOCK_PORT      PORT(SCLOCK_P)
#define SCLOCK_PIN       PIN(SCLOCK_P)
#define SCLOCK_DDR       DDR(SCLOCK_P)
#define SCLOCK_NUMERIC   NUMPORT(SCLOCK_P)
#define SCLOCK           SCLOCK_NUM

#ifdef SPDR
 #define HASHARDSPISR    1
#else
 #define HASHARDSPISR    0
#endif

#define HARDWARE_SPI_SR                                                                 \
    (SR_USE_HARDWARE_SPI && HASHARDSPISR && (SER_IN_NUMERIC == SCLOCK_NUMERIC) && (SCLOCK_NUMERIC == 1) && \
     (SER_IN == 3) && (SCLOCK == 5))
#endif
