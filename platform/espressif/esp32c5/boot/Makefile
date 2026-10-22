# Standalone loader: run from the repository root with make -f <this file>.
boot_dir := $(dir $(lastword $(MAKEFILE_LIST)))
CROSS_COMPILE ?= riscv64-unknown-elf-
OUTPUT_DIR ?= build/base/bin
ESPTOOL ?= esptool

boot_flags := -march=rv32imc_zicsr_zifencei -mabi=ilp32 -Os -g \
	-ffreestanding -fno-builtin -fno-stack-protector -fno-pic -fno-pie \
	-msmall-data-limit=0 -mno-relax -Wall -Wextra -Werror

.PHONY: all
all: $(OUTPUT_DIR)/bootloader.bin

$(OUTPUT_DIR):
	mkdir -p $@

$(OUTPUT_DIR)/bootloader.elf: $(boot_dir)entry.S $(boot_dir)boot.c \
		$(boot_dir)boot.lds.S $(boot_dir)Makefile | $(OUTPUT_DIR)
	$(CROSS_COMPILE)gcc $(boot_flags) -nostdlib -static \
		-Wl,--no-relax,--build-id=none,-T,$(boot_dir)boot.lds.S \
		$(boot_dir)entry.S $(boot_dir)boot.c -o $@

$(OUTPUT_DIR)/bootloader.bin: $(OUTPUT_DIR)/bootloader.elf
	$(ESPTOOL) --chip esp32c5 elf2image --flash-mode dio \
		--flash-freq 20m --flash-size 2MB -o $@ $<
