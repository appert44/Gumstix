/**
 *	\file	CMessagePump.h
 *
 *	\brief 	This header file is used to declare the CMessagePump class and all its methods.
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 22 June, 2012
 *
 */

#ifndef CMESSAGEPUMP_H
#define CMESSAGEPUMP_H

#include "PEGASE_2.h"

/** \brief Value provided for the state machine */
typedef enum
{
	WAITING_MESSAGE_STATE,
	TEST_LIST_STATE,
	WIFI_ON_STATE,
	ON_CONNECT_STATE,
	SEND_MESSAGE_STATE,
	WIFI_OFF_STATE
} MESSAGE_PUMP_STATE_MACHINE;

// TODO : vérfier que ces definitions sont vraiment utile (definitions utilisées pour le transfert de message au C.A.S.C. et non pas pour une pompe à message)
#define HEADER_MESSAGE 					6		/**< \brief C.A.S.C. Header message Id */
#define MESSAGE_ID_ASCII				24		/**< \brief C.A.S.C. ASCII message Id */

/**
 *	\class CMessagePump
 *
 *	\brief 	The CMessagePump Class is a message pump who send all this message every times the users want
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 22 June, 2012
 *
 */
 
class CMessagePump
{
	public :
	// TODO : vérfier que cette structure est vraiment utile (structure utilisée pour le transfert de message au C.A.S.C. et non pas pour une pompe à message)
	struct CMessage
	{
		short		m_sID;								/**< \brief ID of message */
	//	CEvent	 	m_pDate;							/**< \brief Date of Message */
		short		m_sLen;								/**< \brief Lenght of Data */
		short       m_sChannel;							/**< \brief Channel of the supervisor */
		short		m_sData[2000];						/**< \brief Data of the message */
	};
		
	typedef std::list <CMessage *> CMessageList;
	
	private :
	/* Attributes */
	/**************/
	bool 						m_bWifiState;				/**< \brief Boolean wifi status */
	bool						m_bThreadRunning;			/**< \brief Boolean for close the thread */
	
	unsigned int 				m_uiServerPort;				/**< \brief Port of the server */
	unsigned int				m_uiDelay;					/**< \brief Delay to check if the list is empty */
	unsigned int				m_uiMaxMessage;				/**< \brief Max message on the list */
	
	char * 						m_sServerIP;				/**< \brief Ip of the server */
	
	boost::thread				m_oThread;					/**< \brief Thread */
	
	CMessageList				m_List_Message;				/**< \brief The list message*/
	
	MESSAGE_PUMP_STATE_MACHINE	m_oState;					/**< \brief State of the state machine */
	
	/* Methods   */
	/*************/
	void CountDown();
	
	public : 
	/* Methods   */
	/*************/
	// Constructor
	CMessagePump(char * sServerIP, unsigned int uiServerPort, unsigned int uiDelay ,unsigned int uiMaxMessage);
	// Destructor	
	~CMessagePump();
	
	int AddMessageToList (CEvent* pDate,char* sData,unsigned int uiLen,unsigned int uiID);
	int Start();
	int Stop();
};

#endif // CMESSAGEPUMP_H
