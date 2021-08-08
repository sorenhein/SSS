/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_CARD_H
#define SSS_CARD_H

#include <string>

#include "../utils/Compare.h"

using namespace std;


class Card
{
  private:

    unsigned char rank;
    unsigned char depth; // 0 for highest card with that rank
    unsigned char number; // 0 for lowest card with that player
    unsigned char name;

  public:

    Card();

    void reset();

    void set(
      const unsigned char rankIn,
      const unsigned char depthIn,
      const unsigned char numberIn,
      const unsigned char nameIn);

    bool operator > (const Card& card2) const;
    bool operator >= (const Card& card2) const;
    bool operator == (const Card& card2) const;
    bool operator != (const Card& card2) const;
    bool operator <= (const Card& card2) const;
    bool operator < (const Card& card2) const;
    bool identical(const Card& card2) const;

    Compare compare(const Card& card2) const;

    bool isVoid() const;

    bool rankSame(const Card& card2) const;
    bool rankExceeds(const Card& card2) const;

    void operator *= (const Card& card2);
    void operator += (const Card& card2);

    unsigned char getRank() const;
    unsigned char getDepth() const;
    unsigned char getNumber() const;
    unsigned char getName() const;

    void flipDepth(const unsigned char maxDepth);

    string str(
      const string& side,
      const bool rankFlag = true) const;

    string strDebug(const string& side) const;
};

#endif
