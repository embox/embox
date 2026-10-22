# Запуск на ESP32-C5

## Сборка

Нужны bare-metal RISC-V toolchain с поддержкой RV32/ILP32, GDB,
Espressif OpenOCD с поддержкой C5 и esptool (проверено с v5.3.1).
Исполняемые файлы должны быть доступны через `PATH`.

Команды выполняются из корня Embox.

```sh
make confload-platform/espressif/esp32c5
make -jN

make -f platform/espressif/esp32c5/boot/Makefile \
    CROSS_COMPILE=riscv32-esp-elf-

python3 scripts/espressif/esp32c5_image.py \
    build/base/bin/embox build/base/bin/embox.xip.bin

esptool --chip esp32c5 image-info build/base/bin/bootloader.bin
```

Ожидаются один RAM-сегмент по адресу `0x40840000`, точка входа в этом
сегменте и `valid` для checksum/hash загрузочной части.
Загрузчик и приложение линкуются независимо. Вход приложения должен быть
`0x42000000`; упаковщик проверяет это условие.

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
monitor program_esp /absolute/path/to/embox/build/base/bin/bootloader.bin 0x2000 verify
monitor program_esp /absolute/path/to/embox/build/base/bin/embox.xip.bin 0x10000 verify
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
`embox.xip.bin`. Для отладки загрузчика используй отдельный
`bootloader.elf`. Обычный `embox.bin` не подходит для прошивки.
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

При циклических сбросах проверь строку ROM `rst:`. Причина
`TG0_WDT_HPSYS` означает watchdog Timer Group 0. Загрузчик отключает его,
включая flash-boot mode, отдельно от LP watchdog. После изменения этого
кода пересобери и перепрошей `bootloader.bin` на `0x2000`.

## Проверка передачи SRAM приложению

После перехода в `_start` загрузчик больше не вызывается. Embox использует
`0x40800000`–`0x40860000` (384 КиБ); вызовы ROM после передачи управления
не допускаются, поскольку её рабочая память может быть перезаписана.

При проверке новой схемы остановись в `_start` и выполни инструкции до
установки `sp`. Убедись, что стек сменился до записи в BSS или `.data`,
затем проверь достижение `kernel_start`, диагностический вывод и содержимое
`.data`. Для проверки верхней SRAM используй конфигурацию, которая реально
размещает данные или стек выше `0x40840000`. Новая схема требует проверки
на плате; успешная сборка не подтверждает работоспособность освобождённой RAM.
