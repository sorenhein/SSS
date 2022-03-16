/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_TOP_H
#define SSS_TOP_H

#include <string>

#include "Term.h"

using namespace std;


class Top: public Term
{
  private:

    string strEqual(const unsigned char oppsTops) const;

    string strInside(const unsigned char oppsTops) const;


  public:

    string str(const unsigned char oppsTops) const;

};

#endif
