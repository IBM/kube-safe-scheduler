################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/objective/abpfn.c \
../src/objective/commonobj.c \
../src/objective/consfn.c \
../src/objective/lbfn.c \
../src/objective/objfn.c 

OBJS += \
./src/objective/abpfn.o \
./src/objective/commonobj.o \
./src/objective/consfn.o \
./src/objective/lbfn.o \
./src/objective/objfn.o 

C_DEPS += \
./src/objective/abpfn.d \
./src/objective/commonobj.d \
./src/objective/consfn.d \
./src/objective/lbfn.d \
./src/objective/objfn.d 


# Each subdirectory must supply rules for building sources it contributes
src/objective/%.o: ../src/objective/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -DDEBUG -O3 -g3 -Wall -c -fmessage-length=0 -std=c99 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


