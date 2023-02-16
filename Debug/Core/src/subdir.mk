################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/src/hw.c \
../Core/src/interaction.c \
../Core/src/llist.c \
../Core/src/main.c \
../Core/src/protocol.c 

OBJS += \
./Core/src/hw.o \
./Core/src/interaction.o \
./Core/src/llist.o \
./Core/src/main.o \
./Core/src/protocol.o 

C_DEPS += \
./Core/src/hw.d \
./Core/src/interaction.d \
./Core/src/llist.d \
./Core/src/main.d \
./Core/src/protocol.d 


# Each subdirectory must supply rules for building sources it contributes
Core/src/%.o Core/src/%.su: ../Core/src/%.c Core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DSTM32F051R8Tx -c -I"D:/LEARNING/stm32f0disc/signal-generator/CMSIS/inc" -I"D:/LEARNING/stm32f0disc/signal-generator/Core/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-src

clean-Core-2f-src:
	-$(RM) ./Core/src/hw.d ./Core/src/hw.o ./Core/src/hw.su ./Core/src/interaction.d ./Core/src/interaction.o ./Core/src/interaction.su ./Core/src/llist.d ./Core/src/llist.o ./Core/src/llist.su ./Core/src/main.d ./Core/src/main.o ./Core/src/main.su ./Core/src/protocol.d ./Core/src/protocol.o ./Core/src/protocol.su

.PHONY: clean-Core-2f-src

