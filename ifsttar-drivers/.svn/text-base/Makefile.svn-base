# Build ifsttar-synchronization-drv and ifsttar-gpio-drv

DIRS = \
	ifsttar-synchronization-drv \
	ifsttar-gpio-drv

all clean mrproper:
	for dir in $(DIRS) ; do [ ! -d $$dir ] || $(MAKE) -C $$dir $@ || exit 1 ; done

