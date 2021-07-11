#ifndef SSS_WINNERS_H
#define SSS_WINNERS_H

#include <vector>
#include <list>
#include <cassert>

#include "Winner.h"
#include "Card.h"

struct Play;

using namespace std;


class Winners
{
  private:

    // If North has AKx (ranks 5 and 1), then rank is 5, depth is 1
    // if it's the A rather than the K, and number is 3 (x, K, A).
    // See also Ranks::setOrderTables().

    enum WinnersMode
    {
      WINS_NORTH_ONLY = 0,
      WINS_SOUTH_ONLY = 1,
      WINS_NS_DECIDE = 2,
      WINS_EW_DECIDE = 3,
      WINS_EMPTY = 4, // No rank winner, but something is known
      WINS_NOT_SET = 5 // Neutral state
    };

    list<Winner> winners;


    bool operator != (const Winners& w2) const;

    void integrate(const Winner& swNew);

    bool rankExceeds(const Winners& w2) const;

    string strSingleSided(
      const string& name,
      const Card& winner) const;


  public:

    Winners();

    ~Winners();

    void reset();

    void set(
      const WinningSide sideIn,
      const Card& card);

    void set(
      const Card& north,
      const Card& south);

    void setEmpty();

    bool empty() const;

    void operator *= (const Winners& w2);

    bool operator == (const Winners& w2) const;

    WinnerCompare compareForDeclarer(const Winners& w2) const;

    void flip();

    void update(const Play& play);

    string str() const;

    string strEntry() const;

    string strDebug() const;
};

#endif
