/*
 * addresses.h
 *
 *  Created on: 06.11.2013
 *      Author: Felix
 */

#ifndef ADDRESSES_H_
#define ADDRESSES_H_

void update_addresses(uint8_t *unique_id, uint8_t *slave_id);
uint8_t address_valid(uint8_t unique_id, uint8_t slave_id);
uint8_t addresses_load(uint8_t *uniqueid, uint8_t *slaveid);
void addresses_save(uint8_t uniqueid, uint8_t slaveid);
#endif /* ADDRESSES_H_ */
