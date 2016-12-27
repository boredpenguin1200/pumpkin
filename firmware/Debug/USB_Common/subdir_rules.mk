################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
USB_Common/usb.obj: ../USB_Common/usb.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/chrisn/software/ccsv7/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/chrisn/software/ccsv7/ccsv7/ccs_base/msp430/include" --include_path="/home/chrisn/Documents/myRepos/pumpkin/firmware/USB_CDC_API" --include_path="/home/chrisn/Documents/myRepos/pumpkin/firmware/USB_Common" --include_path="/home/chrisn/software/ti/msp/MSP430Ware_3_60_00_10/usblib430/MSP430_USB_Software/MSP430_USB_API/USB_API/USB_Common" --include_path="/home/chrisn/Documents/myRepos/pumpkin/firmware/USB_config" --include_path="/home/chrisn/software/ti/msp/MSP430Ware_3_60_00_10/usblib430/MSP430_USB_Software/MSP430_USB_API" --include_path="/home/chrisn/Documents/myRepos/pumpkin/firmware/driverlib/MSP430F5xx_6xx" --include_path="/home/chrisn/software/ccsv7/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --advice:power=all -g --define=__MSP430F5528__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="USB_Common/usb.d" --obj_directory="USB_Common" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

USB_Common/usbdma.obj: ../USB_Common/usbdma.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/chrisn/software/ccsv7/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="/home/chrisn/software/ccsv7/ccsv7/ccs_base/msp430/include" --include_path="/home/chrisn/Documents/myRepos/pumpkin/firmware/USB_CDC_API" --include_path="/home/chrisn/Documents/myRepos/pumpkin/firmware/USB_Common" --include_path="/home/chrisn/software/ti/msp/MSP430Ware_3_60_00_10/usblib430/MSP430_USB_Software/MSP430_USB_API/USB_API/USB_Common" --include_path="/home/chrisn/Documents/myRepos/pumpkin/firmware/USB_config" --include_path="/home/chrisn/software/ti/msp/MSP430Ware_3_60_00_10/usblib430/MSP430_USB_Software/MSP430_USB_API" --include_path="/home/chrisn/Documents/myRepos/pumpkin/firmware/driverlib/MSP430F5xx_6xx" --include_path="/home/chrisn/software/ccsv7/ccsv7/tools/compiler/ti-cgt-msp430_16.9.0.LTS/include" --advice:power=all -g --define=__MSP430F5528__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="USB_Common/usbdma.d" --obj_directory="USB_Common" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


