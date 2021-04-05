#ifndef SSS_PLAY_H
#define SSS_PLAY_H

#include <deque>

#include "strategies/Card.h"
#include "const.h"


using namespace std;

class Combination;


struct Play
{
  CardPosition side;
  Card const * leadPtr;
  Card const * lhoPtr;
  Card const * pardPtr;
  Card const * rhoPtr;

  bool leadCollapse; // true if the lead rank goes away after the trick
  bool pardCollapse;

  unsigned trickNS; // 1 iff North-South win the trick

  // The leader cards are used to map subsequent winners up to current
  // ones.  Once the leader has played a card, there is one card less
  // remaining.  When reading winners from later combinations, the
  // winners will be numbered according to their remaining count.
  // As we know the number of the current lead, we can perform this map.
  // This is used for tracking rank winners in overall combinations.

  deque<Card const *> const * leaderCardsPtr;
  deque<Card const *> const * partnerCardsPtr;

  // There is a "winner" associated with the current trick.  If 
  // North-South didn't win the trick, it is the nullptr.  If one of
  // them won by a higher rank than the other played card, then that
  // is the winner.  If both players played the same rank, then we
  // keep them both, as this is a choice for North-South.
  Winner const * currBestPtr;

  // Information about the place the play came from.
  unsigned cardCount;
  unsigned holding3;
  bool rotateFlag;

  // The combination following the current trick.  This does not get
  // set directly in Ranks.
  Combination const * combPtr; // UNSET
};

#endif
