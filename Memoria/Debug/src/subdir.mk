################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Configuracion.c \
../src/Memoria.c \
../src/cache.c \
../src/console.c \
../src/operations.c \
../src/server.c 

OBJS += \
./src/Configuracion.o \
./src/Memoria.o \
./src/cache.o \
./src/console.o \
./src/operations.o \
./src/server.o 

C_DEPS += \
./src/Configuracion.d \
./src/Memoria.d \
./src/cache.d \
./src/console.d \
./src/operations.d \
./src/server.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/git/tp-2017-1c-System-Buzz/Common" -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


