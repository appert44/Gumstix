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
Gestionnaire::Gestionnaire()
{
	lm74_=new LM74("/dev/spike");
}

/**
 * Destructeur
 */
Gestionnaire::~Gestionnaire()
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
// ReturnType Gestionnaire::NomMethode(Type parametre)
// {
// }
double Gestionnaire::Acquire()
{
lm74_->Open();
double temperature=lm74_->Read();
cout << "temperature=" << temperature << endl ;
	return temperature;
}
// Methodes protegees

// Methodes privees

// Programme principal
// Si c'est la classe principale du projet, on place une fonction main()
// Dans ce cas, on peut supprimer les fichiers de tests unitaires
// ex :
 int main(int argc, char *argv[])
 {
    // Construction de l'instance de la classe principale
    cout << "Construction de l'instance de la classe principale Gestionnaire" << endl;
    Gestionnaire *gestionnaire = new Gestionnaire();
    // Lancement de l'activité principale
gestionnaire->Acquire();
    return 0;
 }

