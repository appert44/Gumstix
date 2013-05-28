/**
 *  Copyright (C) 2013  plegal  (plegal@appert44.org)
 *  @file         HTTPClient.h
 *  @brief        Client HTTP
 *  @version      0.1
 *  @date         26 janv. 2013 14:32:20
 *
 *  @note         Voir la description detaillee explicite dans le fichier
 *                HTTPClient.cpp
 *                C++ Google Style :
 *                http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _HTTPCLIENT_H
#define _HTTPCLIENT_H

// Includes system C
//#include <stdint.h>  // definition des types int8_t, int16_t ...
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

// Includes qt

// Includes application

// Constantes
// ex :
// const int kDaysInAWeek = 7;

// Enumerations
// ex :
// enum Couleur
// {
//     kBlue = 0,
//     kWhite,
//     kRed,
// };

// Structures
// ex:
// struct UrlTableProperties
// {
//  string name;
//  int numEntries;
// }

// Declarations de classe avancees
// ex:
// class MaClasse;

/** @brief Client HTTP
 *  Description detaillee de la classe.
 */
class HTTPClient {
public:
	/**
	 * Constructeur
	 */
	HTTPClient(boost::asio::io_service& io_service);

	/**
	 * Destructeur
	 */
	~HTTPClient();

	// Methodes publiques de la classe
	// ex : ReturnType NomMethode(Type);

	// Pour les associations :
	// Methodes publiques setter/getter (mutateurs/accesseurs) des attributs prives
	// ex :
	// void setNomAttribut(Type nomAttribut) { nomAttribut_ = nomAttribut; }
	// Type getNomAttribut(void) const { return nomAttribut_; }
      void POST(const std::string& server,
    		  const std::string& port,
    		  const std::string& path,
    		  const std::string& data);
//    int Disconnect();

protected:
	// Attributs proteges

	// Methode protegees

private:
	// Attributs prives
	// ex :
	// Type nomAttribut_;
	tcp::resolver resolver_;
	tcp::socket socket_;
	boost::asio::streambuf request_;
	boost::asio::streambuf response_;

	// Methodes privees
	void HandleResolve(const boost::system::error_code& err,
						tcp::resolver::iterator endpoint_iterator);
	void HandleConnect(const boost::system::error_code& err);
	void HandleWriteRequest(const boost::system::error_code& err);
	void HandleReadStatusLine(const boost::system::error_code& err);
	void HandleReadHeaders(const boost::system::error_code& err);
	void HandleReadContent(const boost::system::error_code& err);
};

// Methodes publiques inline
// ex :
// inline void HTTPClient::maMethodeInline(Type valeur)
// {
//   
// }
// inline Type HTTPClient::monAutreMethode_inline_(void)
// {
//   return 0;
// }

#endif  // _HTTPCLIENT_H
