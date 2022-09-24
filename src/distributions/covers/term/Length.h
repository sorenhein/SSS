/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_LENGTH_H
#define SSS_LENGTH_H

#include <string>

#include "Term.h"

enum Opponent: unsigned;

using namespace std;


class Length: public Term
{
  friend class Top;

  private: 

    // "oppsLength" is the opponents' actual length.
    // If this equals "upper", for example, the overall length
    // is of the form ">= lower" rather than [lower, upper].


  public:

    // bool notVoid() const;

    // Opponent simplestOpponent(const unsigned char maximum) const;

    /*
    string strLengthBare(
      const unsigned char oppsLength,
      const Opponent simplestOpponent) const;
      */
};

#endif
