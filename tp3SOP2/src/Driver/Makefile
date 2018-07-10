# Makefile para el driver del TP3 de Sistemas Operativos II.
# Autor: López Gastón. 

obj-m += modulo.o
KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
 
clean:
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean
