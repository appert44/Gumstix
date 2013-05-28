CROSS_COMPILE=arm-angstrom-linux-gnueabi-
#QUIET=@

SRC = $(wildcard *.cpp)
OBJ = ../../../SDK/CEvent.o ../../../SDK/CIODevice.o ../../../SDK/CUart.o ../../../SDK/CGPS.o $(SRC:.cpp=.o) 

EXE = ifsttar-synchronization-fct-test-app

CFLAGS = -Wall -Wextra -Werror
LFLAGS = -lboost_signals -lboost_thread

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
