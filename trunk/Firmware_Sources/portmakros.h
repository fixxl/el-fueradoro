/*
 * portmakros.h
 *
 *  Created on: 25.04.2013
 *      Author: Felix
 */

#ifndef PORTMAKROS_H_
#define PORTMAKROS_H_

// Definitions for IO-Port-Makros
#define 	CONCAT(x,y) 	x##y
#define 	DDR(x)  		CONCAT(DDR,x)
#define 	PORT(x) 		CONCAT(PORT,x)
#define 	PIN(x)  		CONCAT(PIN,x)

#endif /* PORTMAKROS_H_ */
