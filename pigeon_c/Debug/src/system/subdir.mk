################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/system/cloudsystem.c \
../src/system/defs.c \
../src/system/le.c \
../src/system/pe.c \
../src/system/resource.c \
../src/system/stateupdater.c 

OBJS += \
./src/system/cloudsystem.o \
./src/system/defs.o \
./src/system/le.o \
./src/system/pe.o \
./src/system/resource.o \
./src/system/stateupdater.o 

C_DEPS += \
./src/system/cloudsystem.d \
./src/system/defs.d \
./src/system/le.d \
./src/system/pe.d \
./src/system/resource.d \
./src/system/stateupdater.d 


# Each subdirectory must supply rules for building sources it contributes
src/system/%.o: ../src/system/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DDEBUG -O3 -g3 -Wall -c -fmessage-length=0 -std=c99 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


