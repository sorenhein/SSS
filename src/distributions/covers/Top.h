/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TOP_H
#define SSS_TOP_H

#include <string>

#include "Term.h"

#include "../../utils/table.h"

using namespace std;


class Top: public Term
{
  private:

    string strEqual(
      const unsigned char oppsTops,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

    string strInside(
      const unsigned char oppsTops,
      const Opponent simplestOpponent,
      const bool symmFlag) const;


  public:

    string str(
      const unsigned char oppsTops,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

};

#endif
