################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/crc.c \
../Core/Src/dma.c \
../Core/Src/dma2d.c \
../Core/Src/fmc.c \
../Core/Src/freertos.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/iwdg.c \
../Core/Src/ltdc.c \
../Core/Src/main.c \
../Core/Src/mdma.c \
../Core/Src/rng.c \
../Core/Src/rtc.c \
../Core/Src/sdmmc.c \
../Core/Src/spi.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_hal_timebase_tim.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/crc.o \
./Core/Src/dma.o \
./Core/Src/dma2d.o \
./Core/Src/fmc.o \
./Core/Src/freertos.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/iwdg.o \
./Core/Src/ltdc.o \
./Core/Src/main.o \
./Core/Src/mdma.o \
./Core/Src/rng.o \
./Core/Src/rtc.o \
./Core/Src/sdmmc.o \
./Core/Src/spi.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_hal_timebase_tim.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/crc.d \
./Core/Src/dma.d \
./Core/Src/dma2d.d \
./Core/Src/fmc.d \
./Core/Src/freertos.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/iwdg.d \
./Core/Src/ltdc.d \
./Core/Src/main.d \
./Core/Src/mdma.d \
./Core/Src/rng.d \
./Core/Src/rtc.d \
./Core/Src/sdmmc.d \
./Core/Src/spi.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_hal_timebase_tim.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32H753xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/compat/posix -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/net -I../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -I../Middlewares/Third_Party/LwIP/src/include/compat/stdc -I../Middlewares/Third_Party/LwIP/system/arch -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Tasks" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Utilities" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP/Common" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/BSP/stmpe811" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Config" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Include" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Middlewares/ST/StemWin/Lib" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Graphics" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Archiwum" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Eth" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/Eth/modbus" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/functions" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/include" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/port" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/modbus/rtu" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Settings/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Utilities/Fonts" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Utilities/Inc" -I"D:/ARM_PROJECTS_H7/DL7/DL7/Application/GUI/Factory/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

