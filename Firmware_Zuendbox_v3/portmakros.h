/*
 * portmakros.h
 *
 *  Created on: 25.04.2013
 *      Author: Felix
 */

#ifndef PORTMAKROS_H_
#define PORTMAKROS_H_

// Definitions for IO-Port-Makros
#define CONCAT( x, y ) x ## y
#define CONCAT3( x, y, z ) x ## y ## z
#define DDR( x )       CONCAT( DDR, x )
#define PORT( x )      CONCAT( PORT, x )
#define PIN( x )       CONCAT( PIN, x )
#define BPORT          0
#define CPORT          1
#define DPORT          2
#define NUMPORT( x )   CONCAT( x, PORT )
#define PCMSK( x )     CONCAT( PCMSK, x )
#define PCINTVECT( x ) CONCAT3( PCINT, x, _vect )

#endif /* PORTMAKROS_H_ */
