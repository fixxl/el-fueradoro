/*
 * shiftregister.h
 * Einstellungen und Funktionen für die Schieberegister
 */

#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_

// Adjust Ports and Pins here
#define SER_IN_P    C
#define SER_IN_NUM    0
#define OE_P      C
#define OE_NUM      1
#define RCLOCK_P    C
#define RCLOCK_NUM    2
#define SCLOCK_P    C
#define SCLOCK_NUM    3

// How many channels? (8 or 16)
// For 16 the SER_IN of the 74HC595 for channels 9-16
// has to be connected to Q7S of the 74HC595 for channels 1-8
#define SR_CHANNELS   16


// DO NOT CHANGE ANYTHING BELOW THIS LINE!

void sr_init(void);
void sr_shiftout(uint16_t scheme);

// Generation of names
#define SER_IN_PORT   PORT(SER_IN_P)
#define SER_IN_PIN    PIN(SER_IN_P)
#define SER_IN_DDR    DDR(SER_IN_P)
#define SER_IN      SER_IN_NUM
#define OE_PORT     PORT(OE_P)
#define OE_PIN      PIN(OE_P)
#define OE_DDR      DDR(OE_P)
#define OE        OE_NUM
#define RCLOCK_PORT   PORT(RCLOCK_P)
#define RCLOCK_PIN    PIN(RCLOCK_P)
#define RCLOCK_DDR    DDR(RCLOCK_P)
#define RCLOCK      RCLOCK_NUM
#define SCLOCK_PORT   PORT(SCLOCK_P)
#define SCLOCK_PIN    PIN(SCLOCK_P)
#define SCLOCK_DDR    DDR(SCLOCK_P)
#define SCLOCK      SCLOCK_NUM



#endif
