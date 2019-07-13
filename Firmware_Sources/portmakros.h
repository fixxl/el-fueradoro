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
#define DDR( x )       CONCAT( DDR, x )
#define PORT( x )      CONCAT( PORT, x )
#define PIN( x )       CONCAT( PIN, x )
#define BPORT          1
#define CPORT          2
#define DPORT          3
#define NUMPORT( x )   CONCAT( x, PORT )
#endif /* PORTMAKROS_H_ */
