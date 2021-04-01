#ifndef SSS_WINNER_H
#define SSS_WINNER_H

#include <vector>
#include <list>
#include <cassert>

#include "Subwinner.h"
#include "Card.h"

using namespace std;


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

    list<Subwinner> subwinners;


    bool operator != (const Winner& w2) const;

    void integrate(const Subwinner& swNew);

    bool rankExceeds(const Winner& w2) const;

    string strSingleSided(
      const string& name,
      const Card& winner) const;

    string strDebug() const;


  public:

    Winner();

    ~Winner();

    void reset();

    void set(
      const WinningSide sideIn,
      const unsigned rankIn,
      const unsigned depthIn,
      const unsigned numberIn,
      const char nameIn);

    void operator *= (const Winner& w2);

    void flip();

    void update(
      vector<Card> const * northOrderPtr,
      vector<Card> const * southOrderPtr,
      Winner const * currBestPtr);

    string str() const;

    string strEntry() const;
};

#endif
