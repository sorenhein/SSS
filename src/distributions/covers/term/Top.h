/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TOP_H
#define SSS_TOP_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "Term.h"

#include "../../../utils/table.h"

class Length;
struct Xes;

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

    string strExactLengthEqual(
      const unsigned char distLength,
      const unsigned char oppsLength,
      const unsigned char oppsTops,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

    string strLengthRangeEqual(
      const unsigned char oppsTops,
      const Xes& xes,
      const Opponent simplestOpponent,
      const bool symmFlag) const;


  public:

    string strTop(
      const unsigned char oppsTops,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

    string strWithLength(
      const Length& length,
      const unsigned char oppsLength,
      const unsigned char oppsTops,
      const Opponent simplestOpponent,
      const bool symmFlag) const;
};

#endif
