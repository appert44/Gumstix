/**
 *	\file	CIODevice.cpp
 *
 *	\brief 	This file is used to describe the comportment of each method of the CIODevice class
 *	
 *	\author Theophile Marcade
 *
 *	\version v0.1a
 *
 *	\date 24 April, 2012
 *
 */

#include "CIODevice.h"

using namespace std;

/**
 * \fn 		CIODevice :: CIODevice ()
 * \brief 	Constructor of the CIODevice
 */
CIODevice :: CIODevice ()
{   
	m_fd = -1;
}

/**
 * \fn 		CIODevice :: ~CIODevice ()
 * \brief 	Destructor of the CIODevice
 */
CIODevice :: ~CIODevice ()
{
	close(m_fd);
}

/**
 * \fn 		int CIODevice :: Open () const
 * \brief 	Open the specific IO port
 *
 * \return 	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: Open () const
{
	cout << CIODEVICE_PRINT_NAME << "Open () not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_OPEN_NOT_IMPLEMENTED;
}

/**
 * \fn 		int CIODevice :: Close () const
 * \brief	Close the specific IO port
 *
 * \return 	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: Close () const
{
	cout << CIODEVICE_PRINT_NAME << "Close () not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_CLOSE_NOT_IMPLEMENTED;
}
	
/**
 * \fn 		int CIODevice :: Start () const
 * \brief 	Start a process on IO port
 *
 * \return 	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: Start () const
{
	cout << CIODEVICE_PRINT_NAME << "Start () not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_START_NOT_IMPLEMENTED;
}
	
/**
 * \fn 		int CIODevice :: Stop () const 
 * \brief	Stop a process on IO port
 *
 * \return 	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: Stop () const 
{
	cout << CIODEVICE_PRINT_NAME << "Stop() not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_STOP_NOT_IMPLEMENTED;
}

/**
 * \fn 		int CIODevice :: Read (char * sBuffer, unsigned int uiBufferSize) const
 * \brief 	Read uiBufferSize bytes of sBuffer on specific IO port
 *
 * \param[in]	sBuffer : 			Buffer which contains the message to Write which contains the message to Write
 * \param[in]	uiBufferSize : 		Integer that indicate the size of the message to Write
 *
 * \return 	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: Read (char * sBuffer, unsigned int uiBufferSize) const
{
	unused (sBuffer);
	unused (uiBufferSize);
	
	cout << CIODEVICE_PRINT_NAME << "Read (char *, unsigned int ) not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_READ_NOT_IMPLEMENTED;
}
 
/**
 * \fn 		int CIODevice :: Write (char * sBuffer, unsigned int uiBufferSize) const
 * \brief 	Write uiBufferSize bytes of sBuffer on specific IO port
 *
 * \param[in] 	sBuffer : 			Buffer which contains the message to Write
 * \param[in]	uiBufferSize : 		Integer that indicate the size of the message to Write
 *
 * \return 	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: Write (char * sBuffer, unsigned int uiBufferSize) const
{
	unused (sBuffer);
	unused (uiBufferSize);
	
	cout << CIODEVICE_PRINT_NAME << "Write (char *, unsigned int) not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_WRITE_NOT_IMPLEMENTED;
}

/**
 * \fn		int CIODevice :: PrintConfig () const
 * \brief	Print the Config of the Class
 *
 * \return 	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: PrintConfig () const
{
	cout << CIODEVICE_PRINT_NAME << "PrintConfig () not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_PRINTCONFIG_NOT_IMPLEMENTED;
}

/**
 *	\fn		int CIODevice :: RegisterCallBack (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2) const
 *  \brief	Register a Callback which will be called when the event specified in EnventID will occur
 *
 *  \param[in]	uiEventId : 	Integer that indicate the event ID on which the function will be launched
 *  \param[in]	oFct : 			Object which indicates the function to launch when a event occured
 *  \param[in]	pData1 : 		User pointer WARNING in some class it could be used by the class, read the RegisterCallBack comments of these class
 *  \param[in]	pData2 : 		User pointer
 *
 *	\return	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: RegisterCallBack (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2) const
{
	unused(uiEventId);
	unused(oFct);
	unused(pData1);
	unused(pData2);
	
	cout << CIODEVICE_PRINT_NAME << "RegisterCallBack (unsigned int, boost::function<CallBack>) not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_REGISTERCALLBACK_NOT_IMPLEMENTED;
}

/**
 *	\fn		int CIODevice :: UnregisterCallBack (unsigned int uiEventId, boost::signals2::connection oIdFct) const
 *  \brief	Unregister a previously register callback using its connection Id
 *
 *  \param[in]	uiEventId : 	Integer that indicate the event ID on which the function is launched
 *  \param[in]	oIdFct : 		Object which indicates the connection ID of the function to unregister
 *
 *	\return A negative number if an error occured, 0 otherwise
 */
int CIODevice :: UnregisterCallBack (unsigned int uiEventId, boost::signals2::connection oIdFct) const
{
	unused(uiEventId);
	unused(oIdFct);
	
	cout << CIODEVICE_PRINT_NAME << "UnregisterCallBack (unsigned int, boost::signals2::connection) not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_UNREGISTERCALLBACK_NOT_IMPLEMENTED;	
}

/**  
 * \fn		int CIODevice :: UnregisterAllCallBacks (unsigned int uiEventId) const
 * \brief 	Unregister all previously register callback on a specific event
 *
 * \param[in]  	uiEventId : 	Integer that indicate the event ID to clear
 *
 * \return	A negative number if an error occured, 0 otherwise
 */
int CIODevice :: UnregisterAllCallBacks (unsigned int uiEventId) const
{
	unused(uiEventId);
	
	cout << CIODEVICE_PRINT_NAME << "UnregisterAllCallBack (unsigned int) not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_UNREGISTERALLCALLBACKS_NOT_IMPLEMENTED;
}

/** 
 * \fn 		bool CIODevice :: IsOpenned () const
 * \brief	Check if a specific IO port is openned
 *
 * \return 	True if the IO port is openned, false otherwise
 */
bool CIODevice :: IsOpenned () const
{
	cout << CIODEVICE_PRINT_NAME << "IsOpenned () not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_ISOPENNED_NOT_IMPLEMENTED;
}

/**
 * \fn 		unsigned int CIODevice :: GetOpenMode () const
 * \brief 	Get the open mode of specific IO port
 *
 * \return 	A negative number if an error occured, 0 otherwise
 */
unsigned int CIODevice :: GetOpenMode () const
{
	cout << CIODEVICE_PRINT_NAME << "GetOpenMode () not implemented for this class" << endl;
	
	return -ERR_CIODEVICE_GETOPENMODE_NOT_IMPLEMENTED;
}
 
/**
 * \fn 		boost::signals2::connection CIODevice :: GetLastFctId () const
 * \brief 	Get the Id of the last function Registered to the signal boost
 *
 * \return	The last connected callback connexion Id
 */
boost::signals2::connection CIODevice :: GetLastFctId() const
{
	cout << CIODEVICE_PRINT_NAME << "GetLastFctId () not implemented for this class" << endl;
	
	// TODO : send back something that the user can understand (error code, NULL pointer, ...)
	return m_oLastFctId;
}
