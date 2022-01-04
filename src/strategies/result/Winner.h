/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_WINNER_H
#define SSS_WINNER_H

#include "../../utils/table.h"
#include "../../utils/Card.h"
#include "../../utils/Compare.h"

struct Play;

using namespace std;


class Winner
{
  private:

    // If North has AKx (ranks 5 and 1), then rank is 5, depth is 1
    // if it's the A rather than the K, and number is 3 (x, K, A).
    // See also Ranks::setOrderTables().

    // Encoding is suitable for binary trickery in Winner.cpp

    enum WinnerMode
    {
      WIN_NOT_SET = 0,
      WIN_NORTH_ONLY = 1,
      WIN_SOUTH_ONLY = 2,
      WIN_BOTH = 3
    };

    Card north;
    Card south;
    unsigned char mode;
    unsigned char rank;


    void multiplySide(
      Card& own,
      const Card& other,
      const unsigned char otherMode,
      const unsigned char bitmask);


  public:

    Winner();

    void reset();

    void set(
      const Side sideIn,
      const Card& card);

    void setHigherOf(
      const Card& northIn,
      const Card& southIn);

    void setBoth(
      const Card& northIn,
      const Card& southIn);

    bool operator == (const Winner& winner2) const;

    bool operator != (const Winner& winner2) const;

    void operator *= (const Winner& winner2);

    Compare compareNonEmpties(const Winner& winner2) const;

    Compare compare(const Winner& winner2) const;

    void flip();

    void update(const Play& play);

    void expand(const char rankAdder);

    bool empty() const;

    unsigned char getRank() const;

    unsigned char getAbsNumber() const;

    string str() const;

    string strDebug() const;
};

#endif
