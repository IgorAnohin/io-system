# Лабораторная работа 2

**Название:** Разработка драйверов блочных устройств

**Цель работы:** Получить знания и навыки разработки драйверов блочных устройств для операционной системы Linux.

## Описание функциональности драйвера
При загрузке модуля создается блочное устройство с одними первичным разделом размером 10Мбайт и одним расширенным разделом. Расширенный раздел содержит два логических раздела размером 20Мбайт каждый.

## Инструкция по сборке
Для сборки драйвера выполнить:
```bash
make
```

## Инструкция пользователя
После успешной сборки загрузить полученный модуль:
```bash
insmod main.ko
```
Проверить, что драйвер загрузился без ошибок с помощью команды `dmesg`, в выводе должно быть подобное:
```
mydisk: mydisk1 mydisk2 < mydisk5 mydisk6 >
```

## Примеры использования
После загрузки можно проверить, что драйвер создал блочное устройство согласно заданию
c помощью `fdisk -l /dev/mydisk`:
```
Disk /dev/mydisk: 50 MiB, 52428800 bytes, 102400 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x36e5756d

Device       Boot Start    End Sectors Size Id Type
/dev/mydisk1          1  20479   20479  10M 83 Linux
/dev/mydisk2      20480 102399   81920  40M  5 Extended
/dev/mydisk5      20481  61439   40959  20M 83 Linux
/dev/mydisk6      61441 102399   40959  20M 83 Linux

```
После можно отформатировать новые разделы и смонтировать их в директорию `/mnt`. Выполним команду `make setup` и убедимся что все сработало без ошибок выполнив `lsblk -l /dev/mydisk`:
```
NAME      MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
mydisk    252:0    0   50M  0 disk
├─mydisk1 252:1    0   10M  0 part /mnt/mydisk1
├─mydisk2 252:2    0    1K  0 part
├─mydisk5 252:5    0   20M  0 part /mnt/mydisk5
└─mydisk6 252:6    0   20M  0 part /mnt/mydisk6
```

Далее можно провести бенчмарки, которые измеряют скорость передачи файла 10MB между различными разделами созданного блочного устройства и реального SSD выполнив `sudo ./benchmark_starter`:
```
partitions: ['mydisk1', 'mydisk5', 'mydisk6']
From /mnt/mydisk1 to /mnt/mydisk5, speed=193.1 Mb/s
From /mnt/mydisk1 to /mnt/mydisk6, speed=184.73333333333332 Mb/s
From /mnt/mydisk1 to /home/io-anokhin-gostev, speed=135.45666666666665 Mb/s
From /mnt/mydisk5 to /mnt/mydisk1, speed=180.53333333333333 Mb/s
From /mnt/mydisk5 to /mnt/mydisk6, speed=187.53333333333333 Mb/s
From /mnt/mydisk5 to /home/io-anokhin-gostev, speed=134.33333333333334 Mb/s
From /mnt/mydisk6 to /mnt/mydisk1, speed=192.56666666666666 Mb/s
From /mnt/mydisk6 to /mnt/mydisk5, speed=191.66666666666666 Mb/s
From /mnt/mydisk6 to /home/io-anokhin-gostev, speed=141.83333333333334 Mb/s
From /home/io-anokhin-gostev to /mnt/mydisk1, speed=182.9 Mb/s
From /home/io-anokhin-gostev to /mnt/mydisk5, speed=180.13333333333333 Mb/s
From /home/io-anokhin-gostev to /mnt/mydisk6, speed=182.1 Mb/s
```
Видно, что при чтении/записи файла в созданное блочное устройтво скорость записи в районе 200 MB/s.
Однако при записи файла в директорию, которая смонтирована на реальный SSD можно в одном из
случаем увидеть падение скорости до 135 MB/s, что подтверждает теорию о том, что SSD обладает меньшими скоростями на чтение/запись нежели RAM. Скорее всего этот факт был бы более явным, если проводить бенчмарки на файле большего размера.

После звершения работы можно отмонтировать разделы и выгрузить модуль из ядра:
```bash
make umount
rmmod main
```
