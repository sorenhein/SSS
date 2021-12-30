/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

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
    WinnerMode mode;
    unsigned char rank;


    void multiplySide(
      Card& own,
      const Card& other,
      const WinnerMode otherMode,
      const unsigned bitmask);


  public:

    Winner();

    void reset();

    void set(
      const Side sideIn,
      const unsigned char rankIn,
      const unsigned char depthIn,
      const unsigned char numberIn,
      const unsigned char absNumberIn,
      const unsigned char nameIn);

    void set(
      const Side sideIn,
      const Card& card);

    bool empty() const;

    void setEmpty();

    bool operator == (const Winner& winner2) const;
    bool operator != (const Winner& winner2) const;

    void operator *= (const Winner& winner2);

    unsigned char getRank() const;

    unsigned char getAbsNumber() const;

    Compare compare(const Winner& winner2) const;

    Compare compareNonEmpties(const Winner& winner2) const;

    void flip();

    void update(const Play& play);

    void expand(const char rankAdder);

    bool rankExceeds(const Winner& winner2) const;

    string str() const;

    string strDebug() const;
};

#endif
