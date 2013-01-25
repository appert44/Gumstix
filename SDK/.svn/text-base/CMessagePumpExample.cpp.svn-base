/**
 *	\file	CMessagePumpExample.cpp
 *
 *	\brief 	This file is used to show CMessagePump application
 *	
 *	\author Théophile Marcadé
 *
 *	\version v0.1a
 *
 *	\date 22 june, 2012
 *
 */


#include "PEGASE_2.h"

using namespace std;

// Global variable
bool m_bRunning = true;			/**< \brief Boolean used to stop the program */

/** \brief	Function called by typping <Ctrl+C> */
void Stop (int signal)
{
	unused(signal);
	
	cout << "\nCTRL + C !!" << endl;
	m_bRunning = false;
}

/** \brief	Function called when received a kill */
void Term (int signal)
{
	unused(signal);
	
	cout << "Kill reçu !!" << endl;
	m_bRunning = false;
}

/**
 *	\brief	Demonstration program of using CMessagePump
 *	
 *	\param[in]	argc : 		Number of argument
 *	\param[in]	argv : 		Pointer of argument (here argv[1] = IP and argv[2] = Port)
 */
int main (int argc, char ** argv)
{

	int Port;
	CMessagePump* MsgPmp = NULL;
	
	char Test[2000];
	memset (Test, '\0', sizeof(Test));
	
	for(int i=0;i<12;i++)
	Test[i]=97+i;

	
	if(argc != 3) {
		cout<<"you need to take argument on execution : ./Example IP Port"<<endl;
		return -1;
	}

	Port = atoi(argv[2]);
	
	MsgPmp = new CMessagePump(argv[1],Port,1,10);
	
	MsgPmp->Start();
	
	do{
		sleep(3);
	
	
		if(MsgPmp->AddMessageToList(NULL,Test,strlen(Test),MESSAGE_ID_ASCII) < 0){
			cout<<"List full"<<endl;
		}
		


		cout<<"Message add to the list"<<endl;
	
	}while(m_bRunning);

	MsgPmp->Stop();
	delete MsgPmp;

	return 0;

}
