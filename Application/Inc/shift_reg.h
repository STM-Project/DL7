/*
 * shift_reg.h
 *
 *  Created on: 09.11.2020
 *      Author: RafalMar
 */

#ifndef INC_SHIFT_REG_H_
#define INC_SHIFT_REG_H_

/* definicje dla rejestru przesuwnego */
#define DCDC0               ((uint16_t)0x8000)  /* power supply board power on (to future use) */
#define DCDC1               ((uint16_t)0x4000)  /* board A power on */
#define DCDC2               ((uint16_t)0x2000)  /* board B power on */
#define DCDC3               ((uint16_t)0x1000)  /* board C power on */
#define DCDC4               ((uint16_t)0x0800)  /* board D power on */
#define DCDC5               ((uint16_t)0x0400)  /* board E power on */
#define DCDC6               ((uint16_t)0x0200)  /* board F power on */
#define DCDC7               ((uint16_t)0x0100)  /* board G power on */

#define LED_BLUE            ((uint16_t)0x0080)  /* blue LED on */
#define LED_RED             ((uint16_t)0x0040)  /* red LED on */
#define USB_EN              ((uint16_t)0x0020)  /* disable USB Power Switch */
#define USB_SW_OE           ((uint16_t)0x0010)  /* disable USB Switch */
#define USB_SW_S            ((uint16_t)0x0008)  /* USB Switch to connect the rear USB socket */
#define SD_PWR              ((uint16_t)0x0004)  /* power off microSD card */
#define ETH_PWR             ((uint16_t)0x0002)  /* enable PHY 8720a  */
#define LCD_BL_EN     		((uint16_t)0x0001)  /* enable LCD backlight converter */


extern uint16_t SHIFT_REG_DATA;

void ShiftReg_Init(void);
void ShiftReg_WriteVal(uint16_t data);

//sygnal REG_STR (stobe) dla rej. 4094
#define SHIFT_REG_STR_OFF 	(REG_STR1_GPIO_Port->BSRR = ((REG_STR1_Pin)<<16))
#define SHIFT_REG_STR_ON  	(REG_STR1_GPIO_Port->BSRR = REG_STR1_Pin)

#define BACKLIGHT_OFF ShiftReg_WriteVal(SHIFT_REG_DATA&=(~LCD_BL_EN))
#define BACKLIGHT_ON  ShiftReg_WriteVal(SHIFT_REG_DATA|=(LCD_BL_EN))

//sygnal REG_OE dla rej. 4094
#define SHIFT_REG_OE_OFF 	(REG_OE_GPIO_Port->BSRR = ((REG_OE_Pin)<<16))
#define SHIFT_REG_OE_ON  	(REG_OE_GPIO_Port->BSRR = REG_OE_Pin)

#define BLUE_LED_OFF 		ShiftReg_WriteVal(SHIFT_REG_DATA&=(~LED_BLUE))
#define BLUE_LED_ON  		ShiftReg_WriteVal(SHIFT_REG_DATA|=(LED_BLUE))
#define BLUE_LED_TOGGLE   	ShiftReg_WriteVal(SHIFT_REG_DATA^=(1 << 7))

#define RED_LED_OFF 		ShiftReg_WriteVal(SHIFT_REG_DATA&=(~LED_RED))
#define RED_LED_ON  		ShiftReg_WriteVal(SHIFT_REG_DATA|=(LED_RED))
#define RED_LED_TOGGLE   	ShiftReg_WriteVal(SHIFT_REG_DATA^=(1 << 6))

#define SD_PWR_OFF 		ShiftReg_WriteVal(SHIFT_REG_DATA&=(~SD_PWR))
#define SD_PWR_ON		ShiftReg_WriteVal(SHIFT_REG_DATA|=(SD_PWR))

#define USB_PWR_OFF 		ShiftReg_WriteVal(SHIFT_REG_DATA|=(USB_EN))
#define USB_PWR_ON  		ShiftReg_WriteVal(SHIFT_REG_DATA&=(~USB_EN))

#define USB_SW_OFF 			ShiftReg_WriteVal(SHIFT_REG_DATA|=(USB_SW_OE))
#define USB_SW_ON  			ShiftReg_WriteVal(SHIFT_REG_DATA&=(~USB_SW_OE))

#define USB_BACK 			ShiftReg_WriteVal(SHIFT_REG_DATA|=(USB_SW_S))
#define USB_FRONT  			ShiftReg_WriteVal(SHIFT_REG_DATA&=(~USB_SW_S))
#define USB_FRONT_BACK_TOGGLE   	ShiftReg_WriteVal(SHIFT_REG_DATA^=(1 << 3))

//RS485_OUT_OFF oraz RS485_OUT_ON do w³¹czenie/wy³aczenia zasilania modu³u RS485 na p³ytce zasilacza
#define RS485_OUT_OFF 		ShiftReg_WriteVal(SHIFT_REG_DATA&=(~DCDC0))
#define RS485_OUT_ON  		ShiftReg_WriteVal(SHIFT_REG_DATA|=(DCDC0))

#define IOBOARD_PWR_ON(x)  	ShiftReg_WriteVal(SHIFT_REG_DATA|=(((uint16_t)0x4000)>> (x)))
#define IOBOARD_PWR_OFF(x) 	ShiftReg_WriteVal(SHIFT_REG_DATA&=(~((uint16_t)0x4000)>> (x)))

#define EthPhyOff 			ShiftReg_WriteVal(SHIFT_REG_DATA&=(~ETH_PWR))
#define EthPhyOn 			ShiftReg_WriteVal(SHIFT_REG_DATA|=(ETH_PWR))

#endif /* INC_SHIFT_REG_H_ */
