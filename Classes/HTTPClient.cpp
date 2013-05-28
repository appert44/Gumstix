/**
 *  Copyright (C) 2013  kmoreau  (kmoreaul@appert44.org)
 *  @file         HTTPClient.cpp
 *  @brief        Client HTTP
 *  @version      0.1
 *  @date         26 janv. 2013 14:32:20
 *
 *  Description detaillee du fichier HTTPClient.cpp
 *  Fabrication   gcc (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3 
 *  @todo         Liste des choses restant a faire.
 *  @bug          26 janv. 2013 14:32:20 - Aucun pour l'instant
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
#include <boost/bind/bind.hpp>

// Includes qt

// Includes application
#include "HTTPClient.h"

/**
 * Constructeur
 */
HTTPClient::HTTPClient(boost::asio::io_service& io_service) :
		resolver_(io_service), socket_(io_service) {
	}
/**
 * Destructeur
 */
HTTPClient::~HTTPClient() {
//	delete tcpIpClient_;
}

// Methodes publiques
// ex :
/**
 * Exemple de description d'une methode
 *
 * @param un_parametre  Description d'un parametre
 * @return              Description du retour
 */
// ReturnType HTTPClient::NomMethode(Type parametre)
// {
// }
//int HTTPClient::Connect(string httpServer, int port) {
//    return 0;
//}
//
void HTTPClient::POST(const std::string& server,
		  const std::string& port,
		  const std::string& path,
		  const std::string& data)
{
// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	std::ostream request_stream(&request_);
	request_stream << "POST " << path << " HTTP/1.1\r\n";
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n";
	ostringstream dataLen; // creation string to stend
	dataLen << data.length();  // length of datas
	request_stream << "Content-Length:" << dataLen.str() << "\r\n\r\n";
	request_stream << data << "\r\n";
	// Start an asynchronous resolve to translate the server and service names
	// into a list of endpoints.
	tcp::resolver::query query(server, port); // DNS
	resolver_.async_resolve(query,
			boost::bind(&HTTPClient::HandleResolve, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::iterator));   // DNS

}

//int HTTPClient::Disconnect()
//{
//	return 0;
//}
// Methodes protegees
// Methodes privees
void HTTPClient::HandleResolve(const boost::system::error_code& err,
		tcp::resolver::iterator endpoint_iterator) {
	if (!err) {
		// Attempt a connection to each endpoint in the list until we
		// successfully establish a connection.
		boost::asio::async_connect(socket_, endpoint_iterator,
				boost::bind(&HTTPClient::HandleConnect, this,
						boost::asio::placeholders::error));
	} else {
		std::cout << "Error: " << err.message() << "\n";
	}
}

void HTTPClient::HandleConnect(const boost::system::error_code& err) {
	if (!err) {
		// The connection was successful. Send the request.
		boost::asio::async_write(socket_, request_,
				boost::bind(&HTTPClient::HandleWriteRequest, this,
						boost::asio::placeholders::error));
	} else {
		std::cout << "Error: " << err.message() << "\n";
	}
}

void HTTPClient::HandleWriteRequest(const boost::system::error_code& err) {
	if (!err) {
		// Read the response status line. The response_ streambuf will
		// automatically grow to accommodate the entire line. The growth may be
		// limited by passing a maximum size to the streambuf constructor.
		boost::asio::async_read_until(socket_, response_, "\r\n",
				boost::bind(&HTTPClient::HandleReadStatusLine, this,
						boost::asio::placeholders::error));
	} else {
		std::cout << "Error: " << err.message() << "\n";
	}
}

void HTTPClient::HandleReadStatusLine(const boost::system::error_code& err) {
	if (!err) {
		// Check that response is OK.
		std::istream response_stream(&response_);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
			std::cout << "Invalid response\n";
			return;
		}
		if (status_code != 200) {
			std::cout << "Response returned with status code ";
			std::cout << status_code << "\n";
			return;
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
				boost::bind(&HTTPClient::HandleReadHeaders, this,
						boost::asio::placeholders::error));
	} else {
		std::cout << "Error: " << err << "\n";
	}
}

void HTTPClient::HandleReadHeaders(const boost::system::error_code& err) {
	if (!err) {
		// Process the response headers.
		std::istream response_stream(&response_);
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
			std::cout << header << "\n";
		std::cout << "\n";

		// Write whatever content we already have to output.
		if (response_.size() > 0)
			std::cout << &response_;

		// Start reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
				boost::asio::transfer_at_least(1),
				boost::bind(&HTTPClient::HandleReadContent, this,
						boost::asio::placeholders::error));
	} else {
		std::cout << "Error: " << err << "\n";
	}
}

void HTTPClient::HandleReadContent(const boost::system::error_code& err) {
	if (!err) {
		// Write all of the data that has been read so far.
		std::cout << &response_;

		// Continue reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
				boost::asio::transfer_at_least(1),
				boost::bind(&HTTPClient::HandleReadContent, this,
						boost::asio::placeholders::error));
	} else if (err != boost::asio::error::eof) {
		std::cout << "Error: " << err << "\n";
	}
}

// Programme principal
// Si c'est la classe principale du projet, on place une fonction main()
// Dans ce cas, on peut supprimer les fichiers de tests unitaires
// ex :
// int main(int argc, char *argv[])
// {
//    // Construction de l'instance de la classe principale
//    cout << "Construction de l'instance de la classe principale HTTPClient" << endl;
//    HTTPClient *httpclient = new HTTPClient();
//    // Lancement de l'activité principale
//    return 0;
// }
