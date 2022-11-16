################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/src/main.c 

OBJS += \
./Core/src/main.o 

C_DEPS += \
./Core/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Core/src/%.o Core/src/%.su: ../Core/src/%.c Core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F051R8Tx -c -I"D:/LEARNING/stm32f0disc/stm32f0-dac-1wave-sin/CMSIS/inc" -I"D:/LEARNING/stm32f0disc/stm32f0-dac-1wave-sin/Core/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-src

clean-Core-2f-src:
	-$(RM) ./Core/src/main.d ./Core/src/main.o ./Core/src/main.su

.PHONY: clean-Core-2f-src

