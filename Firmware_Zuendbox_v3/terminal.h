/*
 * terminal.h
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#define TERM_COL_BLACK   "\033[30m"
#define TERM_COL_RED     "\033[31m"
#define TERM_COL_GREEN   "\033[32m"
#define TERM_COL_YELLOW  "\033[33m"
#define TERM_COL_BLUE    "\033[34m"
#define TERM_COL_MAGENTA "\033[35m"
#define TERM_COL_CYAN    "\033[36m"
#define TERM_COL_WHITE   "\033[37m"

void terminal_reset( void );
void fixedspace( int32_t zahl, uint8_t type, uint8_t space );

uint8_t remote_config( char *txf );
uint8_t configprog( const uint8_t devicetype );
uint8_t aesconf( void );

void list_complete( fireslave_t slaves[MAX_ID + 1], uint8_t wrongids );
void list_array( char *arr );
void evaluate_boxes( fireslave_t boxes[MAX_ID + 1], char *quantity );
#endif /* TERMINAL_H_ */
