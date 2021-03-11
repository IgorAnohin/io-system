#!/bin/bash

insmod main.ko

fdisk -l /dev/mydisk

export PARTITIONS="mydisk1 mydisk5 mydisk6"
PARTITIONS_ARR=(mydisk1 mydisk5 mydisk6)

# Create benchmark file
SIZE=10MB
export BENCH_FILE=/tmp/file.benchmark

dd if=/dev/urandom of=$BENCH_FILE bs=$SIZE count=1

# MOUNT
for partition in ${PARTITIONS_ARR[@]}; do
    echo Mount $partition
    mkfs.vfat /dev/$partition
    mkdir /mnt/$partition
    mount /dev/$partition /mnt/$partition;
done


# BENCH
python3 perform_benchmark.py

# UNMOUNT
for partition in ${PARTITIONS_ARR[@]}; do
    echo Unmount $partition
    umount /mnt/$partition
    rmdir /mnt/$partition
done

rmmod main.ko
