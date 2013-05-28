#############################################################################
#
#	Makefile for building MyDriver kernel module
#
#############################################################################

#ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m := ifsttar-synchronization-test-drv.o

#else

PWD       		= $(shell pwd)

CROSS_COMPILE	?= arm-angstrom-linux-gnueabi-
KERNEL_PATH 	?= $(OECORE_TARGET_SYSROOT)/usr/src/linux

ARCH			?= arm

export ARCH CROSS_COMPILE

KBUILD_FLAGS	= -C $(KERNEL_PATH) M=$(PWD)

all : modules

default: modules

modules:
	@cp ../Module.symvers Module.symvers
	$(MAKE) $(KBUILD_FLAGS) modules

clear: 
	clear ; make clean

clean:
	rm -rf *.o .*.o.d *~ *.ko *.mod.c .*.cmd .tmp* *.order *.symvers *.markers

#endif
