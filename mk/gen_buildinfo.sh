#!/usr/bin/env bash
export LC_ALL=C

command -v date >/dev/null 2>&1 || { echo >&2 "Warning: command 'date' not found, skip generating buildinfo."; exit 0; }
command -v hexdump >/dev/null 2>&1 || { echo >&2 "Warning: command 'hexdump' not found, skip generating buildinfo."; exit 0; }

date | hexdump -v -e '"BYTE(0x" 1/1 "%02X" ")\n"'
echo "BYTE(0x00)"
