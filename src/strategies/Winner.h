#ifndef SSS_WINNER_H
#define SSS_WINNER_H

#include <vector>
#include <deque>
#include <list>
#include <cassert>

#include "../const.h"
#include "../utils/Card.h"
#include "../utils/Compare.h"

struct Play;

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
      WIN_BOTH = 2,
      WIN_NOT_SET = 3
    };

    Card north;
    Card south;
    WinnerMode mode;


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

    bool operator == (const Winner& sw2) const;
    bool operator != (const Winner& sw2) const;

    void operator *= (const Winner& sw2);
    void operator |= (const Winner& sw2);

    unsigned char rank() const;

    Compare declarerPrefers(const Winner& sw2) const;

    void flip();

    void update(const Play& play);

    bool rankExceeds(const Winner& sw2) const;

    string str() const;

    string strDebug() const;
};

#endif
