/*
 * dm13a.c
 *
 *  Created on: 15.06.2013
 *      Author: Felix
 */

#include "global.h"

// Initialize
void dm_init( void ) {
    // Activate pullup at /EN, so /EN stays high when switching from input to output
    // and outputs of the 74HC595 stay in high-Z-mode!
    EN_PORT |= ( 1 << EN );

    // Configure pins as outputs
    DAI_DDR |= 1 << DAI;
    EN_DDR  |= 1 << EN;
    LAT_DDR |= 1 << LAT;
    DCK_DDR |= 1 << DCK;

    // All outputs except /EN to low
    DAI_PORT &= ~( 1 << DAI );
    LAT_PORT &= ~( 1 << LAT );
    DCK_PORT &= ~( 1 << DCK );

    // Set shift-register values to 0...
    for ( uint8_t j = 3; j; j-- ) {
        for ( uint8_t i = ( DM_CHANNELS + 4 ); i; i-- ) {
            DCK_PIN = ( 1 << DCK ); // Pin high after toggling
            DCK_PIN = ( 1 << DCK ); // Pin low after toggling
        }

        // ... and apply shift-register to output register.
        LAT_PIN = ( 1 << LAT ); // Pin high after toggling
        LAT_PIN = ( 1 << LAT ); // Pin low after toggling
    }

    _delay_ms( 1 );

    // Now outputs are all low and can be activated!
    EN_PORT &= ~( 1 << EN );

    #if HARDWARE_SPI_DM

        // Activate and configure hardware SPI at F_CPU/16
        if ( !( SPCR & ( 1 << SPE ) ) ) {
            SPCR |= ( 1 << SPE | 1 << MSTR | 1 << SPR0 );
        }

    #endif
}

void dm_enable( void ) {
    EN_PORT &= ~( 1 << EN );
}

void dm_disable( void ) {
    EN_PORT |= ( 1 << EN );
}

// Transfer 16 bit pattern to outputs
void dm_shiftout( uint16_t scheme ) {
    DAI_PORT &= ~( 1 << DAI );
    DCK_PORT &= ~( 1 << DCK );
    LAT_PORT &= ~( 1 << LAT );

    #if HARDWARE_SPI_DM
        SPDR = ( scheme >> 8 ) & 0xFF;

        while ( !( SPSR & ( 1 << SPIF ) ) );

        SPDR = scheme & 0xFF;

        while ( !( SPSR & ( 1 << SPIF ) ) );

    #else
        uint16_t mask = 1 << ( DM_CHANNELS - 1 );

        for ( uint8_t i = DM_CHANNELS; i; i-- ) {
            if ( scheme & mask ) {
                DAI_PORT |= 1 << DAI;
            }

            DCK_PIN   = ( 1 << DCK ); // Pin high after toggling
            DCK_PIN   = ( 1 << DCK ); // Pin low after toggling
            DAI_PORT &= ~( 1 << DAI );
            mask    >>= 1;
        }
    #endif
    LAT_PIN = ( 1 << LAT ); // Pin high after toggling
    LAT_PIN = ( 1 << LAT ); // Pin low after toggling
}