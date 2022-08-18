################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/GUI/Settings/SetArchiveDLG.c \
../Application/GUI/Settings/SetChannelDLG.c \
../Application/GUI/Settings/SetCommunicationDLG.c \
../Application/GUI/Settings/SetConfirmDLG.c \
../Application/GUI/Settings/SetEmailClientDLG.c \
../Application/GUI/Settings/SetFirmwareUpdateDLG.c \
../Application/GUI/Settings/SetGeneralDLG.c \
../Application/GUI/Settings/SetIOBoardsDLG.c \
../Application/GUI/Settings/SetLoginDLG.c \
../Application/GUI/Settings/SetModbusTCPClientDLG.c \
../Application/GUI/Settings/SetUSBDLG.c \
../Application/GUI/Settings/SetUserCharDLG.c \
../Application/GUI/Settings/SetUserScreensDLG.c \
../Application/GUI/Settings/SettingsDLG.c 

OBJS += \
./Application/GUI/Settings/SetArchiveDLG.o \
./Application/GUI/Settings/SetChannelDLG.o \
./Application/GUI/Settings/SetCommunicationDLG.o \
./Application/GUI/Settings/SetConfirmDLG.o \
./Application/GUI/Settings/SetEmailClientDLG.o \
./Application/GUI/Settings/SetFirmwareUpdateDLG.o \
./Application/GUI/Settings/SetGeneralDLG.o \
./Application/GUI/Settings/SetIOBoardsDLG.o \
./Application/GUI/Settings/SetLoginDLG.o \
./Application/GUI/Settings/SetModbusTCPClientDLG.o \
./Application/GUI/Settings/SetUSBDLG.o \
./Application/GUI/Settings/SetUserCharDLG.o \
./Application/GUI/Settings/SetUserScreensDLG.o \
./Application/GUI/Settings/SettingsDLG.o 

C_DEPS += \
./Application/GUI/Settings/SetArchiveDLG.d \
./Application/GUI/Settings/SetChannelDLG.d \
./Application/GUI/Settings/SetCommunicationDLG.d \
./Application/GUI/Settings/SetConfirmDLG.d \
./Application/GUI/Settings/SetEmailClientDLG.d \
./Application/GUI/Settings/SetFirmwareUpdateDLG.d \
./Application/GUI/Settings/SetGeneralDLG.d \
./Application/GUI/Settings/SetIOBoardsDLG.d \
./Application/GUI/Settings/SetLoginDLG.d \
./Application/GUI/Settings/SetModbusTCPClientDLG.d \
./Application/GUI/Settings/SetUSBDLG.d \
./Application/GUI/Settings/SetUserCharDLG.d \
./Application/GUI/Settings/SetUserScreensDLG.d \
./Application/GUI/Settings/SettingsDLG.d 


# Each subdirectory must supply rules for building sources it contributes
Application/GUI/Settings/%.o: ../Application/GUI/Settings/%.c Application/GUI/Settings/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H753xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Tasks" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Utilities" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP/Common" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP/stmpe811" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Config" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Include" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Lib" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Graphics" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Archiwum" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Eth" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Eth/modbus" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/functions" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/include" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/port" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/rtu" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Settings/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Utilities/Fonts" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Utilities/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Factory/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

