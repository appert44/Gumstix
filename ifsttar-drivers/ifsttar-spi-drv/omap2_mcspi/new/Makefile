#############################################################################
#
#	Makefile for building MyDriver kernel module
#
#############################################################################

#ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m := omap2_mcspi.o


#else

PWD       		= $(shell pwd)

CROSS_COMPILE	?= arm-angstrom-linux-gnueabi-
KERNEL_PATH 	?= $(OECORE_TARGET_SYSROOT)/usr/src/linux
ARCH			?= arm
CC=arm-angstrom-linux-gnueabi-gcc
LD=arm-angstrom-linux-gnueabi-ld
STRIP=arm-angstrom-linux-gnueabi-strip
MACHINE=overo

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
