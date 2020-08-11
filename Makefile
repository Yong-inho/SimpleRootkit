ROOTKIT 	:= rootkit_yong

MODULEDIR 	:= /lib/modules/$(shell uname -r)
BUILDDIR	:= $(MODULEDIR)/build

HEADER		:= $(PWD)/headers

obj-m 		:= $(ROOTKIT).o

$(ROOTKIT)-y	+= core.o
$(ROOTKIT)-y	+= module.o
$(ROOTKIT)-y	+= file.o


ccflags-y	:= -I$(HEADER)

all:
	$(MAKE) -C $(BUILDDIR) M=$(PWD) modules
	$(CC) controller.c -o controller
clean:
	$(MAKE) -C $(BUILDDIR) M=$(PWD) clean
	rm controller
