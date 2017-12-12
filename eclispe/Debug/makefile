################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include test/CMakeFiles/test-system-zmdnet-env.dir/subdir.mk
-include test/subdir.mk
-include src/system/subdir.mk
-include src/distribution/auth/subdir.mk
-include src/adaption/subdir.mk
-include src/CMakeFiles/zmdnet.dir/distribution/auth/subdir.mk
-include src/CMakeFiles/zmdnet.dir/adaption/subdir.mk
-include src/CMakeFiles/zmdnet-static.dir/system/subdir.mk
-include src/CMakeFiles/zmdnet-static.dir/distribution/auth/subdir.mk
-include src/CMakeFiles/zmdnet-static.dir/adaption/subdir.mk
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
all: libmakefile-static-lib.a

# Tool invocations
libmakefile-static-lib.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r  "libmakefile-static-lib.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(ARCHIVES)$(OBJS)$(C_DEPS) libmakefile-static-lib.a
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
