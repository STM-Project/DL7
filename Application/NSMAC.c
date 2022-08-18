/*
 * NSMAC.c
 *
 *  Created on: 14 paź 2015
 *      Author: Tomaszs
 */

#include "NSMAC.h"
#include "stdint.h"
#include "fram.h"

unsigned int deviceSerialNumber __attribute__ ((section(".sdram")));
unsigned char deviceMACAddress[6] __attribute__ ((section(".sdram")));

void SaveSerialNumberToFRAM(const unsigned int *newSerialNumber)
{
	FRAM_WriteMultiple(SERIALNUMBER_ADDR, (uint8_t *) newSerialNumber, 4);
	deviceSerialNumber = *newSerialNumber;
}

void SaveMACToFRAM(const unsigned char *newMAC)
{
	FRAM_WriteMultiple(MAC_ADDRESS_ADDR, (uint8_t *) newMAC, 6);
	for (int i = 0; i < 6; i++)
		deviceMACAddress[i] = newMAC[i];
}

void InitSNMAC(void)
{
	FRAM_ReadMultiple(SERIALNUMBER_ADDR, (uint8_t*) &deviceSerialNumber, 4);
	FRAM_ReadMultiple(MAC_ADDRESS_ADDR, (uint8_t*) &deviceMACAddress, 6);
}

unsigned int GetSerialNumber(void)
{
	return deviceSerialNumber;
}

void GetMACAddress(unsigned char *NewMACAddress)
{
	for (int i = 0; i < 6; i++)
		NewMACAddress[i] = deviceMACAddress[i];
}
