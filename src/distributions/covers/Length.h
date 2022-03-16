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

    // "oppsLength" is the opponents' actual length.
    // If this equals "upper", for example, the overall length
    // is of the form ">= lower" rather than [lower, upper].

    string strEqual(const unsigned char oppsLength) const;

    string strInside(const unsigned char oppsLength) const;
 

  public:

    string str(const unsigned char oppsLength) const;

};

#endif
