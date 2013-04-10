/**
 *  Copyright (C) 2013  kevin  (kevin@appert44.org)
 *  @file         Gestionnaire.h
 *  @brief        gestionnaire
 *  @version      0.1
 *  @date         10 avril 2013 09:30:16
 *
 *  @note         Voir la description detaillee explicite dans le fichier
 *                Gestionnaire.cpp
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
#ifndef _GESTIONNAIRE_H
#define _GESTIONNAIRE_H

// Includes system C
//#include <stdint.h>  // definition des types int8_t, int16_t ...

// Includes system C++

// Includes qt

// Includes application
#include "LM74.h"
#include "HTTPClient.h"
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

/** @brief gestionnaire
 *  Description detaillee de la classe.
 */
class Gestionnaire
{
public :
    /**
     * Constructeur
     */
    Gestionnaire();
    /**
     * Destructeur
     */
    ~Gestionnaire();

    // Methodes publiques de la classe
    // ex : ReturnType NomMethode(Type);
double Acquire();
    // Pour les associations :
    // Methodes publiques setter/getter (mutateurs/accesseurs) des attributs prives
    // ex :
    // void setNomAttribut(Type nomAttribut) { nomAttribut_ = nomAttribut; }
    // Type getNomAttribut(void) const { return nomAttribut_; }

protected :
    // Attributs proteges

    // Methode protegees

private :
    // Attributs prives
    // ex :
    // Type nomAttribut_;
LM74 *lm74_;
HTTPClient *httpclient_;
string device_path_;
    // Methodes privees
};

// Methodes publiques inline
// ex :
// inline void Gestionnaire::maMethodeInline(Type valeur)
// {
//   
// }
// inline Type Gestionnaire::monAutreMethode_inline_(void)
// {
//   return 0;
// }

#endif  // _GESTIONNAIRE_H

