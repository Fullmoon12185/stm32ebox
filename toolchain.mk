TOOLCHAIN_PATH = C:/ST/STM32CubeIDE_1.7.0/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346/tools

#######################################
# binaries
#######################################
GCC_BINARY_PATH = $(TOOLCHAIN_PATH)/bin
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifeq ($(OS),Windows_NT)
	CC = $(GCC_BINARY_PATH)/$(PREFIX)gcc.exe
	AS = $(GCC_BINARY_PATH)/$(PREFIX)gcc.exe -x assembler-with-cpp
	CP = $(GCC_BINARY_PATH)/$(PREFIX)objcopy.exe
	SZ = $(GCC_BINARY_PATH)/$(PREFIX)size.exe
else
	CC = $(GCC_BINARY_PATH)/$(PREFIX)gcc
	AS = $(GCC_BINARY_PATH)/$(PREFIX)gcc -x assembler-with-cpp
	CP = $(GCC_BINARY_PATH)/$(PREFIX)objcopy
	SZ = $(GCC_BINARY_PATH)/$(PREFIX)size
endif

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S


#Include
GCC_INCLUDE_PATH = -I$(TOOLCHAIN_PATH)/arm-none-eabi/include \
-I$(TOOLCHAIN_PATH)/lib/gcc/arm-none-eabi/9.3.1/include \
-I$(TOOLCHAIN_PATH)/lib/gcc/arm-none-eabi/9.3.1/include-fixed \

#Lib