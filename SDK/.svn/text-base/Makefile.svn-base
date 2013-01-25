CROSS_COMPILE=arm-angstrom-linux-gnueabi-
#QUIET=@

DOCS_DIR = docs
EXEC_DIR = apps

PEGASE2 = CEvent.o CIODevice.o

OBJ_GpioTest = $(PEGASE2) CGPIO.o CGPIOExample.o
OBJ_SpiTest = $(PEGASE2) CSpiPort.o CSpiPortExample.o
OBJ_UartTest = $(PEGASE2) CUart.o CUartExample.o
OBJ_GpsTest = $(PEGASE2) CUart.o CGPS.o CGPSExample.o
OBJ_ClientTest = $(PEGASE2) CTcpIpClient.o CTcpIpClientExample.o
OBJ_ServerTest = $(PEGASE2) CTcpIpServer.o CTcpIpServerExample.o
OBJ_MessagePumpTest = $(PEGASE2) CTcpIpClient.o CMessagePump.o CMessagePumpExample.o
OBJ_SynchronizationTest = $(PEGASE2) CUart.o CGPS.o CGPIO.o SynchronizationTest.o
OBJ_App1 = $(PEGASE2) CTcpIpClient.o CUart.o CGPS.o App1.o
OBJ_App2 = $(PEGASE2) CTcpIpClient.o CUart.o CGPS.o App2.o

CFLAGS = -Wall -Wextra -Werror
LFLAGS = -lboost_signals -lboost_thread

LFLAGS_GpioTest = -lpthread

#EXE = GpioTest SpiTest UartTest GpsTest ClientTest ServerTest MessagePumpTest SynchronizationTest App1 App2
EXE = GpioTest UartTest GpsTest ClientTest ServerTest MessagePumpTest SynchronizationTest App1 App2

apps: $(EXE)

all : apps docs

GpioTest.x: $(OBJ_GpioTest)
SpiTest.x: $(OBJ_SpiTest)
UartTest.x: $(OBJ_UartTest)
GpsTest.x: $(OBJ_GpsTest)
ClientTest.x: $(OBJ_ClientTest)
ServerTest.x: $(OBJ_ServerTest)
MessagePumpTest.x: $(OBJ_MessagePumpTest)
SynchronizationTest.x: $(OBJ_SynchronizationTest)
App1.x: $(OBJ_App1)
App2.x: $(OBJ_App2)

docs :
	doxygen doxygen.config
	$(QUIET)ln -fs html/index.html $(DOCS_DIR)/.
	$(MAKE) -C $(DOCS_DIR)/latex clean all
	$(QUIET)ln -fs latex/refman.pdf $(DOCS_DIR)/.
	mv doxygen_warnings.log $(DOCS_DIR)/.

.SUFFIXES:
.SUFFIXES: .c .cpp .o

.cpp.o:
	@echo -e 'CC\t$@'
	$(QUIET)$(CROSS_COMPILE)g++ -o $@ -c $< $(CFLAGS)

% : %.x
	@mkdir -p $(EXEC_DIR)
	@echo -e 'LD\t$(EXEC_DIR)/$*'
	$(QUIET)$(CROSS_COMPILE)g++ -o $(EXEC_DIR)/$* $(OBJ_$@) $(LFLAGS) $(LFLAGS_$@)
	
clear :
	clear
	make clean

clean mrproper:
	$(QUIET)$(RM) -rf $(OBJ) *~ *.o $(EXEC_DIR) $(DOCS_DIR) 
