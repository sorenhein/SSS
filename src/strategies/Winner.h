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

    enum WinnerMode
    {
      WIN_NORTH_ONLY = 0,
      WIN_SOUTH_ONLY = 1,
      WIN_NS_DECIDE = 2,
      WIN_EW_DECIDE = 3,
      WIN_EMPTY = 4, // No rank winner, but something is known
      WIN_NOT_SET = 5 // Neutral state
    };

    struct SideWinner
    {
      unsigned rank;
      unsigned depth;
      unsigned number;

      void reset()
      {
        rank = 0;
        depth = 0;
        number = 0;
      }

      void set(
        const unsigned rankIn,
        const unsigned depthIn,
        const unsigned numberIn)
      {
        rank = rankIn;
        depth = depthIn;
        number = numberIn;
      }
    };

    SideWinner north;
    SideWinner south;

    WinnerMode mode;


    string strSingleSided(
      const string& name,
      const SideWinner& winner) const;


  public:

    Winner();

    ~Winner();

    void reset();

    void set(
      const WinningSide sideIn,
      const unsigned rankIn,
      const unsigned depthIn,
      const unsigned number);

    void flip();

    void update(
      const vector<Winner>& northOrder,
      const vector<Winner>& southOrder,
      const Winner& currBest,
      const unsigned trickNS);

    string str() const;

    string strEntry() const;
};

#endif
