################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
bootloader.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
bootloader.list \

OBJCOPY_HEX += \
bootloader.hex \

OBJCOPY_BIN += \
bootloader.bin \

OBJCOPY_SREC += \
bootloader.srec \

OBJCOPY_SYMBOLSREC += \
bootloader.symbolsrec \


# All Target
all: bootloader.elf secondary-outputs

# Tool invocations
bootloader.elf: $(OBJS) $(USER_OBJS) D:\git\robot\src\bootloader\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "bootloader.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\git\robot\src\bootloader\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="bootloader.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

bootloader.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "bootloader.list"
	@echo 'Finished building: $@'
	@echo ' '

bootloader.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "bootloader.hex"
	@echo 'Finished building: $@'
	@echo ' '

bootloader.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "bootloader.bin"
	@echo 'Finished building: $@'
	@echo ' '

bootloader.srec: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O srec $(EXECUTABLES) "bootloader.srec"
	@echo 'Finished building: $@'
	@echo ' '

bootloader.symbolsrec: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O symbolsrec $(EXECUTABLES) "bootloader.symbolsrec"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN) $(OBJCOPY_SREC) $(OBJCOPY_SYMBOLSREC)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
