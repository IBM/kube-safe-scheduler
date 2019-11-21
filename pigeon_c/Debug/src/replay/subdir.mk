################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/replay/parser.c \
../src/replay/properties.c \
../src/replay/reader.c 

OBJS += \
./src/replay/parser.o \
./src/replay/properties.o \
./src/replay/reader.o 

C_DEPS += \
./src/replay/parser.d \
./src/replay/properties.d \
./src/replay/reader.d 


# Each subdirectory must supply rules for building sources it contributes
src/replay/%.o: ../src/replay/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DDEBUG -O3 -g3 -Wall -c -fmessage-length=0 -std=c99 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


