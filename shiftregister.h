/*
 * shiftregister.h
 * Einstellungen und Funktionen für die Schieberegister
 */

#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_

// Hier Pinbelegungen eintragen
#define SER_IN_P		C
#define SER_IN_NUM		0
#define OE_P			C
#define OE_NUM			1
#define RCLOCK_P		C
#define RCLOCK_NUM		2
#define SCLOCK_P		C
#define SCLOCK_NUM		3

// Angabe, wie viele Kanäle (8/16)
#define SR_CHANNELS		16

void sr_init(void);
void sr_shiftout(uint16_t scheme);


// Automatische Generierung der Bezeichnungen aus Angaben oben
#define SER_IN_PORT		PORT(SER_IN_P)
#define SER_IN_PIN		PIN(SER_IN_P)
#define SER_IN_DDR		DDR(SER_IN_P)
#define SER_IN 			SER_IN_NUM
#define OE_PORT			PORT(OE_P)
#define OE_PIN			PIN(OE_P)
#define OE_DDR			DDR(OE_P)
#define OE 				OE_NUM
#define RCLOCK_PORT		PORT(RCLOCK_P)
#define RCLOCK_PIN		PIN(RCLOCK_P)
#define RCLOCK_DDR		DDR(RCLOCK_P)
#define RCLOCK 			RCLOCK_NUM
#define SCLOCK_PORT		PORT(SCLOCK_P)
#define SCLOCK_PIN		PIN(SCLOCK_P)
#define SCLOCK_DDR		DDR(SCLOCK_P)
#define SCLOCK 			SCLOCK_NUM



#endif
