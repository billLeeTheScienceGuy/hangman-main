################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/%.obj: ../FreeRTOS/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/ccs1110/ccs/ccs_base/arm/include" --include_path="C:/ti/ccs1110/ccs/ccs_base/arm/include/CMSIS" --include_path="C:/Users/byeon/OneDrive/Desktop/ECE353/FreeRTOSv202112.00/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/byeon/OneDrive/Desktop/ECE353/FreeRTOSv202112.00/FreeRTOS-Plus/Source/AWS/ota/test/cbmc/FreeRTOS-Kernel/include" --include_path="C:/Users/byeon/OneDrive/Desktop/ECE353/hangman-main" --include_path="C:/ti/ccs1110/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


