#############################################################################
#
#	Makefile for building MyDriver kernel module
#
#############################################################################

#ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m := ifsttar_can_spi.o

#else

PWD       		= $(shell pwd)

CROSS_COMPILE	?= arm-angstrom-linux-gnueabi-
KERNEL_PATH 	?= /home/theophile/TRAVAIL/PEGASE-2/Perso/SPI/Drivers/Source/git/

ARCH			?= arm

export ARCH CROSS_COMPILE

KBUILD_FLAGS	= -C $(KERNEL_PATH) M=$(PWD)

all : modules

default: modules

modules:
	$(MAKE) $(KBUILD_FLAGS) modules

clear: 
	clear ; make clean

clean:
	rm -rf *.o .*.o.d *~ *.ko *.mod.c .*.cmd .tmp* *.order *.symvers *.markers

#endif
