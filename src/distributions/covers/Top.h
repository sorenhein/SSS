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

class Length;

using namespace std;


class Top: public Term
{
  private:

    struct Xes
    {
      unsigned char westMax, westMin;
      unsigned char eastMax, eastMin;
      string strWest, strEast;

      void set(
        const unsigned char distLengthLower,
        const unsigned char distLengthUpper,
        const unsigned char topsExact,
        const unsigned char oppsLength,
        const unsigned char oppsTops)
      {
        westMax = distLengthUpper - topsExact;
        westMin = distLengthLower - topsExact;

        eastMax = (oppsLength - distLengthLower) - (oppsTops - topsExact);
        eastMin = (oppsLength - distLengthUpper) - (oppsTops - topsExact);

        strWest = string(westMin, 'x') +
          "(" + string(westMax - westMin, 'x') + ")";
        strEast = string(eastMin, 'x') +
          "(" + string(eastMax - eastMin, 'x') + ")";
      };
 
      string str() const
      {
        stringstream ss;
 
        ss << "coverXes: " <<
          westMin << "-" << westMax << ", " <<
          eastMin << "-" << eastMax << ", " <<
          strWest << ", " << strEast << "\n";
 
        return ss.str();
      };
    };


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
