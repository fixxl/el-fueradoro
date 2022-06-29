/*
 * dm13a.h
 * Einstellungen und Funktionen f�r die Schieberegister
 */

#ifndef DM13A_H_
#define DM13A_H_

// Adjust Ports and Pins here
#define DAI_P               B
#define DAI_NUM             3
#define EN_P                D
#define EN_NUM              4
#define LAT_P               C
#define LAT_NUM             0
#define DCK_P               B
#define DCK_NUM             5

// How many channels?
// For concatenating two DM13A DAI of the device for channels 17-32
// has to be connected to Q7S of the device for channels 1-8
#ifndef DM_CHANNELS
    #define DM_CHANNELS         16
#endif

/* Use Hardware-SPI if available? */
#define DM_USE_HARDWARE_SPI 1

// DO NOT CHANGE ANYTHING BELOW THIS LINE!

void dm_init( void );
void dm_enable( void );
void dm_disable( void );
void dm_shiftout( uint32_t scheme );

// Generation of names
#define DAI_PORT            PORT( DAI_P )
#define DAI_PIN             PIN( DAI_P )
#define DAI_DDR             DDR( DAI_P )
#define DAI_NUMERIC         NUMPORT( DAI_P )
#define DAI                 DAI_NUM

#define EN_PORT             PORT( EN_P )
#define EN_PIN              PIN( EN_P )
#define EN_DDR              DDR( EN_P )
#define EN_NUMERIC          NUMPORT( EN_P )
#define EN                  EN_NUM

#define LAT_PORT            PORT( LAT_P )
#define LAT_PIN             PIN( LAT_P )
#define LAT_DDR             DDR( LAT_P )
#define LAT_NUMERIC         NUMPORT( LAT_P )
#define LAT                 LAT_NUM

#define DCK_PORT            PORT( DCK_P )
#define DCK_PIN             PIN( DCK_P )
#define DCK_DDR             DDR( DCK_P )
#define DCK_NUMERIC         NUMPORT( DCK_P )
#define DCK                 DCK_NUM

#ifdef SPDR
    #define HASHARDSPIDM    1
#else
    #define HASHARDSPIDM    0
#endif

#define HARDWARE_SPI_DM                                                                               \
    (   DM_USE_HARDWARE_SPI && HASHARDSPIDM && ( DAI_NUMERIC == DCK_NUMERIC ) && ( DCK_NUMERIC == BPORT ) \
    && ( DAI == 3 ) && ( DCK == 5 ) )
#endif
