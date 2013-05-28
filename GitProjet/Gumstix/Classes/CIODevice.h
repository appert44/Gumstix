/**
 *	\file	CIODevice.h
 *
 *	\brief 	This header file is used to declare the CIODevice class and all its methods.
 *	
 *	\author Theophile Marcade
 *
 *	\version v0.1a
 *
 *	\date 25 April, 2012
 *
 */

#ifndef CIODEVICE_H
#define CIODEVICE_H

#include "CEvent.h"
#include "common.h"


/** 
 *	\brief	Prototype function of a Callback for PEGASE2.
 *
 *	\param	sBuffer :		Pointer on received data
 *	\param	iBuffer_size :	Size of received data
 *  \param	pEvent : 		Pointer of a CEvent object which contains informations about the event (timing and location information)
 *	\param	pData1 :		Optionnal Private pointer 1
 *	\param	pData2 :		Optionnal Private pointer 2
 */
typedef void (CallBack) (char* sBuffer, int iBuffer_size, CEvent * pEvent, void* pData1, void* pData2);

/**
 *	\class CIODevice
 *
 *	\brief 	CIODevice is an abstract class used to generalized methods and attributes of all its child classes.
 *	
 *	All child class should override the method they will need.
 *	All the method of CIODevice is for using IO port.
 *	
 *	\author Theophile Marcade
 *
 *	\version v0.1a
 *
 *	\date 26 April, 2012
 *
 */
class CIODevice
{
	protected:
	/* Attributes */
	/**************/
 	int m_fd;											/**< \brief File descriptor of the Class used port. */
	boost::signals2::connection m_oLastFctId;			/**< \brief Connexion Id of the last connected callback. */
	void * m_pData1;									/**< \brief User pointer 1*/
	void * m_pData2;									/**< \brief User pointer 2*/
	
	public:
	/* Methods  */
	/************/
	// Constructor
	CIODevice();
	// Destructor
	virtual ~CIODevice();

	// IO methods 
	virtual int Open () const;												// Open the specific IO port.
	virtual int Close () const;												// Close the specific IO port.
	virtual int Start () const;												// Start a process on IO port.
	virtual int Stop () const;												// Stop a process on IO port.
	virtual int Read (char * sBuffer, unsigned int uiBufferSize ) const;	// Read uiBufferSize bytes of sBuffer on specific IO port.
	virtual int Write (char * sBuffer, unsigned int uiBufferSize) const;	// Write uiBufferSize bytes of sBuffer on specific IO port.
	virtual int PrintConfig () const;										// Print the Config of the Class.

	// Registration callbacks managing methods
	virtual int RegisterCallBack (unsigned int uiEventId, boost::function<CallBack> oFct, void * pData1, void * pData2) const;	// Register a Callback which will be called when the event specified in EnventID will occur.
	virtual int UnregisterCallBack (unsigned int uiEventId, boost::signals2::connection oIdFct) const;		// Unregister a previously register callback using its connection Id.
	virtual int UnregisterAllCallBacks (unsigned int uiEventId) const;		// Unregister all previously register callback on a specific event.
  
	// Accessor
	virtual bool IsOpenned () const;										// Check if a specific IO port is openned.
	virtual unsigned int GetOpenMode () const;								// Get the open mode of specific IO port.
	virtual boost::signals2::connection GetLastFctId () const;				// Get the Id of the last function Registered to the signal boost.
};

#endif // CIODEVICE_H
