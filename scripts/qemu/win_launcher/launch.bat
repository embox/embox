@echo off
if "%~1"=="" (
	echo.
	echo Usage: launch.bat [path_to_embox_bin]
	echo [path_to_embox_bin] should not end with "embox.bin", just "embox"!
 	echo Qemu will be listening for incoming connection at 127.0.0.1:12345
	echo.
	echo Example: launch.bat d:/embox/build/base/bin/embox
	goto :eof
)

::If you want to start it from explorer, remove the code above
::and replace "%~1" with path to embox.bin at the next line
start "name" /D"%programfiles%\qemu\" qemu.exe -kernel %~1 -net nic,model=ne2k_pci,macaddr=AA:BB:CC:DD:EE:02 -serial tcp:127.0.0.1:12345,server


