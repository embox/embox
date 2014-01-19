qemu-system-microblaze -nographic -M petalogix-s3adsp1800 -kernel ../../build/base/bin/embox -net tap,name=tap0,script=start_script,downscript=stop_script
