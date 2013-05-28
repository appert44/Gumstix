/**
 *  Copyright (C) 2013  plegal  (plegal@appert44.org)
 *  @file         LM74.cpp
 *  @brief        Classe LM74
 *  @version      0.1
 *  @date         08 avril 2013 08:38:53
 *
 *  Description detaillee du fichier LM74.cpp
 *  Fabrication   gcc (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3 
 *  @todo         Liste des choses restant a faire.
 *  @bug          08 avril 2013 08:38:53 - Aucun pour l'instant
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
 #include <sys/types.h>	// open()
 #include <sys/stat.h>		// open()
 #include <fcntl.h>		// open()
 #include <sys/ioctl.h>	// ioctl()
#include <stdio.h> // printf
#include <string.h> // memset
#include <stdlib.h> // atof

// Includes system C++
// A décommenter si besoin cout, cin, ...
 #include <iostream>
 using namespace std;

// Includes qt

// Includes application
#include "LM74.h"
#include "../spike/spike.h"

/**
 * Constructeur
 */
LM74::LM74(string device_path)
{
	device_path_ = device_path;
	fd_ = 0;
}

/**
 * Destructeur
 */
LM74::~LM74()
{
}

// Methodes publiques
// ex :
/**
 * Exemple de description d'une methode
 *
 * @param un_parametre  Description d'un parametre
 * @return              Description du retour
 */
// ReturnType LM74::NomMethode(Type parametre)
// {
// }

int LM74::Open(void)
{
	int retval = 0;
	fd_ = open(device_path_.c_str(), O_RDWR);
	if (fd_ < 0)
	{
		cout << "open error : " << fd_ << endl;
		return -ERR_SPIKE_DRIVER_OPEN;
	}
<<<<<<< HEAD
	//cout << "fd = " << fd_ << endl;
=======
	cout << "fd = " << fd_ << endl;
>>>>>>> 6ad2973104ac9e543faf601dd99f13e6436ed8cf
	return retval;
}

void LM74::Close(void)
{
	if (fd_ > 0)
		close(fd_);
}

int  LM74::SetContConvMode()
{
	int retval = 0;
	if (fd_ > 0)
	{
		if ( ioctl(fd_, LM74_IOCTL_SET_MODE, CONTCONV_MODE) < 0)
		{
			cout << "ioctl error : " << fd_ << endl;
			retval = -ERR_SPIKE_DRIVER_IOCTL;
		}
	}
	else
	{
		cout << "open error : " << fd_ << endl;
		retval = -ERR_SPIKE_DRIVER_OPEN;
	}

	return retval;
}

int LM74::SetShutdownMode()
{
	int retval = 0;

	if (fd_ > 0)
		{
			if ( ioctl(fd_, LM74_IOCTL_SET_MODE, SHUTDOWN_MODE) < 0)
			{
				cout << "ioctl error : " << fd_ << endl;
				retval = -ERR_SPIKE_DRIVER_IOCTL;
			}
		}
		else
		{
			cout << "open error : " << fd_ << endl;
			retval = -ERR_SPIKE_DRIVER_OPEN;
		}

		return retval;
}

double LM74::Read()
{
	double retval = 0;
	unsigned short temp = 0;
	//struct spike_dev spike_dev;

	char mesure[2];   //Initialization reception buffer
	memset (mesure, '\0', 2);
	 read(fd_, mesure, 2) ;

	// printf("mesure 0x%2x 0x%2x",mesure[0],mesure[1]);
	temp += mesure[0] << 8 ;
	//printf ("temp 0x%4x \n",temp);
	temp += mesure[1] ;
	temp = temp >> 3 ;
	//printf ("temp 0x%4x \n",temp);
	retval = (double)temp * RATIO ;
	//printf ("retval %f",retval);

return retval;

	//return retval;
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
//    cout << "Construction de l'instance de la classe principale LM74" << endl;
//    LM74 *lm74 = new LM74();
//    // Lancement de l'activité principale
//    return 0;
// }

