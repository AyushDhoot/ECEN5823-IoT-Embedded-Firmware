################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../platform/emdrv/sleep/src/sleep.c 

OBJS += \
./platform/emdrv/sleep/src/sleep.o 

C_DEPS += \
./platform/emdrv/sleep/src/sleep.d 


# Each subdirectory must supply rules for building sources it contributes
platform/emdrv/sleep/src/sleep.o: ../platform/emdrv/sleep/src/sleep.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DEFR32BG13P632F512GM48=1' '-D__STACK_SIZE=0x800' '-DHAL_CONFIG=1' '-D__StackLimit=0x20000000' '-D__HEAP_SIZE=0xD00' -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\hardware\kit\common\halconfig" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\Device\SiliconLabs\EFR32BG13P\Include" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\hardware\kit\common\drivers" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\emlib\inc" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\CMSIS\Include" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\emlib\src" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\protocol\bluetooth\ble_stack\inc\common" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\hardware\kit\EFR32BG13_BRD4104A\config" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\hardware\kit\common\bsp" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\radio\rail_lib\common" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\emdrv\uartdrv\inc" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\halconfig\inc\hal-config" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\Device\SiliconLabs\EFR32BG13P\Source" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\bootloader\api" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\protocol\bluetooth\ble_stack\inc\soc" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\emdrv\sleep\src" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\emdrv\sleep\inc" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\emdrv\common\inc" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\app\bluetooth\common\stack_bridge" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\radio\rail_lib\chip\efr32" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\Device\SiliconLabs\EFR32BG13P\Source\GCC" -I"C:\Users\Ayush\SimplicityStudio\v4_workspace\soc-thermometer_2 - CLIENT\platform\bootloader" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.3//util/silicon_labs/silabs_core/graphics" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.3//platform/middleware/glib/dmd" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.3//platform/middleware/glib/glib" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.3//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.3//platform/middleware/glib" -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"platform/emdrv/sleep/src/sleep.d" -MT"platform/emdrv/sleep/src/sleep.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


