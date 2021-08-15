/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_WINNER_H
#define SSS_WINNER_H

#include "../../const.h"
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

    Compare declarerPrefersSide(
      const Card& own,
      const Card& other,
      const WinnerMode otherMode,
      const unsigned bitmask) const;


  public:

    Winner();

    ~Winner();

    void reset();

    void set(
      const WinningSide sideIn,
      const unsigned char rankIn,
      const unsigned char depthIn,
      const unsigned char number,
      const unsigned char nameIn);

    void set(
      const WinningSide sideIn,
      const Card& card);

    bool empty() const;

    void setEmpty();

    bool operator == (const Winner& winner2) const;
    bool operator != (const Winner& winner2) const;

    void operator *= (const Winner& winner2);
    void operator += (const Winner& winner2);

    unsigned char getRank() const;

    Compare declarerPrefers(const Winner& winner2) const;

    void flip();

    void update(const Play& play);

    bool rankExceeds(const Winner& winner2) const;

    string str() const;

    string strDebug() const;
};

#endif
