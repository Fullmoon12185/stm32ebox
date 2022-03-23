################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ServerCommunication/app_http.c 

OBJS += \
./Core/Src/ServerCommunication/app_http.o 

C_DEPS += \
./Core/Src/ServerCommunication/app_http.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/ServerCommunication/%.o: ../Core/Src/ServerCommunication/%.c Core/Src/ServerCommunication/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-ServerCommunication

clean-Core-2f-Src-2f-ServerCommunication:
	-$(RM) ./Core/Src/ServerCommunication/app_http.d ./Core/Src/ServerCommunication/app_http.o

.PHONY: clean-Core-2f-Src-2f-ServerCommunication

