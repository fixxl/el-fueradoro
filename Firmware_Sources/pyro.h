/*
 * pyro.h
 *
 *  Created on: 10.07.2013
 *      Author: Felix
 */

#ifndef PYRO_H_
#define PYRO_H_

// Main clock frequency
#ifndef F_CPU
    #define F_CPU             9830400UL
#endif

// Key switch location
#define KEYPORT               C
#define KEYNUM                4

//
#define MOSSWITCH_PORT        D
#define MOSSWITCH_NUM         4

// Ignition time * 10ms
#define EMATCH_TIME           2
#define TALON_TIME            245

// Maximum ID
// Maximum ID
#ifndef MAX_ID
    #define MAX_ID            30
#endif

// Maximum Array Size
#define MAX_COM_ARRAYSIZE     40

// Threshold to clear LCD (Number of counter overflows)
#define DEL_THRES             251

// Value for input timeout
#define TIMEOUTVAL            ( F_CPU >> 4 )

// Radio message types
#define   FIRE                'f'
#define   IDENT               'i'
#define   ERROR               'e'
#define   PARAMETERS          'p'
#define   TEMPERATURE         't'
#define   CHANGE              'c'
#define   MEASURE             'm'
#define   IMPEDANCES          'z'
#define   RSSITHRESHOLD       'r'
#define   IDLE                0

// Ceiled duration of byte transmission in microseconds
#define   BYTE_DURATION_US    ( 8 * ( 1000000UL + BITRATE ) / BITRATE )

#define   setTxCase( XX ) case XX: { loopcount = XX ## _REPEATS; tmp = XX ## _LENGTH - 1; break; }
#define   waitRx( XX )    for ( uint8_t i = rx_field[XX ## _LENGTH - 1] - 1; i; i-- ) _delay_us ( ( ADDITIONAL_LENGTH + XX ## _LENGTH ) * BYTE_DURATION_US )

// Radio message lengths
#define   ADDITIONAL_LENGTH    13    // Preamble (4) + Passwort (2) + Length Byte (1) + CRC (2) + Spare
#define   FIRE_LENGTH          4
#define   IDENT_LENGTH         4
#define   MEASURE_LENGTH       4
#define   PARAMETERS_LENGTH    8
#define   TEMPERATURE_LENGTH   5
#define   CHANGE_LENGTH        6
#define   RSSITHRESHOLD_LENGTH 2
#define   IMPEDANCES_LENGTH    (4+SR_CHANNELS)

// Number of repetitions for radio messages
#define   FIRE_REPEATS          5
#define   RSSITHRESHOLD_REPEATS 5
#define   IDENT_REPEATS         3
#define   CHANGE_REPEATS        3
#define   PARAMETERS_REPEATS    2
#define   TEMPERATURE_REPEATS   2
#define   MEASURE_REPEATS       2
#define   IMPEDANCES_REPEATS    2

// Bitflags
typedef union {
    struct {
        unsigned uart_active    : 1;
        unsigned uart_config    : 1;
        unsigned fire           : 1;
        unsigned finish_firing  : 1;
        unsigned is_fire_active : 1;
        unsigned send           : 1;
        unsigned transmit       : 1;
        unsigned receive        : 1;
        unsigned list           : 1;
        unsigned lcd_update     : 1;
        unsigned tx_post        : 1;
        unsigned rx_post        : 1;
        unsigned show_only      : 1;
        unsigned reset_device   : 1;
        unsigned clear_list     : 1;
        unsigned hw             : 1;
        unsigned remote         : 1;
    }        b;
    uint32_t complete;
} bitfeld_t;

typedef struct {
    uint8_t slave_id;
    uint8_t battery_voltage;
    uint8_t sharpness;
    int8_t  temperature;
    uint8_t channels;
    uint8_t rssi;
} fireslave_t;

#define TRANSMITTER                  ( !ig_or_notrans )

#define KEY_DDR                      DDR( KEYPORT )
#define KEY_PIN                      PIN( KEYPORT )
#define KEY_PORT                     PORT( KEYPORT )
#define KEY_NUMERIC                  NUMPORT( KEYPORT )
#define KEY                          KEYNUM
#if ( KEY_NUMERIC == 2 )
    #define KEYINT                   PCINT1_vect
#elif ( KEY_NUMERIC == 1 )
    #define KEYINT                   PCINT0_vect
#else
    #define KEYINT                   PCINT2_vect
#endif

#define MOSSWITCHDDR                 DDR( MOSSWITCH_PORT )
#define MOSSWITCHPIN                 PIN( MOSSWITCH_PORT )
#define MOSSWITCHPORT                PORT( MOSSWITCH_PORT )
#define MOSSWITCH                    MOSSWITCH_NUM

// ID storage settings for EEPROM
#define START_ADDRESS_ID_STORAGE     24
#define STEP_ID_STORAGE              36
#define CRC_ID_STORAGE               16
#define ID_MESS                                                                                               \
    !( eeread( START_ADDRESS_ID_STORAGE )                                                                     \
       == eeread( START_ADDRESS_ID_STORAGE + STEP_ID_STORAGE ) )                                              \
    && ( eeread( START_ADDRESS_ID_STORAGE ) == eeread( START_ADDRESS_ID_STORAGE + 2 * STEP_ID_STORAGE ) )     \
    && ( eeread( START_ADDRESS_ID_STORAGE + 1 ) == eeread( START_ADDRESS_ID_STORAGE + 1 + STEP_ID_STORAGE ) ) \
    && ( eeread( START_ADDRESS_ID_STORAGE + 1 ) == eeread( START_ADDRESS_ID_STORAGE + 1 + 2 * STEP_ID_STORAGE ) )

// Temperatursensoren
#define DS18B20                      'o'

#if ( RFM == 69 )
    #define RFM_PWR_ADDRESS          5
#endif

#define START_ADDRESS_AESKEY_STORAGE 32

// Funktionsprototypen
void    wdt_init( void ) __attribute__( ( naked ) ) __attribute__( ( section( ".init1" ) ) );
void    create_symbols( void );
uint8_t asciihex( char inp );
void    key_init( void );
void    key_deinit( void );
uint8_t debounce( volatile uint8_t *port, uint8_t pin );
uint8_t fire_command_uart_valid( const char *field );
#endif /* PYRO_H_ */
