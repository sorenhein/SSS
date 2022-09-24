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

class Length;
struct Xes;
struct TopData;
enum Opponent: unsigned;

using namespace std;


class Top: public Term
{
  public:

    // Opponent simplestOpponent(const unsigned char maximum) const;

    string strTopBare(
      const TopData& oppsTopData,
      const Opponent simplestOpponent) const;
};

#endif
