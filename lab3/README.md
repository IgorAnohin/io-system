# Лабораторная работа 3

**Название:** "Разработка драйверов сетевых устройств"

**Цель работы:** получить знания и навыки разработки драйверов сетевых интерфейсов для операционной системы Linux

## Описание функциональности драйвера

Пакеты протокола ICMP (Internet Control Message Protocol) – только тип 8. Вывести данные. 
Состояние разбора пакетов необходимо выводить в файл в директории `/proc`

## Инструкция по сборке

```shell
$ cd io-system/lab3
$ make
```

## Инструкция пользователя

После успешной сборки загрузить полученный модуль:
```shell
$ insmod lab3.ko
```

Проверить, что драйвер загрузился без ошибок с помощью команды dmesg, в выводе должно быть подобное:
```shell
[24313.650432] Module lab3 loaded
[24313.650433] lab3: create link vni0
[24313.650434] lab3: registered rx handler for enp0s3
[24313.665287] IPv6: ADDRCONF(NETDEV_UP): vni0: link is not ready
[24313.665321] lab3: device opened: name=vni0
```

Проверить созданный сетевой интерфейс:
```
$ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host
       valid_lft forever preferred_lft forever
2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 08:00:27:cb:4b:6c brd ff:ff:ff:ff:ff:ff
    inet 192.168.1.45/24 brd 192.168.1.255 scope global dynamic noprefixroute enp0s3
       valid_lft 20993sec preferred_lft 20993sec
    inet6 fe80::1d57:8539:6289:4b71/64 scope link noprefixroute
       valid_lft forever preferred_lft forever
7: vni0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UNKNOWN group default qlen 1000
    link/ether 08:00:27:cb:4b:6c brd ff:ff:ff:ff:ff:ff
```

Проверить файл в `/proc`:
```shell
$ cat /proc/lab3
```

Удалить модуль:
```shell
$ sudo rmmod lab3
```


## Примеры использования

...
