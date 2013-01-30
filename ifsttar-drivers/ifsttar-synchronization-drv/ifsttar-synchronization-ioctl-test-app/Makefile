CROSS_COMPILE=arm-angstrom-linux-gnueabi-
#QUIET=@

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

EXE = ifsttar-synchronization-ioctl-test-app

CFLAGS = -Wall -Wextra -Werror
LFLAGS = -lpthread

all: $(EXE)

$(EXE): $(OBJ)
	@echo -e 'LD\t$@'
	$(QUIET)$(CROSS_COMPILE)g++ -o $@ $^ $(LFLAGS)

.SUFFIXES:
.SUFFIXES: .c .cpp .o

.cpp.o:
	@echo -e 'CC\t$@'
	$(QUIET)$(CROSS_COMPILE)g++ -o $@ -c $<  $(CFLAGS)

clear :
	clear
	make clean

clean mrproper:
	$(QUIET)$(RM) -f $(OBJ) $(EXE)
