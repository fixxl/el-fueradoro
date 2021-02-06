/*
 * pyro.h
 *
 *  Created on: 10.07.2013
 *      Author: Felix
 */



#ifndef PYRO_H_
#define PYRO_H_

// Switches' location
#define ARMPORT			D
#define ARMNUM			2

#define TESTPORT		D
#define TESTNUM			4

#define REEDPORT		D
#define REEDNUM			3

#define HOLDPORT		C
#define HOLDNUM			0	

//
#define MOSSWITCH_PORT        C
#define MOSSWITCH_NUM         2

// Ignition time * 10ms
#define EMATCH_TIME           2
#define TALON_TIME            245

// Main clock frequency
#ifndef F_CPU
    #define F_CPU             11059200UL
#endif
// Maximum ID
#ifndef MAX_ID
    #define MAX_ID            30
#endif

#define DEBOUNCE_DEVS         3
#define T0_PRELOAD_VALUE      (256ULL - ((10ULL * F_CPU + 512000ULL) / 1024000ULL))
// Maximum Array Size for communication (UART + radio)
#define MAX_COM_ARRAYSIZE     40

// Ignition time * 10ms
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
#define   BYTE_DURATION_US    ( 8 * ( 1000000UL + BITRATE ) / BITRATE )

#define   setTxCase( XX ) case XX: { loopcount = XX ## _REPEATS; tmp = XX ## _LENGTH - 1; break; }
#define   waitRx( XX )    for ( uint8_t i = rx_field[XX ## _LENGTH - 1] - 1; i; i-- ) _delay_us ( ( ADDITIONAL_LENGTH + ((XX ## _LENGTH)/16 + 1 * (((XX ## _LENGTH)%16) && 1))*16 ) * BYTE_DURATION_US + (500000/BITRATE) + 5 + (5*RFM_RAMP_UP/4))

// Radio message lengths
#define   ADDITIONAL_LENGTH    10    // Preamble (4) + Passwort (2) + Length Byte (1) + CRC (2) + Spare
#define   FIRE_LENGTH          4
#define   IDENT_LENGTH         4
#define   MEASURE_LENGTH       4
#define   PARAMETERS_LENGTH    9
#define   TEMPERATURE_LENGTH   5
#define   CHANGE_LENGTH        6
#define   RSSITHRESHOLD_LENGTH 2
#define   IMPEDANCES_LENGTH   (4+SR_CHANNELS)

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
        unsigned read_impedance : 1;
        unsigned list_impedance : 1;
        unsigned reset_device   : 1;
		unsigned shutdown		: 1;
        unsigned clear_list     : 1;
        unsigned hw             : 1;
        unsigned remote         : 1;
    }        b;
    uint16_t complete;
} bitfeld_t;

typedef struct {
    uint8_t slave_id;
    uint8_t battery_voltage;
    uint8_t sharpness;
    int8_t  temperature;
    uint8_t channels;
    uint8_t rssi;
} fireslave_t;

#define ARM_DDR                      DDR( ARMPORT )
#define ARM_PIN                      PIN( ARMPORT )
#define ARM_PORT                     PORT( ARMPORT )
#define ARM_NUMERIC                  NUMPORT( ARMPORT )
#define ARM                          ARMNUM
#define ARM_PCMSK					 PCMSK( ARM_NUMERIC )
#define ARM_PCINTVECT                PCINTVECT( ARM_NUMERIC )

#define TEST_DDR                      DDR( TESTPORT )
#define TEST_PIN                      PIN( TESTPORT )
#define TEST_PORT                     PORT( TESTPORT )
#define TEST_NUMERIC                  NUMPORT( TESTPORT )
#define TEST                          TESTNUM
#define TEST_PCMSK		     		  PCMSK( TEST_NUMERIC )
#define TEST_PCINTVECT                PCINTVECT( TEST_NUMERIC )

#define REED_DDR                      DDR( REEDPORT )
#define REED_PIN                      PIN( REEDPORT )
#define REED_PORT                     PORT( REEDPORT )
#define REED_NUMERIC                  NUMPORT( REEDPORT )
#define REED                          REEDNUM
#define REED_PCMSK					  PCMSK( REED_NUMERIC )
#define REED_PCINTVECT                PCINTVECT( REED_NUMERIC )

#define HOLD_ON_DDR                   DDR( HOLDPORT )
#define HOLD_ON_PIN                   PIN( HOLDPORT )
#define HOLD_ON_PORT                  PORT( HOLDPORT )
#define HOLD_ON_NUMERIC               NUMPORT( HOLDPORT )
#define HOLD_ON                       HOLDNUM


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

// Externally used fuctions
uint8_t asciihex( char inp );

#endif /* PYRO_H_ */
