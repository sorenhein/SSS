/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_DECLARER_H
#define SSS_DECLARER_H

#include <vector>
#include <deque>

#include "Player.h"

class Opponents;

using namespace std;


class Declarer: public Player
{
  private:

    bool singleRank;


    void setVoid();

    void setSingleRank();

    void fixDepths();


  public:

    void setNames();

    bool playRank(
      const unsigned char rank,
      const Declarer& partner,
      const unsigned char maxGlobalRank);

    void finish();

    bool greater(
      const Declarer& p2,
      const Opponents& opps) const;

    bool tops(const Declarer& p2) const;
    
    const Card& top() const;

    // Unlike in Opponents, this can get either ranks or cards.
    const deque<Card const *>& getCards(const bool fullFlag) const;

    bool isSingleRanked() const;
};

#endif
