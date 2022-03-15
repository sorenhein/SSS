/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_LENGTH_H
#define SSS_LENGTH_H

#include <string>

#include "Term.h"

using namespace std;


class Length: public Term
{
  private: 

    string strEqual(const unsigned char lenActual) const;

    string strInside(const unsigned char lenActual) const;
 

  public:

    string str(const unsigned char lenActual) const;

};

#endif
