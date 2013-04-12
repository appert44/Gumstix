/**
 *  Copyright (C) 2013  kevin  (kevin@appert44.org)
 *  @file         Gestionnaire.cpp
 *  @brief        gestionnaire
 *  @version      0.1
 *  @date         10 avril 2013 09:30:16
 *
 *  Description detaillee du fichier Gestionnaire.cpp
 *  Fabrication   gcc (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3 
 *  @todo         Liste des choses restant a faire.
 *  @bug          10 avril 2013 09:30:16 - Aucun pour l'instant
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


// Includes qt

// Includes application
#include "Gestionnaire.h"

/**
 * Constructeur
 */
Gestionnaire::Gestionnaire(boost::asio::io_service& io_service)
{
	lm74_=new LM74("/dev/spike");
	httpclient_ = new HTTPClient(io_service);

}

/**
 * Destructeur
 */
Gestionnaire::~Gestionnaire()
{
	delete httpclient_;
	delete lm74_;
}

// Methodes publiques
// ex :
/**
 * Exemple de description d'une methode
 *
 * @param un_parametre  Description d'un parametre
 * @return              Description du retour
 */
// ReturnType Gestionnaire::NomMethode(Type parametre)
// {
// }
double Gestionnaire::Acquire()
{
lm74_->Open();
double temperature=lm74_->Read();
cout << "temperature=" << temperature << endl ;
	return temperature;
	lm74_->Close();
}
void Gestionnaire::Send()
{
double temperature=this->Acquire();
std::string str_temp ;
{
	std::ostringstream oss;
	oss << temperature ;
	str_temp = oss.str();
}

httpclient_->POST("enigmatic-cliffs-5746.herokuapp.com","80","/alfheimweb/measure/","sensor_type=temp&device_sn=0001&time=04/12/13&value="+str_temp);
	cout << "requete envoyee" << endl;
}
// Methodes protegees

// Methodes privees

// Programme principal
// Si c'est la classe principale du projet, on place une fonction main()
// Dans ce cas, on peut supprimer les fichiers de tests unitaires
// ex :
 int main(int argc, char *argv[])
 {

	 boost::asio::io_service io_service;
    // Construction de l'instance de la  classe principale
    cout << "Construction de l'instance de la classe principale Gestionnaire" << endl;
    Gestionnaire *gestionnaire = new Gestionnaire(io_service);
    int i=0;
    	     while (i < 1)
    	    {
    gestionnaire->Send();
    io_service.run();
    cout << "Prochain envoie de mesure dans 10sec" << endl;
    sleep(10);
   }
    return 0;
 }

