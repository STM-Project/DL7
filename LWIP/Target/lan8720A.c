/*
 * lan8720A.c
 *
 *  Created on: Nov 16, 2020
 *      Author: TomaszSok
 */

#include "lan8720A.h"


HAL_StatusTypeDef PHY_Init(ETH_HandleTypeDef *heth)
{
	/*-------------------- PHY initialization and configuration ----------------*/
  uint32_t phyreg = 0;

	/* Put the PHY in reset mode */
	if ((HAL_ETH_WritePHYRegister(heth, LAN8720A_PHY_ADDRESS_PHY_ADDRESS, PHY_BCR, PHY_RESET)) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Delay to assure PHY reset */
	HAL_Delay(PHY_RESET_DELAY);

	/* We wait for linked status */
	HAL_ETH_ReadPHYRegister(heth, LAN8720A_PHY_ADDRESS_PHY_ADDRESS, PHY_BSR, &phyreg);
	if((phyreg & PHY_LINKED_STATUS))
	{
		/* Enable Auto-Negotiation */
		if ((HAL_ETH_WritePHYRegister(heth,LAN8720A_PHY_ADDRESS_PHY_ADDRESS, PHY_BCR, PHY_AUTONEGOTIATION)) != HAL_OK)
		{
			/* Return HAL_ERROR */
			return HAL_ERROR;
		}

		/* Wait until the auto-negotiation will be completed */
		do
		{
			HAL_ETH_ReadPHYRegister(heth, LAN8720A_PHY_ADDRESS_PHY_ADDRESS, PHY_BSR, &phyreg);

		} while (((phyreg & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));

	}

	/* Delay to assure PHY configuration */
	HAL_Delay(PHY_CONFIG_DELAY);

	return HAL_OK;

}

uint8_t PHY_GetLinkStatus(ETH_HandleTypeDef *heth)
{
  uint32_t phyreg = 0;
	if(HAL_OK == HAL_ETH_ReadPHYRegister(heth, LAN8720A_PHY_ADDRESS_PHY_ADDRESS, PHY_BSR, &phyreg))
	{
		if((phyreg & PHY_LINKED_STATUS))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}


void PHY_UpdateMAC(ETH_HandleTypeDef *heth)
{
  uint32_t regvalue = 0;
  ETH_MACConfigTypeDef macconf;

	HAL_ETH_GetMACConfig(heth, &macconf);

	HAL_ETH_ReadPHYRegister(heth, LAN8720A_PHY_ADDRESS_PHY_ADDRESS, PHY_SR, &regvalue);

  if((regvalue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
  {
  	macconf.DuplexMode = ETH_FULLDUPLEX_MODE;
  }
  else
  {
  	macconf.DuplexMode = ETH_HALFDUPLEX_MODE;
  }


  if(regvalue & PHY_SPEED_STATUS)
  {
    /* Set Ethernet speed to 10M following the auto-negotiation */
  	macconf.Speed = ETH_SPEED_10M;
  }
  else
  {
    /* Set Ethernet speed to 100M following the auto-negotiation */
  	macconf.Speed = ETH_SPEED_100M;
  }

  HAL_ETH_SetMACConfig(heth, &macconf);

}
