################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/ComputeChannel.c \
../Application/ExpansionCards.c \
../Application/ExpansionCards_descriptions.c \
../Application/NSMAC.c \
../Application/UserChar.c \
../Application/UserLabels.c \
../Application/alarms.c \
../Application/backlight.c \
../Application/buzzer.c \
../Application/channels.c \
../Application/cycleEvents.c \
../Application/fram.c \
../Application/historicalData.c \
../Application/measurments.c \
../Application/outputExpansionCards.c \
../Application/parameters.c \
../Application/powerfail.c \
../Application/shift_reg.c \
../Application/watchdog.c 

OBJS += \
./Application/ComputeChannel.o \
./Application/ExpansionCards.o \
./Application/ExpansionCards_descriptions.o \
./Application/NSMAC.o \
./Application/UserChar.o \
./Application/UserLabels.o \
./Application/alarms.o \
./Application/backlight.o \
./Application/buzzer.o \
./Application/channels.o \
./Application/cycleEvents.o \
./Application/fram.o \
./Application/historicalData.o \
./Application/measurments.o \
./Application/outputExpansionCards.o \
./Application/parameters.o \
./Application/powerfail.o \
./Application/shift_reg.o \
./Application/watchdog.o 

C_DEPS += \
./Application/ComputeChannel.d \
./Application/ExpansionCards.d \
./Application/ExpansionCards_descriptions.d \
./Application/NSMAC.d \
./Application/UserChar.d \
./Application/UserLabels.d \
./Application/alarms.d \
./Application/backlight.d \
./Application/buzzer.d \
./Application/channels.d \
./Application/cycleEvents.d \
./Application/fram.d \
./Application/historicalData.d \
./Application/measurments.d \
./Application/outputExpansionCards.d \
./Application/parameters.d \
./Application/powerfail.d \
./Application/shift_reg.d \
./Application/watchdog.d 


# Each subdirectory must supply rules for building sources it contributes
Application/%.o: ../Application/%.c Application/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H753xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Tasks" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Utilities" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP/Common" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP/stmpe811" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Config" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Include" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Lib" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Graphics" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Archiwum" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Eth" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Eth/modbus" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/functions" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/include" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/port" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/rtu" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Settings/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Utilities/Fonts" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Utilities/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Factory/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

