#ifndef SSS_SUBWINNER_H
#define SSS_SUBWINNER_H

#include <vector>
#include <list>
#include <cassert>

#include "Card.h"
#include "../const.h"

using namespace std;


class Subwinner
{
  private:

    // If North has AKx (ranks 5 and 1), then rank is 5, depth is 1
    // if it's the A rather than the K, and number is 3 (x, K, A).
    // See also Ranks::setOrderTables().

    enum SubwinnerMode
    {
      SUBWIN_NORTH_ONLY = 0,
      SUBWIN_SOUTH_ONLY = 1,
      SUBWIN_BOTH = 2,
      SUBWIN_NOT_SET = 3
    };

    Card north;
    Card south;
    SubwinnerMode mode;


  public:

    Subwinner();

    ~Subwinner();

    void reset();

    void set(
      const WinningSide sideIn,
      const unsigned rankIn,
      const unsigned depthIn,
      const unsigned number,
      const char nameIn);

    bool operator == (const Subwinner& sw2) const;
    bool operator != (const Subwinner& sw2) const;

    void operator *= (const Subwinner& sw2);

    WinnerCompare declarerPrefers(const Subwinner& sw2) const;

    void flip();

    void update(
      vector<Card> const * northOrderPtr,
      vector<Card> const * southOrderPtr);

    bool rankExceeds(const Subwinner& sw2) const;

    string str() const;

    string strDebug() const;
};

#endif
