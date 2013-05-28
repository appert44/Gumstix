/**
 *	\file	common.h
 *
 *	\brief 	This file is used to include the librairies, to define the events available, and to define the print prefixes of each class.
 *	
 *	\author Théophile Marcadé, Jonathan Aillet
 *
 *	\version v1.0a
 *
 *	\date 16 July, 2012
 *
 */

#ifndef COMMON_H
#define COMMON_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// Librairies includes
#include <arpa/inet.h>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
//#include <boost/bind.hpp>
//#include <endian.h>
//#include <errno.h>
#include <fcntl.h>
//#include <iostream>
//#include <list>
#include <linux/rtc.h>
//#include <math.h>
//#include <netdb.h>
#include <netinet/tcp.h>
//#include <ostream>
#include <poll.h>
#include <pthread.h>
//#include <stdint.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
//#include <sys/socket.h>
//#include <sys/types.h>
#include <termios.h>
//#include <time.h>
//#include <unistd.h>

#endif 		// DOXYGEN_SHOULD_SKIP_THIS

// Drivers includes
#include "../ifsttar-drivers/ifsttar-synchronization-drv/ifsttar-synchronization-drv.h"
#include "../ifsttar-drivers/ifsttar-gpio-drv/ifsttar-gpio-drv.h"
// TODO : à inclure
//#include "../ifsttar-drivers/ifsttar-spi-drv/ifsttar_can_spi.h.h"

// Events defines
#include "events.h"

// To infinite and beyond
#define INFINITE								0xFFFFFFFF			/**< \brief Maximum valor of an integer (0xFFFFFFFF = 4294967295) */

// Class prints prefixes
#define CEVENT_PRINT_NAME			"CEvent > "						/**< \brief Prefixe of each prints of the CEvent class */
#define CIODEVICE_PRINT_NAME		"CIODevice > "					/**< \brief Prefixe of each prints of the CIODevice class */
#define CGPIO_PRINT_NAME			"CGPIO > "						/**< \brief Prefixe of each prints of the CGPIO class */
#define CGPS_PRINT_NAME				"CGPS > "						/**< \brief Prefixe of each prints of the CGPS class */
#define CMESSAGEPUMP_PRINT_NAME		"CMessagePump > "				/**< \brief Prefixe of each prints of the CMessagePump class */
#define CTCPIPCLIENT_PRINT_NAME		"CTcpIpClient > "				/**< \brief Prefixe of each prints of the CTcpIpClient class */
#define CTCPIPSERVER_PRINT_NAME		"CTcpIpServer > "				/**< \brief Prefixe of each prints of the CTcpIpServer class */
#define CSPIPORT_PRINT_NAME			"CSpiPort > "					/**< \brief Prefixe of each prints of the CSpiPort class */
#define CUART_PRINT_NAME			"CUart > "						/**< \brief Prefixe of each prints of the CUart class */

// Errors defines file
#include "sdk_errors.h"

// Template used to delete the unused parameters warnings
template<typename T>
void unused(T const &) { }			/**< \brief Template of the function used to ignore the unused parameters warnings */

#endif // COMMON_H
