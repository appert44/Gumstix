/**
 *  Copyright (C) 2013  kevin  (kevin@appert44.org)
 *  @file         ClientHTTP.h
 *  @brief        classe client http
 *  @version      0.1
 *  @date         25 janv. 2013 11:43:29
 *
 *  @note         Voir la description detaillee explicite dans le fichier
 *                ClientHTTP.cpp
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
#ifndef _CLIENTHTTP_H
#define _CLIENTHTTP_H

// Includes system C
//#include <stdint.h>  // definition des types int8_t, int16_t ...

// Includes system C++

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
//  int num_entries;
// }

// Declarations de classe avancees
// ex:
// class MaClasse;

/** @brief classe client http
 *  Description detaillee de la classe.
 */
class ClientHTTP
{
public :
    /**
     * Constructeur
     */
    ClientHTTP();
    /**
     * Destructeur
     */
    ~ClientHTTP();

    // Methodes publiques de la classe
    // ex : ReturnType NomMethode(Type);
void Connexion();
    // Pour les associations :
    // Methodes publiques setter/getter (mutateurs/accesseurs) des attributs prives
    // ex :
    // void set_nom_attribut_(Type nom_attribut) { nom_attribut_ = nom_attribut; }
    // Type get_nom_attribut_(void) const { return nom_attribut_; }

protected :
    // Attributs proteges

    // Methode protegees

private :
    // Attributs prives
    // ex :
    // Type nom_attribut_;

    // Methodes privees
};

// Methodes publiques inline
// ex :
// inline void ClientHTTP::ma_methode_inline(Type valeur)
// {
//   
// }
// inline Type ClientHTTP::mon_autre_methode_inline_(void)
// {
//   return 0;
// }

#endif  // _CLIENTHTTP_H

