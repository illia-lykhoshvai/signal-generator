################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/src/system_stm32f0xx.c 

OBJS += \
./CMSIS/src/system_stm32f0xx.o 

C_DEPS += \
./CMSIS/src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/src/%.o CMSIS/src/%.su: ../CMSIS/src/%.c CMSIS/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F051R8Tx -c -I"D:/LEARNING/stm32f0disc/signal-generator/CMSIS/inc" -I"D:/LEARNING/stm32f0disc/signal-generator/Core/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-CMSIS-2f-src

clean-CMSIS-2f-src:
	-$(RM) ./CMSIS/src/system_stm32f0xx.d ./CMSIS/src/system_stm32f0xx.o ./CMSIS/src/system_stm32f0xx.su

.PHONY: clean-CMSIS-2f-src

