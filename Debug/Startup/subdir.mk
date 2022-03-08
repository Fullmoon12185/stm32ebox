################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Startup/startup_stm32f103rbtx.s 

OBJS += \
./Startup/startup_stm32f103rbtx.o 

S_DEPS += \
./Startup/startup_stm32f103rbtx.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/startup_stm32f103rbtx.o: ../Startup/startup_stm32f103rbtx.s
	arm-none-eabi-gcc -mcpu=cortex-m3 -g3 -c -I../ -x assembler-with-cpp -MMD -MP -MF"Startup/startup_stm32f103rbtx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

