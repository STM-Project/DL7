/*
 * NSMAC.h
 *
 *  Created on: 14 paź 2015
 *      Author: Tomaszs
 */

#ifndef NSMAC_H_
#define NSMAC_H_

extern unsigned int deviceSerialNumber;
extern unsigned char deviceMACAddress[];


void SaveSerialNumberToFRAM(const unsigned int *newSerialNumber);
void SaveMACToFRAM(const unsigned char *newMAC);

void InitSNMAC(void);

unsigned int GetSerialNumber(void);
void GetMACAddress(unsigned char *NewMACAddress);

#endif /* NSMAC_H_ */
