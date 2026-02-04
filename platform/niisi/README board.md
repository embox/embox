Скопировать в папку boot/barebox/embox.bin на на SD-карте файлы barebox.sh и embox.bin

Вставить карту обратно в плату

Открыть терминал (например minicom) со скоростью 115200 на порту /dev/ttyUSB1

Наблюдать вывод. Должно появиться приглашение embox

```
## Starting application at 0x80001000 ...                                       
                                                                                
[info] (Kernel) Interrupt controller: sc64_intc_v2                              
[info] (Kernel) Embox kernel start                                              
[info] (unit) initializing embox.mem.phymem                                     
[info] (phymem) start=0xffffffff821c3000 end=0xffffffff84001000 size=31711232   
[info] (mod) runlevel is 0                                                      
[info] (unit) initializing embox.mem.static_heap                                
[info] (unit) initializing embox.kernel.task.task_resource                      
[info] (unit) initializing embox.kernel.task.task_table                         
[info] (unit) initializing embox.kernel.task.kernel_task                        
[info] (unit) initializing embox.profiler.tracing                               
[info] (unit) initializing embox.kernel.sched.sched                             
[info] (mod) runlevel is 1                                                      
[info] (unit) initializing embox.device.char_dev                                
[info] (unit) initializing embox.fs.buffer_cache                                
[info] (unit) initializing embox.driver.flash.flash_nofs                        
[info] (unit) initializing komdiv.driver.clock.sc64_timer_v2                    
[info] (unit) initializing embox.driver.tty.serial                              
[info] (unit) initializing embox.fs.rootfs_dvfs                                 
[info] (mod) runlevel is 2                                                      
[info] (mod) runlevel is 3                                                      
Default IO device[ttyS0]                                                        
>export PWD=/                                                                   
>export HOME=/                                                                  
>tish                                                                           
root@embox:/#
```
В консоли набираем version или uname -a









== Прошивка ПЛК1-01 с инструментальной машины ==

Необходимо, чтобы размер прошивки соответствовал размеру flash-памяти. Для этого необходимо записать прошивку в файл образа, который равен размеру флешки (4МБ).

flash-kd.sh - скрипт записи прошивки под размер flash-памяти 


Прошивка модуля:

    1. sudo apt install libqt5serialbus5-dev && sudo adduser $USER dialout

    2. скачать файл   flashrom 

    3.  Посмотреть версии флэшей.  Эти верии далее ввести с параметром ( -c)
    `$ ./flashrom -p ft2232_spi:type=plc`

    4. При необходимости, сохранить бекап flash-памяти в файл barebox_back.bin
    `$ ./flashrom -p ft2232_spi:type=plc -c W25Q32BV/W25Q32CV/W25Q32DV -r barebox_back.bin`

    5. Записать во flash-память из файла barebox_back.bin
        `./flashrom -p ft2232_spi:type=plc -c W25Q32BV/W25Q32CV/W25Q32DV -w barebox_back.bin`

=== Посмотреть версии флэшей ===
        
./flashrom -p ft2232_spi:type=plc
flashrom 1.4.0-devel (git:v1.2-1402-gf2a75047) on Linux 5.4.0-190-generic (x86_64)
flashrom is free software, get the source code at https://flashrom.org

Using clock_gettime for delay loops (clk_id: 1, resolution: 1ns).
BOARD:          PLC Baget
Found Winbond flash chip "W25Q32BV/W25Q32CV/W25Q32DV" (4096 kB, SPI) on ft2232_spi.
Found Winbond flash chip "W25Q32FV" (4096 kB, SPI) on ft2232_spi.
Found Winbond flash chip "W25Q32JV" (4096 kB, SPI) on ft2232_spi.
Multiple flash chip definitions match the detected chip(s): "W25Q32BV/W25Q32CV/W25Q32DV", "W25Q32FV", "W25Q32JV"
Please specify which chip definition to use with the -c <chipname> option.
