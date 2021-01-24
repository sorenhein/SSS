#ifndef SSS_WINNER_H
#define SSS_WINNER_H

#include <vector>
#include <list>
#include <cassert>

#include "Winner.h"

using namespace std;


enum WinningSide
{
  WIN_NORTH = 0,
  WIN_SOUTH = 1,
  WIN_EITHER = 2,
  WIN_NONE = 3
};


class Winner
{
  private:

    // If North has AKx (ranks 5 and 1), then rank is 5, depth is 1
    // if it's the A rather than the K, and number is 3 (x, K, A).
    // See also Ranks::setOrderTables().

    WinningSide side;
    unsigned rank;
    unsigned depth;
    unsigned number;


  public:

    Winner();

    ~Winner();

    void reset();

    void set(
      const unsigned rankIn,
      const WinningSide sideIn);

    void setFull(
      const WinningSide sideIn,
      const unsigned rankIn,
      const unsigned depthIn,
      const unsigned number);

    string str() const;

    string strEntry() const;
};

#endif
