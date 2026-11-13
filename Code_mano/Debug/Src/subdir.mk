################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/bluetooth.c \
../Src/main.c \
../Src/motor.c \
../Src/oled_ssd1306.c \
../Src/sensor_flex.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/bluetooth.o \
./Src/main.o \
./Src/motor.o \
./Src/oled_ssd1306.o \
./Src/sensor_flex.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/bluetooth.d \
./Src/main.d \
./Src/motor.d \
./Src/oled_ssd1306.d \
./Src/sensor_flex.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F401CEUx -DSTM32 -DSTM32F4 -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/bluetooth.cyclo ./Src/bluetooth.d ./Src/bluetooth.o ./Src/bluetooth.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/motor.cyclo ./Src/motor.d ./Src/motor.o ./Src/motor.su ./Src/oled_ssd1306.cyclo ./Src/oled_ssd1306.d ./Src/oled_ssd1306.o ./Src/oled_ssd1306.su ./Src/sensor_flex.cyclo ./Src/sensor_flex.d ./Src/sensor_flex.o ./Src/sensor_flex.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

