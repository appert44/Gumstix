MODULES := ifsttar-gpio-drv.o 

#guest architecture
ARCH := arm

obj-m := $(MODULES)

#path of the arm compiled kernel
ROOTDIR := $(OECORE_TARGET_SYSROOT)/usr/src/kernel

MAKEARCH := $(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

all: modules

modules:
	@cp ../ifsttar-synchronization-drv/Module.symvers Module.symvers
	$(MAKEARCH) -C $(ROOTDIR) M=${shell pwd} modules

clean:
	$(MAKEARCH) -C $(ROOTDIR) M=${shell pwd} clean


