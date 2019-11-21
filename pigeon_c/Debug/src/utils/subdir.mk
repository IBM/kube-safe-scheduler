################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/utils/array.c \
../src/utils/hashmap.c \
../src/utils/hashset.c \
../src/utils/integer.c \
../src/utils/maparray.c \
../src/utils/mathhelper.c \
../src/utils/matrix.c \
../src/utils/printhelper.c \
../src/utils/queue.c \
../src/utils/stringbuffer.c \
../src/utils/vector.c 

OBJS += \
./src/utils/array.o \
./src/utils/hashmap.o \
./src/utils/hashset.o \
./src/utils/integer.o \
./src/utils/maparray.o \
./src/utils/mathhelper.o \
./src/utils/matrix.o \
./src/utils/printhelper.o \
./src/utils/queue.o \
./src/utils/stringbuffer.o \
./src/utils/vector.o 

C_DEPS += \
./src/utils/array.d \
./src/utils/hashmap.d \
./src/utils/hashset.d \
./src/utils/integer.d \
./src/utils/maparray.d \
./src/utils/mathhelper.d \
./src/utils/matrix.d \
./src/utils/printhelper.d \
./src/utils/queue.d \
./src/utils/stringbuffer.d \
./src/utils/vector.d 


# Each subdirectory must supply rules for building sources it contributes
src/utils/%.o: ../src/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DDEBUG -O3 -g3 -Wall -c -fmessage-length=0 -std=c99 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


