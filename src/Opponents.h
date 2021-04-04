#ifndef SSS_OPPONENTS_H
#define SSS_OPPONENTS_H

#include "Player.h"

#include <deque>

using namespace std;


class Opponents: public Player
{
  private:


  public:

    void setVoid();

    void setNames();

    // Unlike in Declarer, this can only return the ranks.
    const deque<Card const *>& getCards() const;
};

#endif
