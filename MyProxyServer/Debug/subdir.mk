################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../error.c \
../generate.c \
../http.c \
../main.c \
../reception.c \
../tcp.c \
../util.c 

OBJS += \
./error.o \
./generate.o \
./http.o \
./main.o \
./reception.o \
./tcp.o \
./util.o 

C_DEPS += \
./error.d \
./generate.d \
./http.d \
./main.d \
./reception.d \
./tcp.d \
./util.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


