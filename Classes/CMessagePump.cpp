/**
 *	\file	CMessagePump.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CMessagepump class
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 22 June, 2012
 *
 */

#include "CMessagePump.h"

using namespace std;

/**
 *	\fn			CMessagePump::CMessagePump(char * sServerIP, unsigned int uiServerPort, unsigned int uiDelay, unsigned int uiMaxMessage)
 *	\brief 		Constructor of the CMessagePump class.
 *
 *	\param[in] 	sServerIP		: ip server
 *	\param[in] 	uiServerPort	: port server
 *	\param[in] 	uiDelay			: delay to check if the list is empty
 *	\param[in] 	uiMaxMessage	: Max message on the list
 */
CMessagePump::CMessagePump(char * sServerIP, unsigned int uiServerPort, unsigned int uiDelay, unsigned int uiMaxMessage)
{
	m_sServerIP 	 	= sServerIP;
	m_uiServerPort		= uiServerPort;
	m_uiDelay			= uiDelay;
	m_uiMaxMessage		= uiMaxMessage;
	m_bWifiState		= false;
	m_bThreadRunning	= false;
}

/**
 *	\fn 		CMessagePump::~CMessagePump()
 *	\brief		Destructor of the CMessagePump class.
 */
CMessagePump::~CMessagePump()
{

}

/**
 *	\fn			int CMessagePump::AddMessageToList(CEvent* sDate, char* sData, unsigned int uiLen, unsigned int uiID)
 *	\brief 		Add a message to the list
 *
 *	\param[in]	sDate	: Date
 *	\param[in]	sData	: Data of the message
 *	\param[in]	uiLen	: Lenght of the message
 *	\param[in]	uiID	: ID of the message
 *	\return 	int (-1 for error) (0 otherwise)
 */
int CMessagePump::AddMessageToList(CEvent* pDate, char* sData, unsigned int uiLen, unsigned int uiID)
{

	struct CMessage*  sMessage;
	sMessage= (struct CMessage*) malloc(sizeof(struct CMessage));


	if(uiID == MESSAGE_ID_ASCII){
		pDate = NULL;
		memset (sMessage->m_sData, '\0', sizeof(sMessage->m_sData));
		memcpy(sMessage->m_sData, sData, uiLen);

		//sMessage->m_pDate	= sDate;
		if(uiLen % 2) uiLen +=1;

		sMessage->m_sLen	= uiLen;
		sMessage->m_sID 	= uiID;
		sMessage->m_sChannel =1;
	}
	

	if(m_List_Message.size() <= m_uiMaxMessage){
		m_List_Message.push_back(sMessage);
	}else{
		cout<<"Max Message reached"<<endl;
		return -1;
	}

	return 0;
}

/**
 *	\fn			int CMessagePump::Start()
 *	\brief 		Start the thread CountDown
 *
 *	\return 	int (-1 for error) (0 otherwise)
 */
int CMessagePump::Start()
{
	boost::thread m_oThread(boost::bind(&CMessagePump::CountDown, this));
	//TODO tester si le lancement du thread a échoué

	return 0;
}

/**
 *	\fn			void CMessagePump::CountDown()
 *	\brief 		check if the list is empty every m_uiDelay seconds and if is not empty send all message who are in the list
 *
 */
void CMessagePump::CountDown()
{
	struct CMessage* sFirstMessage;
	sFirstMessage = (struct CMessage*) malloc(sizeof(struct CMessage));
	bool bConnected = false;


	list<CMessage*>::iterator it;
	
	CTcpIpClient* pClient = NULL;
	pClient = new CTcpIpClient(false,0);

	m_bThreadRunning = true;
	
	do{
	
		if(bConnected == true){
			pClient->Close();
			bConnected = false;
		}
			
			
		if(m_bWifiState == true)
		{
			m_oState = WIFI_OFF_STATE;
			system("./wifi-down.sh");
			m_bWifiState = false;
		}
		m_oState = WAITING_MESSAGE_STATE;
		sleep(m_uiDelay);
		m_oState = TEST_LIST_STATE;		
		
		if(!m_List_Message.empty())
	  	{

	  		if(m_bWifiState == false){
				m_oState = WIFI_ON_STATE;
				system("./wifi-up.sh"); 
				if(1){
					m_bWifiState = true;
				}
			}
			m_oState = ON_CONNECT_STATE;
			if(pClient->Connect(m_sServerIP,m_uiServerPort) < 0){
				cout<<"Connect Failed"<<endl;
			}
			else {
				bConnected = true;
				while(!m_List_Message.empty()){
					
					m_oState = SEND_MESSAGE_STATE;
					sFirstMessage = m_List_Message.front();
					
					if(pClient->Write((char*)sFirstMessage,HEADER_MESSAGE + sFirstMessage->m_sLen) < 0){
						cout<<"Error On Write"<<endl;
						break;
					}else{
						it = m_List_Message.begin();
						m_List_Message.erase(it);
					}
					cout<<"Message Send"<<endl;
					usleep(100000);
				}
			}
		}

	}while(m_bThreadRunning);

}

/**
 *	\fn			int CMessagePump::Stop()
 *	\brief 		stop the thread
 *
 *	\return 	int (-1 for error) (0 otherwise)
 */
int CMessagePump::Stop()
{
	m_bThreadRunning = false;
	return 0;
}

