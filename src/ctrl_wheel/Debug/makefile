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
ctrl_wheel.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
ctrl_wheel.list \

OBJCOPY_HEX += \
ctrl_wheel.hex \

OBJCOPY_BIN += \
ctrl_wheel.bin \

OBJCOPY_SREC += \
ctrl_wheel.srec \

OBJCOPY_SYMBOLSREC += \
ctrl_wheel.symbolsrec \


# All Target
all: ctrl_wheel.elf secondary-outputs

# Tool invocations
ctrl_wheel.elf: $(OBJS) $(USER_OBJS) D:\git\robot\src\ctrl_wheel\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "ctrl_wheel.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\git\robot\src\ctrl_wheel\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="ctrl_wheel.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

ctrl_wheel.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "ctrl_wheel.list"
	@echo 'Finished building: $@'
	@echo ' '

ctrl_wheel.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "ctrl_wheel.hex"
	@echo 'Finished building: $@'
	@echo ' '

ctrl_wheel.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "ctrl_wheel.bin"
	@echo 'Finished building: $@'
	@echo ' '

ctrl_wheel.srec: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O srec $(EXECUTABLES) "ctrl_wheel.srec"
	@echo 'Finished building: $@'
	@echo ' '

ctrl_wheel.symbolsrec: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O symbolsrec $(EXECUTABLES) "ctrl_wheel.symbolsrec"
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
