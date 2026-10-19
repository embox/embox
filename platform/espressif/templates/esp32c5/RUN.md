# Запуск на ESP32-C5

## Сборка

Нужны bare-metal RISC-V toolchain с поддержкой RV32/ILP32, GDB,
Espressif OpenOCD с поддержкой C5 и esptool (проверено с v5.3.1).
Исполняемые файлы должны быть доступны через `PATH`.

Команды выполняются из корня Embox.

```sh
make confload-platform/espressif/esp32c5
make -jN

python3 scripts/espressif/esp32c5_image.py \
    build/base/bin/embox build/base/bin/embox.flash.bin \
    --objcopy riscv64-elf-objcopy

esptool --chip esp32c5 image-info build/base/bin/embox.flash.bin
```

Ожидаются один RAM-сегмент по адресу `0x40840000`, точка входа в этом
сегменте и `valid` для checksum/hash загрузочной части.
Предупреждения objcopy об empty loadable segment при упаковке ожидаемы.

## OpenOCD

Подключи разъём **USB** платы для встроенного JTAG и запусти:

```sh
sudo openocd -f board/esp32c5-builtin.cfg
```

Оставь OpenOCD работающим. Ожидается GDB-сервер на порту `3333`.

## Прошивка и остановка на входе

В другом терминале, из корня Embox:

```sh
riscv32-elf-gdb build/base/bin/embox
```

В GDB выполни следующие команды. Вместо `/absolute/path/to/embox` укажи
абсолютный путь к репозиторию: файл открывает процесс OpenOCD, поэтому
относительный путь может считаться от другого каталога.

```gdb
target extended-remote localhost:3333
monitor reset halt
monitor program_esp /absolute/path/to/embox/build/base/bin/embox.flash.bin 0x2000 verify
```

Запись заменяет существующий загрузчик и перекрываемые части flash.
После `Verify OK`:

```gdb
monitor reset halt
hbreak _start
continue
```

Ожидается остановка по адресу `0x42000000`. Для перехода к старту ядра:

```gdb
delete breakpoints
hbreak kernel_start
continue
```

Для пошаговой отладки используй `step` и `next`. Если выполнение зависло,
нажми Ctrl+C и посмотри регистры:

```gdb
info registers pc mcause mepc mtval
```

Для символов используется ELF `embox` от той же сборки, что и прошитый
`embox.flash.bin`. Обычный `embox.bin` не подходит для прошивки.
Команда GDB `load` здесь не используется.

Предупреждения OpenOCD о FreeRTOS и неизвестной таблице разделов ожидаемы:
в этом образе их нет. Для точек останова используется аппаратный `hbreak`.

## UART

Подключи второй разъём платы, **UART** (мост CH343P). Найди устройство:

```sh
minicom -D /dev/ttyACM1 -b 115200
```

Выбери фактическое имя порта. Настройки: 115200, 8N1,
аппаратный и программный flow control выключены.
Пока CPU остановлен в GDB, нового вывода не будет.
