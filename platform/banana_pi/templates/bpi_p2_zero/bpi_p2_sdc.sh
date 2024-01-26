#!/bin/bash

# разметка SD
echo '1. создание разделов'
(
 echo o;

 echo n;
 echo p;
 echo 1;
 echo 2048;
 echo +100M;
 echo;
 
 echo n;
 echo p;
 echo 2;
 echo;
 echo +8G;
 echo;
  
 echo w;
) | fdisk /dev/sdc

echo 'Ваша разметка диска'
fdisk -l /dev/sdc

echo '2. Очистка первого раздела'
dd if=/dev/zero of=/dev/sdc bs=1K count=1023 seek=1

echo '3. Копирование u-boot'
dd if=u-boot-sunxi-with-spl.bin of=/dev/sdc bs=1024 seek=8

echo '4.Форматирование дисков'
mkfs.vfat /dev/sdc1
mkfs.ext2 /dev/sdc2

echo '5. Установка embox'
mount /dev/sdc2 /mnt
mkdir /mnt/boot
mount /dev/sdc1 /mnt/boot

mkimage -C none -A arm -T script -d boot.cmd boot.scr

cp boot.scr /mnt/boot
cp ../embox/build/base/bin/embox.bin /mnt/boot
umount /mnt/boot
umount /mnt



