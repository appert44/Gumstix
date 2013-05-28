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
#include <ctime>
using namespace std;

// Includes qt

// Includes application
#include "Gestionnaire.h"

/**
 * Constructeur
 */
Gestionnaire::Gestionnaire(boost::asio::io_service& io_service) {
	lm74_ = new LM74("/dev/spike");
	httpclient_ = new HTTPClient(io_service);
	pirstdlpsensor_ = new PIRSTDLPSensor();

}

/**
 * Destructeur
 */
Gestionnaire::~Gestionnaire() {
	delete pirstdlpsensor_;
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
double Gestionnaire::AcquireTemp() {
	lm74_->Open();
	double temperature = lm74_->Read();
	cout << "temperature=" << temperature << endl;
	return temperature;
	lm74_->Close();
}
void Gestionnaire::SendTemp(string str_temp, string time) {

	httpclient_->POST("enigmatic-cliffs-5746.herokuapp.com", "80",
			"/alfheimweb/measure/",
			"sensor_type=temp&device_sn=0008&time=" + time + "&value="
					+ str_temp);
	cout << "requete envoyee" << endl;
}
string Gestionnaire::DoubleToString(double temperature) {
	std::string str_temp;
	{
		std::ostringstream oss;
		oss << temperature;
		str_temp = oss.str();
	}
	return str_temp;
}

string Gestionnaire::GetTime() {
	char buffer[256];  //creation buffer reception heure
	time_t temps_act = time(NULL);
	strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S",
			localtime(&temps_act)); // formatage de l'heure pour Django
	asctime(localtime(&temps_act));
	return buffer;
}

double Gestionnaire::AcquirePresence()
{
	pirstdlpsensor_->Open();
	int valeur = pirstdlpsensor_ -> Presencerate();
	return valeur ;
}
void Gestionnaire::SendPresence(string presence, string time)
{
	httpclient_->POST("enigmatic-cliffs-5746.herokuapp.com", "80",
			"/alfheimweb/measure/",
			"sensor_type=presence&device_sn=0008&time=" + time + "&value="
					+ presence);
	cout << "requete envoyee" << endl;

}
// Methodes protegeesh

// Methodes privees

// Programme principal
// Si c'est la classe principale du projet, on place une fonction main()
// Dans ce cas, on peut supprimer les fichiers de tests unitaires
// ex :
int main(int argc, char *argv[]) {

	boost::asio::io_service io_service;
    boost::asio::io_service io_service2;
	// Construction de l'instance de la  classe principale
	cout << "Construction de l'instance de la classe principale Gestionnaire"
	<< endl;
	Gestionnaire *gestionnaire = new Gestionnaire(io_service);
	Gestionnaire *gestionnaire2 = new Gestionnaire(io_service2);

	string time = gestionnaire->GetTime();
	cout << "heure: " << time << endl;
	double valeur = gestionnaire->AcquirePresence();
	string presence = gestionnaire->DoubleToString(valeur);
	 double temperature = gestionnaire->AcquireTemp();
	if (temperature >50) {
		cout << "valeur fausse" << endl;
		return 0;
	}
	else {
		string str_temp = gestionnaire->DoubleToString(temperature);
		gestionnaire->SendPresence(presence,time);
				io_service.run();

		gestionnaire2->SendTemp(str_temp,time);
		io_service2.run();




		return 0;
	}
delete gestionnaire ;
delete gestionnaire2 ;
}
