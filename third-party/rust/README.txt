## Подготовка к использованию

## Для использования Rust в Embox необходимо:
1. Установить свежую версию Rust: https://www.rust-lang.org/ru/tools/install
    1.1. Установить Rust-nightly: rustup install nightly
    1.2. Установить target i686-unknown-linux-gnu: rustup target install i686-unknown-linux-gnu
    1.3. Установить bindgen: cargo install bindgen
2. Установить clang средствами OS. Например для Debian: sudo apt install clang

## Запуск примера Rust-приложения console_print
1. Зайти в корневую директорию embox
2. Выполнить: make confload-x86/qemu
3. Добавить в конец файла conf/mods.conf строку: include third_party.cmd.console_print
4. Выполнить: make
5. В случае успешной сборки выполнить: sudo ./scripts/qemu/auto_qemu
6. После успешного запуска embox в строке tish выполнить: rcp

## Дополнительная информация
cbind.sh.example - пример шелл-скрипта для вызова bindgen через консоль
build.rs.example - пример использования bindgen из Rust
targets - директория с целями для компиляции rust-проектов
Про global_allocator. см.: https://doc.rust-lang.org/nightly/std/alloc/#the-global_allocator-attribute
// ВАЖНО! т.к. emlibc-проект исключительно для подготовки биндингов - обязательно используем хост-цель
.clang_arg("--target=x86_64-pc-linux-gnu")
https://gcc.gnu.org/onlinedocs/gccint/Integer-library-routines.html

## Отладка. 
// Тут можно прочитать по gdb: http://rus-linux.net/nlib.php?name=/MyLDP/algol/gdb/otladka-s-gdb.html
0. Устанавливаем gdb
1. Запускаем embox через: sudo ./scripts/qemu/auto_qemu -s -S
2. Запускаем: gdb build/base/bin/embox или ugdb (если установлен)
3. Ставим точки останова, например: b НАЗВАНИЕ_ОСНОВНОЙ_ФУНЦИИ_МОДУЛЯ
    - НАЗВАНИЕ_ОСНОВНОЙ_ФУНЦИИ_МОДУЛЯ можно найти после сборки в директории: build/base/obj/module/ПУТЬ_К_МОДУЛЮ
    - Например для build/base/obj/module/third_party/cmd/console_print функция будет иметь название main_third_party__cmd__console_print
    - Посмотреть можно в файле: console_print.o.build.o.cmd.o
4. Присоединяемся из отладчика к запущенному в п.1 embox командой (tar)get ext:1234
5. Продолжаем выполнение в отладке командой: (c)ontinue
