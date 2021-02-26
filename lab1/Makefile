KVERSION = $(shell uname -r)

obj-m += lab1.o
lab1-objs := ./src/lab1.o ./src/proc.o ./src/vector.o ./src/device.o

all:
	make --directory /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make --directory /lib/modules/$(KVERSION)/build M=$(PWD) clean
