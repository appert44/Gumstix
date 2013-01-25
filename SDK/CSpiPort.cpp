
/**
 *	\file	CSpiPort.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CSpiPort class
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 8 June, 2012
 *
 */

#include "CSpiPort.h"

using namespace std;

/**
 *	\fn		CSpiPort::CSpiPort(unsigned int uiSampleSize, unsigned int uiSpiChannel, unsigned int uiSampleCountbyCB, unsigned int uiSampleFrequency , unsigned int uiCsNumber)
 *	\brief	Constructor of the CSpiPort class
 *
 *  \param[in]	uiSampleSize 		: 	Size of the Sample of the ADC
 *  \param[in]	uiSpiChannel 		:	Channel of the Spi
 *  \param[in]	uiSampleCountbyCB 	:	Number of sample on Callback
 *  \param[in]  uiSampleFrequency 	:	Sample Frequency
 *  \param[in]  uiCsNumber			:	Cs of the SpiChannel
 */
CSpiPort::CSpiPort(unsigned int uiSampleSize, unsigned int uiSpiChannel, unsigned int uiSampleCountbyCB, unsigned int uiSampleFrequency , unsigned int uiCsNumber)
{	
	m_uiSampleSize = uiSampleSize;
	m_uiSpiChannel = uiSpiChannel;
	m_uiSampleCountbyCB = uiSampleCountbyCB;
	m_uiSampleFrequency = uiSampleFrequency;
	m_uiCsNumber = uiCsNumber;
	m_fd = -1;
}

/** 
 *	\fn		CSpiPort::~CSpiPort()
 *	\brief	Destructor of the CSpiPort class
 */
CSpiPort::~CSpiPort()
{
	m_fd = -1;
}


/**
 *	\fn		int CSpiPort::Open() 
 *	\brief	Launch initialization and configuration of a CSpiPort object
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CSpiPort::Open() 
{
	int iRet = -1;
	
	m_fd = open("/dev/ifsttar_can_spi", O_RDWR);

	if(m_fd<0)
	return iRet;

	if((m_uiSampleSize==8) || (m_uiSampleSize==16) || (m_uiSampleSize==32)){

		if(ioctl(m_fd, CMD_SPI_BITS_PER_WORD, m_uiSampleSize) > 0) {
			cout<<"ioctl SPI BITS PER WORD failed"<<endl;
			return iRet;
		}		
	}else
	{
		cout<<"Wrong word-len "<<endl;
			return iRet;
	}
	

	if(m_uiSampleCountbyCB < 160 || m_uiSampleCountbyCB > 100000) 
	{
		cout<<"Sample Number not correct"<<endl;
		return iRet;
	}

	if(ioctl(m_fd, CMD_SPI_NB_ECH, m_uiSampleCountbyCB) < 0) {
		cout<<"ioctl SampleCount failed"<<endl;
		return iRet;
	}

	if(m_uiSampleFrequency < 1 || m_uiSampleFrequency > 300000){
		cout<<"Bad Sample Frequency"<<endl;
		return iRet;
	}

	if(ioctl(m_fd, CMD_SPI_FREQUENCY, m_uiSampleFrequency) < 0) {
		cout<<"ioctl Sample Frequency failed"<<endl;
		return iRet;
	}


	return 0;
}

/**
 *	\fn		int CSpiPort::Close()
 *	\brief	Close the SpiPort object
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CSpiPort::Close()
{
	ioctl(m_fd,CMD_SPI_STOP);
	// Fermeture du driver
	close(m_fd);
	return 0;
}

/**
 *	\fn		int CSpiPort::Start()
 *	\brief	Start the SPI acquisition
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CSpiPort::Start()
{
	int iRet = -1;

	if(ioctl(m_fd, CMD_SPI_START) < 0) {
		cout<<"ioctl Start Failed"<<endl;
		return iRet;
	}

	return 0;
}

/**
 *	\fn		int CSpiPort::Stop()
 *	\brief	stop the SPI acquisition
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CSpiPort::Stop()
{
	int iRet = -1;

	if(ioctl(m_fd, CMD_SPI_STOP) < 0) {
		cout<<"ioctl Stop Failed"<<endl;
		return iRet;
	}

	return 0;
}

/**
 *	\fn		int CSpiPort::Write(char * pcBuffer, unsigned int uiBuffer_size)
 *	\brief	Write on the slave
 *  \param[in] pcBuffer 		: 	Buffer who users want to send
 *  \param[in] uiBuffer_size	:	Size of the buffer
 *	\return A negative number if an error occured, 0 otherwise
 */
int CSpiPort::Write(char * pcBuffer, unsigned int uiBuffer_size)
{
	int iRet = -1;
	
	if(pcBuffer == NULL){
		cout<<"Error NULL buffer..."<<endl;
		return iRet;
	}
	iRet = write(m_fd,pcBuffer, uiBuffer_size);
	if(iRet < 0){
		cout<<"Error On Write.."<<endl;
		return iRet;
	}
	return 0;
} 
	
/**
 *	\fn		int CSpiPort::RegisterCallBack(unsigned int uiIdEvent, boost::function<CallBack> oFct, void* pData1 = NULL, void* pData2 = NULL)
 *  \brief	Register a Callback which will be called when the event specified in EnventID will occure
 *  \param[in]	uiIdEvent : 		Integer that indicate the event ID on which the function will be launched
 *  \param[in]	oFct : 			Object which indicates the function to launch when a event occure
 *  \param[in] 	pData1 : 		User pointer.
 *  \param[in] 	pData2 : 		User pointer.
 *	\return A negative number if an error occured, 0 otherwise
 */
int CSpiPort::RegisterCallBack(unsigned int uiIdEvent, boost::function<CallBack> oFct, void* pData1 = NULL, void* pData2 = NULL)
{
	bool b_ThreadLaunching = false;
	
	// If the boost::signal is empty
	if (m_oSigOnRec.num_slots() == 0) {
		if (m_bThreadRunning) {
			cout << "Error , thread launched without CallBack connected on signal." << endl;
			// Indicate to the thread it should stop
			m_bThreadRunning = false;
			// Wait for the thread end
			m_oThreadRx.join();
			return -3;
		}
		else {
			// Indicate that the thread should be launch
			b_ThreadLaunching = true;
		}
	}
	
	if (uiIdEvent == EVENT_CSPIPORT_RECEIVEDATA) {
		// Connect the passed function
		m_oLastFctId = m_oSigOnRec.connect(oFct);
		
		// Checks if the function was well connected
		if(m_oLastFctId.connected()) {
			cout << "Callback correctly registered" << endl;
			if (b_ThreadLaunching) {

				// Indicate to the thread it should run
				m_bThreadRunning = true;
				// And launch the data reader thread
				boost::thread m_oThreadRx(boost::bind(&CSpiPort::OnReceive, this));
			}
			
			m_pData1 = pData1;
			m_pData2 = pData2;
			
			return 0;
		}
		else {
			cout << "The CallBack has not be registered" << endl;
			return -1;
		}
	}
	// Unknown event
	else {
		cout << "Error on IDEvent" << endl;
		return -2;
	}
}

/**
 *	\fn		int CSpiPort::UnregisterCallBack(unsigned int uiIdEvent, boost::signals2::connection oIdFct)
 *  \brief	Unregister a previously register callback using its connection Id
 *
 *  \param[in]	uiIdEvent : 		Integer that indicate the event ID on which the function is launched
 *  \param[in]	oIdFct : 		Object which indicates the connection ID of the function to unregister
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CSpiPort::UnregisterCallBack(unsigned int uiIdEvent, boost::signals2::connection oIdFct)
{
	// Init variables
	std::size_t iNumBefore;
	std::size_t iNumAfter;

	if (uiIdEvent == EVENT_CSPIPORT_RECEIVEDATA) {
		// Checks if there are some connected functions
		if (m_oSigOnRec.num_slots() == 0) {
			cout << "Aucune Callback n'est actuelement connectée sur l'évenement de reception des données sur l'Uart" << endl;
			return -1;
		}
		
		// Saved the number of connected function before the deconnexion
		iNumBefore = m_oSigOnRec.num_slots();
		
		// Disconnect the function using its connexion ID
		oIdFct.disconnect();
		
		// Saved the number of connected function after the deconnexion
		iNumAfter = m_oSigOnRec.num_slots();
	}
	// Unknown event
	else {
		cout << "Erreur dans l'ID de l'évenement" << endl;
		return -2;
	}
	
	// Checks if the function was well disconnected
	if (iNumBefore == iNumAfter+1) {
		//m_lListOnRec.erase(pItToDelete);
		cout << "Callback correctement déconnecté" << endl;
		
		// If there is no longer connected functions to the boost::signals
		if (m_oSigOnRec.num_slots() == 0) {
			// Indicate to the thread it should stop
			m_bThreadRunning = false;
			// Wait for the thread end
			m_oThreadRx.join();
		}
		
		return 0;
	}
	else {
		cout << "La Callback n'a pas pu être correctement déconnecté" << endl;
		return -2;
	}
}

/**
 *	\fn		int CSpiPort::UnregisterAllCallBacks(unsigned int uiIdEvent)
 *  \brief	Unregister all previously register callback on a specific event
 *
 *  \param[in]	uiIdEvent : 		Integer that indicate the event ID to empty
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CSpiPort::UnregisterAllCallBacks(unsigned int uiIdEvent)
{
	if (uiIdEvent == ALL_EVENTS || uiIdEvent == EVENT_CSPIPORT_RECEIVEDATA) {
		// Checks if there are some connected functions
		if (m_oSigOnRec.num_slots() == 0) {
			cout << "Aucune Callback n'est actuelement connectée" << endl;
			return -1;
		}
		else {
			// Disconnect all the functions form this specific events
			m_oSigOnRec.disconnect_all_slots();
			
			// Checks if all the functions was well disconnected
			if (m_oSigOnRec.num_slots() == 0)
			{
				cout << "Toutes les callbacks ont bien été deconnectées" << endl;
				
				// Indicate to the thread it should stop
				m_bThreadRunning = false;
				// Wait for the thread end
				m_oThreadRx.join();
				
				return 0;
			}
			else {
				cout << "Les callbacks n'ont pas été correctement deconnectées" << endl;
				return -2;
			}
		}
	}
	// Unknown event
	else {
		cout << "Erreur dans l'ID de l'évenement" << endl;
		return -3;
	}
}


/**
 *	\fn		void CSpiPort::OnReceive()
 *	\brief	Method who manage the launch of callbacks when data are received on the serial link
 */
void CSpiPort::OnReceive()
{
	// Init variables	
	int iNreaded, iRet;
	struct pollfd fds;
	//CEvent * pEvent = NULL;
	char *sRxBuffer = (char *) malloc (m_uiSampleCountbyCB);	
	memset (sRxBuffer, '\0', m_uiSampleCountbyCB);

	// Init poll
	fds.fd = m_fd;
	fds.events = POLLIN | POLLPRI;


	//printf("	read \n");
	// While we haven't said to the thread to stop
	do {
		// Launch the poll (which remplace a blockinck reading)
		poll(&fds,1,10);
		//printf("	read %d\n",iRet);
		// Create a new CEvent object
	//	pEvent = new CEvent;
		if (iRet == 0)
		{
			//time out
		}
		// Check why the poll has been finished
		else if (iRet > 0) {
			// If it's for a rising edge, that's means a data has been received on the uart
			if (fds.revents | POLLIN) {	//TODO Read On SPI
				
				iNreaded = read (m_fd, sRxBuffer,m_uiSampleCountbyCB );

				//printf("on	read %d %d \n",iNreaded,m_uiSampleCountbyCB );

				if (iNreaded != (int)m_uiSampleCountbyCB) {
					//cout<<"Erreur de lecture...\n";
					continue;
				}

				m_oSigOnRec(sRxBuffer, iNreaded, NULL, m_pData1, m_pData2);
							
			}
		}
		else 
		{
			cout<<"Error on Poll"<<endl;
		}
		
		// Delete the previously created CEvent object
	//	delete(pEvent);
	} while (m_bThreadRunning);
}


/**
 *	\fn		boost::signals2::connection CSpiPort::GetLastFctId()
 *  \brief	Send back the last connected callback connexion Id
 *
 *	\return	The last connected callback connexion Id
 */
boost::signals2::connection CSpiPort::GetLastFctId()
{
	return m_oLastFctId;
}

/**
 *	\fn		int CSpiPort::PrintConfig()
 *  \brief	print the Actual config
 *
 *	\return	The last connected callback connexion Id
 */
int CSpiPort::PrintConfig()
{

	cout<<"Sample Size : "<<m_uiSampleSize<<endl; 
	cout<<"Spi Channel : "<<m_uiSpiChannel<<endl;
	cout<<"Sample Count by CB : "<<m_uiSampleCountbyCB<<endl;
	cout<<"Sample Frequency : "<<m_uiSampleFrequency<<endl;
	cout<<"Cs Number : "<<m_uiCsNumber<<endl;

	return 0;
}

