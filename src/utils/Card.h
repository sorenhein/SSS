/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CARD_H
#define SSS_CARD_H

#include <vector>
#include <string>

#include "../utils/Compare.h"

using namespace std;


class Card
{
  private:

    unsigned char rank;
    unsigned char number; // 1 for lowest card with that player
    unsigned char absNumber; // 1 for lowest card in that combination
    unsigned char name;

  public:

    Card();

    void reset();

    void set(
      const unsigned char rankIn,
      const unsigned char numberIn,
      const unsigned char absNumberIn,
      const unsigned char nameIn);

    void updateName(const unsigned char nameIn);

    bool operator > (const Card& card2) const;
    bool operator != (const Card& card2) const;

    Compare compare(const Card& card2) const;

    bool isVoid() const;

    bool rankSame(const Card& card2) const;
    bool rankExceeds(const Card& card2) const;

    void operator *= (const Card& card2);
    void operator += (const Card& card2);

    unsigned char getRank() const;
    unsigned char getNumber() const;
    unsigned char getAbsNumber() const;
    unsigned char getName() const;

    void expand(const char rankAdder);

    string str() const;

    string strDebug(const string& side) const;
};

#endif
