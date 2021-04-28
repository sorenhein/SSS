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
      const unsigned rank,
      const Declarer& partner,
      const unsigned maxGlobalRank);

    void finish();

    bool greater(
      const Declarer& p2,
      const Opponents& opps) const;
    
    const Card& top() const;

    // Unlike in Opponents, this can get either ranks or cards.
    const deque<Card const *>& getCards(const bool fullFlag) const;

    bool isSingleRanked() const;
};

#endif
