start "name" /D"c:\Program Files\qemu\" qemu.exe -kernel d:/projects/embox/build/base/bin/embox -net nic,model=ne2k_pci,macaddr=AA:BB:CC:DD:EE:02 -serial tcp:127.0.0.1:12345,server
