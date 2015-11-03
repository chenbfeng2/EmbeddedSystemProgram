################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../\ ex_2lock_try.c 

OBJS += \
./\ ex_2lock_try.o 

C_DEPS += \
./\ ex_2lock_try.d 


# Each subdirectory must supply rules for building sources it contributes
\ ex_2lock_try.o: ../\ ex_2lock_try.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF" ex_2lock_try.d" -MT"\ ex_2lock_try.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


