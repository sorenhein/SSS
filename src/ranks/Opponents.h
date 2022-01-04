/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_OPPONENTS_H
#define SSS_OPPONENTS_H

#include "Player.h"

#include <deque>

using namespace std;


class Opponents: public Player
{
  private:

    // In order to have something to refer to when a void is needed.
    Card voidCard;


  public:

    void setVoid();

    void setNames();

    void playRank(const unsigned char rank);

    // Unlike in Declarer, this can only return the ranks.
    const deque<Card const *>& getCards() const;

    Card const * voidPtr() const;
};

#endif
