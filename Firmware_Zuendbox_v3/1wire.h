#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#define W1PORT            B
#define W1NUM             1

#define W1_PORT           PORT(W1PORT)
#define W1_DDR            DDR(W1PORT)
#define W1_PIN            PIN(W1PORT)
#define W1                W1NUM

#define MATCH_ROM         0x55
#define SKIP_ROM          0xCC
#define SEARCH_ROM        0xF0

#define CONVERT_T         0x44 // DS1820 commands
#define READ              0xBE
#define WRITE             0x4E
#define EE_WRITE          0x48
#define EE_RECALL         0xB8

#define SEARCH_FIRST      0xFF // start new search
#define PRESENCE_ERR      0xFF
#define DATA_ERR          0xFE
#define LAST_DEVICE       0x00 // last device found
// 0x01 ... 0x40: continue searching
#define DEVICE_REMOVED    0xAA55

#define NULL              ((void *)0) // Nullpointer

uint8_t w1_reset(void);

uint8_t w1_bit_io(uint8_t val);
uint8_t w1_byte_wr(uint8_t byte);
uint8_t w1_byte_rd(void);
void w1_command(uint8_t command, uint8_t *id);
uint8_t w1_rom_search(uint8_t diff, uint8_t *id);
uint8_t w1_get_sensor_ids(uint8_t id_field[][8]);

// Speziell für 1-Wire-Temperatursensoren
void w1_temp_conf(int8_t th, int8_t tl, uint8_t res);
uint16_t w1_read_temp(uint8_t *id);
int16_t w1_tempread_to_celsius(uint16_t temp, uint8_t digit);
int16_t w1_tempmeas(uint8_t byten);
void w1_temp_to_array(int32_t tempmalzehn, char *tempfield, uint8_t signdigit);
#endif
