/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_PLAY_H
#define SSS_PLAY_H

#include <deque>
#include <string>

#include "../strategies/result/Winners.h"


using namespace std;

class Combinations;
class Combination;
class Card;


struct Play
{
  Side side;
  Card const * leadPtr;
  Card const * lhoPtr;
  Card const * pardPtr;
  Card const * rhoPtr;

  bool leadCollapse; // true if the lead rank goes away after the trick
  bool pardCollapse;

  unsigned char trickNS; // 1 iff North-South win the trick

  // The leader cards are used to map subsequent winners up to current
  // ones.  Once the leader has played a card, there is one card less
  // remaining.  When reading winners from later combinations, the
  // winners will be numbered according to their remaining count.
  // As we know the number of the current lead, we can perform this map.
  // This is used for tracking rank winners in overall combinations.

  deque<Card const *> const * northCardsPtr;
  deque<Card const *> const * southCardsPtr;

  // There is a "winner" associated with the current trick.  If 
  // North-South didn't win the trick, it is the nullptr.  If one of
  // them won by a higher rank than the other played card, then that
  // is the winner.  If both players played the same rank, then we
  // keep them both, as this is a choice for North-South.
  Winners currBest;

  // Information about the place the play came from.
  unsigned cardsLeft;

  unsigned holding3;
  bool rotateFlag;

  // The combination following the current trick.  This does not get
  // set directly in Ranks.
  Combination const * combPtr;

  
  unsigned lead(const bool fullFlag = false) const;
  unsigned lho(const bool fullFlag = false) const;
  unsigned pard(const bool fullFlag = false) const;
  unsigned rho(const bool fullFlag = false) const;

  void setCombPtr(const Combinations& combinations);

  void setVoidFlags(
    bool& westFlag,
    bool& eastFlag) const;

  Card const * northTranslate(const unsigned number) const;
  Card const * southTranslate(const unsigned number) const;

  bool samePartial(
    const Play& play2,
    const Level level) const;

  string strPartialTrick(const Level level) const;

  string strTrick(const unsigned number) const;

  string strHeader() const;

  string strLine() const;
};

#endif
