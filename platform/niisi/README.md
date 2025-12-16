## Baget_PLC1.01 board
## Build
```bash
make confload-platform/niisi/baget_plc1 
make
```
## Prepare SD card
copy platform/niisi/barebox.sh and build/base/bin/embox.bin to boot/barebox/ folder on SD card

## QEMU

### vk01
```bash
make confload-platform/niisi/vk01_qemu 
make
```

### vm10
```bash
make confload-platform/niisi/vm10_qemu 
make
```

### Run qemu

```
AUTOQEMU_PREFIX=<DUNA QEMU DIR>/build/ ./scripts/qemu/auto_qemu
```
