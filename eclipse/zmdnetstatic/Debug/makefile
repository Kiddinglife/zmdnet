################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include test/CMakeFiles/test-system-zmdnet-env.dir/subdir.mk
-include test/subdir.mk
-include src/CMakeFiles/zmdnet.dir/subdir.mk
-include src/CMakeFiles/zmdnet-static.dir/subdir.mk
-include src/subdir.mk
-include sample/CMakeFiles/tzmdnet.dir/subdir.mk
-include sample/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libzmdnetstatic.a

# Tool invocations
libzmdnetstatic.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r  "libzmdnetstatic.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(ARCHIVES)$(OBJS)$(C_DEPS) libzmdnetstatic.a
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
