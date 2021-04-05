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

    // remainders maps winners in subsequent tricks to winners
    // in the current reference frame.
    // vector<vector<Card>> remainders;

    // The best card(s) that NS play to this trick, whether or not
    // they win it.  If they win, then the winner is interesting.
    // The indices are the first and second plays to this trick.
    vector<vector<Winner>> best;


    void setVoid();

    void setSingleRank();

    void fixDepths();

    // void setRemainders();

    void countNumbers(vector<unsigned>& numbers) const;

    void setBest(const Declarer& partner);

  public:

    void resize(
      const unsigned cardsIn,
      const CardPosition sideIn);

    void setNames();

    bool playRank(
      const unsigned rank,
      const Declarer& partner,
      const unsigned maxGlobalRank);

    void finish(const Declarer& partner);

    bool greater(
      const Declarer& p2,
      const Opponents& opps) const;
    
    const Card& top() const;

    const Winner& getWinner(
      const unsigned lead,
      const unsigned pard) const;

    // Unlike in Opponents, this can get either ranks or cards.
    const deque<Card const *>& getCards(const bool fullFlag) const;

    bool isSingleRanked() const;
};

#endif
