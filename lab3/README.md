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

Отправка `ICMP` пакетов на родительский интерфейс `enp0s3`. Для этого выполним команды:
```shell
$ ping -c 2 -p 5765206c6f76650a -s 8 192.168.1.45
$ ping -c 2 -p 5365726765690a -s 7 192.168.1.45
$ ping -c 2 -p 42796b6f76736b69690a -s 10 192.168.1.45
```

Проверка содержимого файла `/proc/lab3`:
```
$ cat /proc/lab3
We love
We love
Sergei
Sergei
Bykovskii
Bykovskii
```

Cтатистику по принятым пакетам выполнив:
```
$  ip -s link show vni0
8: vni0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UNKNOWN mode DEFAULT group default qlen 1000
    link/ether 08:00:27:cb:4b:6c brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast
    276        6        0       0       0       0
    TX: bytes  packets  errors  dropped carrier collsns
    0          0        0       0       0       0

```

## Фильтрация по содержимому ICMP пакета

Сначала необходимо задать строку-фильтр, по которой будут отбираться подходящие ICMP пакеты.
Фильтр задается путем отправления UDP пакета на порт `69` машины:
```shell
nping -N -udp -p 69 192.168.1.45 --data-string "ovs"
```
В данном примере будут отбираться лишь ICMP пакеты, которые содержат в своем payload'е последовательность `ovs`.

В dmesg видим, что фильтр `ovs` был установлен:
```
[ 1646.766722] lab3: handled frame.
[ 1646.766769] DATA LEN: 3
[ 1646.766784] We assigned following filter: ovs
[ 1646.766798] lab3: it was UDP control packet!
[ 1647.766633] lab3: handled frame.
[ 1647.766646] DATA LEN: 3
[ 1647.766650] We assigned following filter: ovs
[ 1647.766653] lab3: it was UDP control packet!
```

Отправка `ICMP` пакетов на родительский интерфейс `enp0s3`. Для этого выполним команды:
```shell
ping -c 2 -p 42796b6f76736b69690a -s 10 192.168.1.45
```

В dmesg видим, что последовательность `Bykovskii` (payload ICMP пакета) была заматчена установленным ранее фильтром `ovs`:
```
[ 1718.859763] lab3: We match Bykovskii
                word in ovs pcakge!
[ 1718.859803] lab3: it was my IMCP packet!
[ 1719.877044] lab3: handled frame.
[ 1719.877059] lab3: We match Bykovskii
                word in ovs pcakge!
[ 1719.877062] lab3: it was my IMCP packet!
```

Проверка содержимого файла `/proc/lab3`:
```
Bykovskii
Bykovskii
```

Затем установим фильтр по последовательности `ovn`:
```shell
nping -N -udp -p 69 192.168.1.45 --data-string "ovn"
```

В dmesg видим, что фильтр установлен:
```
[ 1806.728238] lab3: handled frame.
[ 1806.728297] DATA LEN: 3
[ 1806.728311] We assigned following filter: ovn
[ 1806.728324] lab3: it was UDP control packet!
[ 1807.730091] lab3: handled frame.
[ 1807.730150] DATA LEN: 3
[ 1807.730165] We assigned following filter: ovn
[ 1807.730178] lab3: it was UDP control packet!
```

С фильтром по последовательности `ovn` попытаемся заматчить последовательность `Bykovskii`
```shell
ping -c 2 -p 42796b6f76736b69690a -s 10 192.168.1.45
```

В dmesg видим, что пересланная по ICMP последовательность не была заматчена с фильтром `ovs`:
```
[ 1838.250078] lab3: handled frame.
[ 1838.250080] lab3: We DONT match Bykovskii
                word in ovn pcakge!
[ 1839.274159] lab3: handled frame.
[ 1839.274218] lab3: We DONT match Bykovskii
                word in ovn pcakge!
```

Проверка содержимого файла `/proc/lab3`. Видим, что новых payload'ов не добавилось (как было 2 строки, так и осталось)
```
Bykovskii
Bykovskii
```

Выведем статистику по заматченым ICMP пакетам:
```
$ ip -s link show vni0
8: vni0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UNKNOWN mode DEFAULT group default qlen 1000
    link/ether 08:00:27:cb:4b:6c brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast
    92         2        0       0       0       0
    TX: bytes  packets  errors  dropped carrier collsns
    0          0        0       0       0       0
```
... видим что всего заматчено 2 ICMP пакета (из 4-ех, которые суммарно были отправлены)
