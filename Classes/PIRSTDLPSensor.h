/**
 *  Copyright (C) 2013  kevin  (kevin@appert44.org)
 *  @file         PIRSTDLPSensor.h
 *  @brief        classe capteur de presence
 *  @version      0.1
 *  @date         28 mai 2013 08:27:34
 *
 *  @note         Voir la description detaillee explicite dans le fichier
 *                PIRSTDLPSensor.cpp
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
#ifndef _PIRSTDLPSENSOR_H
#define _PIRSTDLPSENSOR_H

// Includes system C
//#include <stdint.h>  // definition des types int8_t, int16_t ...

// Includes system C++
#include "CGPIO.h"

// Includes qt

// Includes application

// Constantes
// ex :
// const int kDaysInAWeek = 7;
#define ERR_GPIO_DRIVER_OPEN   6000
#define ERR_GPIO_DRIVER_IOCTL  6001
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

/** @brief classe capteur de presence
 *  Description detaillee de la classe.
 */
class PIRSTDLPSensor
{
public :
    /**
     * Constructeur
     */
    PIRSTDLPSensor();
    /**
     * Destructeur
     */
    ~PIRSTDLPSensor();

    // Methodes publiques de la classe
    // ex : ReturnType NomMethode(Type);
int Open(void);
void Close(void);
int Read(void);
double Presencerate(void);
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
int fd_ ;

CGPIO * cgpio_ ;
    // Methodes privees
};

// Methodes publiques inline
// ex :
// inline void PIRSTDLPSensor::maMethodeInline(Type valeur)
// {
//   
// }
// inline Type PIRSTDLPSensor::monAutreMethode_inline_(void)
// {
//   return 0;
// }

#endif  // _PIRSTDLPSENSOR_H

