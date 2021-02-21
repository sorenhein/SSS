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

      bool operator >(const SideWinner& sw2) const
      {
        return (rank > sw2.rank);
      }

      bool operator >=(const SideWinner& sw2) const
      {
        return (rank >= sw2.rank);
      }

      bool operator <(const SideWinner& sw2) const
      {
        return (rank < sw2.rank);
      }

      bool operator <=(const SideWinner& sw2) const
      {
        return (rank <= sw2.rank);
      }

      bool operator ==(const SideWinner& sw2) const
      {
        // Only really need rank and number.
        return (rank == sw2.rank &&
            depth == sw2.depth &&
            number == sw2.number);
      }

      bool operator !=(const SideWinner& sw2) const
      {
        // Only really need rank and number.
cout << "rank " << (rank == sw2.rank ? "same" : "diff") << " " <<
  rank << " " << sw2.rank << endl;
cout << "depth " << (depth == sw2.depth ? "same" : "diff") << " " <<
  depth << " " << sw2.depth << endl;
cout << "number " << (number == sw2.number ? "same" : "diff") << " " <<
  number << " " << sw2.number << endl;
        return (rank != sw2.rank ||
            depth != sw2.depth ||
            number != sw2.number);
      }

      bool sameRank(const SideWinner& sw2) const
      {
        return (rank == sw2.rank);
      }

      void operator *=(const SideWinner& sw2)
      {
        // The opponents choose the lowest card.
        if (number > sw2.number)
          * this = sw2;
      }
    };

    SideWinner north;
    SideWinner south;

    WinnerMode mode;


    bool operator !=(const Winner& w2) const;

    string strSingleSided(
      const string& name,
      const SideWinner& winner) const;

    string strDebug() const;


  public:

    Winner();

    ~Winner();

    void reset();

    void set(
      const WinningSide sideIn,
      const unsigned rankIn,
      const unsigned depthIn,
      const unsigned number);

    void operator *= (const Winner& w2);

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
