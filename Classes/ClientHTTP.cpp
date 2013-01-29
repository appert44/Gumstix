/**
 *  Copyright (C) 2013  kevin  (kevin@appert44.org)
 *  @file         ClientHTTP.cpp
 *  @brief        classe client http
 *  @version      0.1
 *  @date         25 janv. 2013 11:43:29
 *
 *  Description detaillee du fichier ClientHTTP.cpp
 *  Fabrication   gcc (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3 
 *  @todo         Liste des choses restant a faire.
 *  @bug          25 janv. 2013 11:43:29 - Aucun pour l'instant
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Includes system C

// Includes system C++
// A décommenter si besoin cout, cin, ...
#include <iostream>
using namespace std;
#include <stdio.h>

// Includes qt

// Includes application
#include "ClientHTTP.h"
#include "PEGASE_2.h"

void OnReceiveCallBack(char* sBuffer, int iBufferSize, CEvent * pEvent,
		void * pData1, void * pData2) {
	unused(pEvent);
	unused(pData1);
	unused(pData2);

	cout << "My callback called: " << sBuffer << ", size : " << iBufferSize
			<< endl;
}


void OnDeconnectionCallBack(char* pcServerIp, int iServerPort, CEvent* pEvent,
		void* pData1, void* pData2) {
	unused(pEvent);
	unused(pData1);
	unused(pData2);

	cout << "You have been disconnected of the server : Ip server : "
			<< pcServerIp << ", Server port : " << iServerPort << endl;
}


/**
 * Constructeur
 */
ClientHTTP::ClientHTTP()
{
	tcpIpClient_ = new CTcpIpClient(true, INFINITE);
	oOnCallBackRx_ = &OnReceiveCallBack;
		oOnCallBackDc_ = &OnDeconnectionCallBack;
}

/**
 * Destructeur
 */
ClientHTTP::~ClientHTTP()
{
	delete tcpIpClient_ ;
}

// Methodes publiques
// ex :
/**
 * Exemple de description d'une methode
 *
 * @param un_parametre  Description d'un parametre
 * @return              Description du retour
 */
// ReturnType ClientHTTP::NomMethode(Type parametre)
// {
// }
int ClientHTTP::Connect(string httpServer, int port)
{
int ret = tcpIpClient_->Connect((char*)httpServer.c_str(), port);
if (ret < 0) {

		cout << "HTTPClient > Connexion failed check your Server." << endl;
		return -1;

	}

	sleep(1);
    ret = tcpIpClient_->RegisterCallBack(EVENT_CTCPIPCLIENT_RECEIVEDATA ,oOnCallBackRx_, tcpIpClient_, NULL);				//Register Rx CallBack
    if(ret < 0)
    {

		cout<<"CTcpIpClient > Register failed"<<endl;
		return -1;

    }
return 0;
}


int ClientHTTP::POST(string hostname, string url, string data)
{
	int ret = -1;

	while	(!tcpIpClient_->IsConnected())
	{
		cout << "wait for connection..." << endl;
		sleep(1);
	}
	if (tcpIpClient_->IsConnected())
	{
		cout << "post request..." << endl;
		string request = "POST " + url + " HTTP/1.1\r\n";
		request+="Host: " + hostname + "\r\n";
		ostringstream oss;
		oss << data.length();
		request+="Content-Length:" + oss.str() + "\r\n\r\n";
		request+=data + "\r\n";
		tcpIpClient_->Write((char*)request.c_str(),request.length());
		ret = 0;
	}
	return ret;
}






int ClientHTTP::Disconnect()
{
	int ret = -1;

		if (tcpIpClient_->IsConnected())
		{
			cout << "Disconnect..." << endl;
			ret = tcpIpClient_->Close();
		}
		return ret;
}
// Methodes protegees

// Methodes privees

// Programme principal
// Si c'est la classe principale du projet, on place une fonction main()
// Dans ce cas, on peut supprimer les fichiers de tests unitaires
// ex :
// int main(int argc, char *argv[])
// {
//    // Construction de l'instance de la classe principale
//    cout << "Construction de l'instance de la classe principale ClientHTTP" << endl;
//    ClientHTTP *clienthttp = new ClientHTTP();
//    // Lancement de l'activité principale
//    return 0;
// }

