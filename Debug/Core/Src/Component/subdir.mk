################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Component/app_25LC512.c \
../Core/Src/Component/app_i2c_lcd.c \
../Core/Src/Component/app_sim7600.c 

OBJS += \
./Core/Src/Component/app_25LC512.o \
./Core/Src/Component/app_i2c_lcd.o \
./Core/Src/Component/app_sim7600.o 

C_DEPS += \
./Core/Src/Component/app_25LC512.d \
./Core/Src/Component/app_i2c_lcd.d \
./Core/Src/Component/app_sim7600.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Component/%.o: ../Core/Src/Component/%.c Core/Src/Component/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

