/*
 * pyro.h
 *
 *  Created on: 10.07.2013
 *      Author: Felix
 */

#ifndef PYRO_H_
#define PYRO_H_

// Key switch location
#define KEYPORT               C
#define KEYNUM                4

//
#define MOSSWITCH_PORT        D
#define MOSSWITCH_NUM         4

// Ignition time * 10ms
#define EMATCH_TIME           2
#define FLAMER_TIME           35
#define TALON_TIME            245

// Main clock frequency
#ifndef F_CPU
    #define F_CPU             9830400ULL
#endif

// Maximum ID
#ifndef MAX_ID
    #define MAX_ID            30
#endif

#define DEBOUNCE_DEVS         1
#define T0_PRELOAD_VALUE      (256ULL - ((10ULL * F_CPU + 512000ULL) / 1024000ULL))

// Maximum Array Size for communication (UART + radio)
#define MAX_COM_ARRAYSIZE     40

// Number of payload bytes for short/long Rx timeout
#define RFM_SHORT_LENGTH      16
#define RFM_LONG_LENGTH       48

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
#define   BYTE_DURATION_US    ( 8 * ( 1000000UL + BR ) / BR )

#define   setTxCase( XX ) case XX: { loopcount = XX ## _REPEATS; tmp = XX ## _LENGTH - 1; break; }
#define   waitRx( XX )    for ( uint8_t i = rx_field[rx_length - 1] - 1; i; i-- ) _delay_us ( ( ADDITIONAL_LENGTH + ((XX ## _LENGTH)/16 + 1 * (((XX ## _LENGTH)%16) && 1))*16 ) * BYTE_DURATION_US + (500000/BR + 1) + 5005 + (5*RFM_RAMP_UP/4))

// Radio message lengths
#define   ADDITIONAL_LENGTH    12    // Preamble (4) + Passwort (2) + Length Byte (1) + CRC (2) + Spare
#define   FIRE_LENGTH          4
#define   IDENT_LENGTH         4
#define   MEASURE_LENGTH       4
#define   PARAMETERS_LENGTH    10
#define   TEMPERATURE_LENGTH   5
#define   CHANGE_LENGTH        6
#define   RSSITHRESHOLD_LENGTH 2
#define   IMPEDANCES_LENGTH    (4+FIRE_CHANNELS)

// Number of repetitions for radio messages
#define   FIRE_REPEATS          3
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
    uint8_t squelch;
} fireslave_t;

#define TRANSMITTER                  ( !ig_or_notrans )

#define KEY_DDR                      DDR( KEYPORT )
#define KEY_PIN                      PIN( KEYPORT )
#define KEY_PORT                     PORT( KEYPORT )
#define KEY_NUMERIC                  NUMPORT( KEYPORT )
#define KEY                          KEYNUM
#define KEY_PCMSK                    PCMSK( KEY_NUMERIC )
#define KEY_PCINTVECT                PCINTVECT( KEY_NUMERIC )

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

#define START_ADDRESS_AESKEY_STORAGE 32
#define FIRE_DURATION_ADDRESS        365

// Funktionsprototypen
uint8_t asciihex( char inp );

#endif /* PYRO_H_ */
