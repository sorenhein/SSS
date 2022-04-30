/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_LENGTH_H
#define SSS_LENGTH_H

#include <string>

#include "Term.h"

#include "../../../utils/table.h"

using namespace std;


class Length: public Term
{
  friend class Top;

  private: 

    // "oppsLength" is the opponents' actual length.
    // If this equals "upper", for example, the overall length
    // is of the form ">= lower" rather than [lower, upper].

    string strEqual(
      const unsigned char oppsLength,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

    string strInside(
      const unsigned char oppsLength,
      const Opponent simplestOpponent,
      const bool symmFlag) const;
 

  public:

    bool notVoid() const;

    string strLengthBare(
      const unsigned char oppsLength,
      const Opponent simplestOpponent) const;

    string strLength(
      const unsigned char oppsLength,
      const Opponent simplestOpponent,
      const bool symmFlag) const;
};

#endif
