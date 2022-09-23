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
  private:

    string strInside(
      const TopData& oppsTopData,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

    string strExactLengthEqual(
      const unsigned char distLength,
      const unsigned char oppsLength,
      const TopData& oppsTopData,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

    string strLengthRangeEqual(
      const TopData& oppsTopData,
      const Xes& xes,
      const Opponent simplestOpponent,
      const bool symmFlag) const;


  public:

    Opponent simplestOpponent(const unsigned char maximum) const;

    string strTop(
      const TopData& oppsTopData,
      const Opponent simplestOpponent,
      const bool symmFlag) const;

    string strTopBare(
      const TopData& oppsTopData,
      const Opponent simplestOpponent) const;

    string strTopBareEqual(
      const TopData& oppsTopData,
      const Opponent simplestOpponent) const;

    string strEqualWithLength(
      const Length& length,
      const unsigned char oppsLength,
      const TopData& oppsTopData,
      const Opponent simplestOpponent,
      const bool symmFlag) const;
};

#endif
