/**
 *  Copyright (C) 2013  plegal  (plegal@appert44.org)
 *  @file         LM74.h
 *  @brief        Classe LM74
 *  @version      0.1
 *  @date         08 avril 2013 08:38:53
 *
 *  @note         Voir la description detaillee explicite dans le fichier
 *                LM74.cpp
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
#ifndef _LM74_H
#define _LM74_H

// Includes system C
//#include <stdint.h>  // definition des types int8_t, int16_t ...

// Includes system C++
#include <string>
using namespace std;
// Includes qt

// Includes application

// Constantes
// ex :
// const int kDaysInAWeek = 7;

#define ERR_SPIKE_DRIVER_OPEN		6000
#define ERR_SPIKE_DRIVER_IOCTL	 	6001
#define RATIO                       0.0625
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

/** @brief Classe LM74
 *  Description detaillee de la classe.
 */
class LM74
{
public :
    /**
     * Constructeur
     */
    LM74(string device_path);
    /**
     * Destructeur
     */
    ~LM74();

    // Methodes publiques de la classe
    // ex : ReturnType NomMethode(Type);

    int Open(void);
    void Close(void);
    int SetContConvMode();
    int SetShutdownMode();
    double Read();

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
    string device_path_;
    int fd_;

    // Methodes privees
};

// Methodes publiques inline
// ex :
// inline void LM74::maMethodeInline(Type valeur)
// {
//   
// }
// inline Type LM74::monAutreMethode_inline_(void)
// {
//   return 0;
// }

#endif  // _LM74_H

