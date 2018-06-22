
**** This config demonstarates USB device and host examples imported from STM32 Cube.

Compile and run one of the two following examples:

1. USB DEVICE example (include stm32f7.cmd.usb_device_example)

	1. Build Embox.
	
	2. # ./scripts/stm32/import_stm32_cube_example.py --name usb_device_example --platform f7 ./build/extbld/third_party/bsp/stmf7cube/core/STM32Cube_FW_F7_V1.5.0/Projects/STM32746G-Discovery/Applications/USB_Device/MSC_Standalone
	
	After Execution of this command, the folder 'platform/stm32f7/cmds/usb_device_example' will be created with corresponding sources imported from Cube.
	Read usb_device_example/readme.txt to figure out what this example should do.
	
	3. Now you should to enable USE_USB_HS or USE_USB_FS as it is stated in usb_device_example/readme.txt. To do this, modify platform/stm32f7/cmds/usb_device_example/Src/usbd_conf.c: Insert '#define USE_USB_HS' at the beginning of the file.
	
	4. Uncomment module 'stm32f7.cmd.usb_device_example' in mods.config and rebuild Embox.
	
	Type 'usb_device_example' in Embox shell to launch the imported example.

2. USB HOST example (include stm32f7.cmd.usb_host_example)

	1. Build Embox.
	
	2. # ./scripts/stm32/import_stm32_cube_example.py --name usb_host_example --platform f7 ./build/extbld/third_party/bsp/stmf7cube/core/STM32Cube_FW_F7_V1.5.0/Projects/STM32746G-Discovery/Applications/USB_Host/MSC_Standalone
	
	After Execution of this command, the folder 'platform/stm32f7/cmds/usb_host_example' will be created with corresponding sources imported from Cube.
	Read usb_host_example/readme.txt to figure out what this example should do.
	
	3. Now you should to enable USE_USB_HS or USE_USB_FS as it is stated in usb_host_example/readme.txt. To do this, modify platform/stm32f7/cmds/usb_host_example/Src/usbh_conf.c: Insert '#define USE_USB_HS' at the beginning of the file.
	
	4. Uncomment module 'stm32f7.cmd.usb_host_example' in mods.config and rebuild Embox.
	
	Type 'usb_host_example' in Embox shell to launch the imported example.
