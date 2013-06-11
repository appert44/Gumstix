/**
 *  Copyright (C) 2013  kevin  (kevin@appert44.org)
 *  @file         PIRSTDLPSensor.cpp
 *  @brief        classe capteur de presence
 *  @version      0.1
 *  @date         28 mai 2013 08:27:34
 *
 *  Description detaillee du fichier PIRSTDLPSensor.cpp
 *  Fabrication   gcc (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3 
 *  @todo         Liste des choses restant a faire.
 *  @bug          28 mai 2013 08:27:34 - Aucun pour l'instant
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
#include <sys/types.h>	// open()
#include <sys/stat.h>		// open()
#include <fcntl.h>		// open()
#include <sys/ioctl.h>	// ioctl()
#include <string.h> // memset
#include <stdlib.h> // atof
#include "../ifsttar-drivers/ifsttar-synchronization-drv/ifsttar-synchronization-drv.h"
#include "../ifsttar-drivers/ifsttar-gpio-drv/ifsttar-gpio-drv.h"

// Includes qt

// Includes application
#include "PIRSTDLPSensor.h"

/**
 * Constructeur
 */
PIRSTDLPSensor::PIRSTDLPSensor() {
	cgpio_ = new CGPIO((char *) "/dev/gpio086", INPUT, 0);

}

/**
 * Destructeur
 */
PIRSTDLPSensor::~PIRSTDLPSensor() {
	delete cgpio_;
}

// Methodes publiques
// ex :
/**
 * Exemple de description d'une methode
 *
 * @param un_parametre  Description d'un parametre
 * @return              Description du retour
 */
// ReturnType PIRSTDLPSensor::NomMethode(Type parametre)
// {Read
// }
int PIRSTDLPSensor::Open(void) {
	int ret = cgpio_->Open();

	if (ret < 0) {
		cout << "open error : " << ret << endl;
		return -ERR_GPIO_DRIVER_OPEN;
	}


	return ret;
}

int PIRSTDLPSensor::Read(void) {
	int value = cgpio_->Read();
	cout << "valeur : " << value << endl;
	if (value == 0) {
		cout << "Presence positive" << endl;

	} else {
		cout << "Pas de presence" << endl;
	}
	return value;
}

double PIRSTDLPSensor::Presencerate() {
	int mesures[30];
	cout <<" Obtention de la presence sur 30s en cours " << endl ;
	for (int i = 0; i < 31; i++) {
		int value = cgpio_->Read();
		mesures[i] = value;
		sleep(1);
	}
	double presence = 0;
	for (int j = 0; j < 31; j++) {
		if (mesures[j] == 0)
		{

				presence ++ ;

	}
}


	presence = (presence * 100)/30;
			cout << " taux de presence : " << presence << "%" << endl ;
			return presence ;
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
//    cout << "Construction de l'instance de la classe principale PIRSTDLPSensor" << endl;
//    PIRSTDLPSensor *pirstdlpsensor = new PIRSTDLPSensor();
//    // Lancement de l'activité principale
//    return 0;
// }

