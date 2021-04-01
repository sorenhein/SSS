#ifndef SSS_PLAYER_H
#define SSS_PLAYER_H

#include <vector>

#include "strategies/Card.h"
#include "strategies/Winner.h"

using namespace std;


class Player
{
  private:

    struct Rank
    {
      unsigned rank;
      unsigned count;

      Rank()
      {
        count = 0;
      }

      void add(const unsigned rankIn)
      {
        rank = rankIn;
        count++;
      }
    };

    vector<Rank> ranks;
    unsigned maxPos;
    unsigned minPos;

    vector<unsigned> fullCount;
    unsigned maxRank;
    unsigned minRank;

    bool singleRank;
    unsigned len;

    // TODO CardPosition should have a more general name such as
    // Side.
    CardPosition side;

    // remainders maps winners in subsequent tricks to winners
    // in the current reference frame.
    vector<vector<Card>> remainders;

    // The best card(s) that NS play to this trick, whether or not
    // they win it.  If they win, then the winner is interesting.
    // The indices are the first and second plays to this trick.
    vector<vector<Winner>> best;

    // The numbers of each relevant NS card.
    vector<unsigned> numRank;

    void countNumbers(vector<unsigned>& numbers) const;

    // TMP
    unsigned iMinOpps;


  public:

    Player();

    ~Player();

    void resize(
      const unsigned cardsIn,
      const CardPosition sideIn);

    void resizeBest(const Player& partner);

    void clear(); // Empties the vectors

    void zero(); // Keeps the vectors, but sets them to zero

    void update(
      const unsigned position,
      const unsigned rank,
      bool& firstFlag);

    void setVoid(const bool forceFlag);

    void setSingleRank();

    // TODO Both cards and names should transition to Player
    void setNames(
      const vector<Card>& cards,
      const bool declarerFlag,
      vector<string>& names);

    void setRemainders(const vector<string>& names);

    void setBest(
      const Player& partner,
      const vector<string>& namesOwn,
      const vector<string>& namesPartner);

    void playFull(const unsigned rankFullIn);
    void restoreFull(const unsigned rankFullIn);

    bool greater(
      const Player& p2,
      const Player& opps) const;
    
    bool hasReducedRank(const unsigned rankIn) const;
    bool hasFullRank(const unsigned rankFullIn) const;

    unsigned length() const;

    unsigned minFullRank() const;
    unsigned maxFullRank() const;

    // TODO Temporary, should be a loop in Player.
    unsigned minNumber() const;
    unsigned maxNumber() const;
    unsigned rankOfNumber(const unsigned no) const;
    void setTMP(const unsigned iMin);

    const vector<Card>& remainder(const unsigned rank) const;

    const Winner& getWinner(
      const unsigned lead,
      const unsigned pard) const;

    bool isVoid() const;

    bool isSingleRanked() const;

    unsigned count(const unsigned rank) const;
};

#endif
