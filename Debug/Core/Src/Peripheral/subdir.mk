################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Peripheral/app_gpio.c \
../Core/Src/Peripheral/app_i2c.c \
../Core/Src/Peripheral/app_spi.c \
../Core/Src/Peripheral/app_system_clock_config.c \
../Core/Src/Peripheral/app_timer.c \
../Core/Src/Peripheral/app_uart.c 

OBJS += \
./Core/Src/Peripheral/app_gpio.o \
./Core/Src/Peripheral/app_i2c.o \
./Core/Src/Peripheral/app_spi.o \
./Core/Src/Peripheral/app_system_clock_config.o \
./Core/Src/Peripheral/app_timer.o \
./Core/Src/Peripheral/app_uart.o 

C_DEPS += \
./Core/Src/Peripheral/app_gpio.d \
./Core/Src/Peripheral/app_i2c.d \
./Core/Src/Peripheral/app_spi.d \
./Core/Src/Peripheral/app_system_clock_config.d \
./Core/Src/Peripheral/app_timer.d \
./Core/Src/Peripheral/app_uart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Peripheral/%.o: ../Core/Src/Peripheral/%.c Core/Src/Peripheral/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Peripheral

clean-Core-2f-Src-2f-Peripheral:
	-$(RM) ./Core/Src/Peripheral/app_gpio.d ./Core/Src/Peripheral/app_gpio.o ./Core/Src/Peripheral/app_i2c.d ./Core/Src/Peripheral/app_i2c.o ./Core/Src/Peripheral/app_spi.d ./Core/Src/Peripheral/app_spi.o ./Core/Src/Peripheral/app_system_clock_config.d ./Core/Src/Peripheral/app_system_clock_config.o ./Core/Src/Peripheral/app_timer.d ./Core/Src/Peripheral/app_timer.o ./Core/Src/Peripheral/app_uart.d ./Core/Src/Peripheral/app_uart.o

.PHONY: clean-Core-2f-Src-2f-Peripheral

