#ifndef SSS_PLAYER_H
#define SSS_PLAYER_H

#include <vector>
#include <deque>

#include "strategies/Card.h"
#include "strategies/Winner.h"

using namespace std;


class Player
{
  private:

    // ranks is a compact list of the ranks that occur in fullCount.
    // So if fullCount has entries at 3, 7 and 9, then ranks will
    // contain 3, 7 and 9.
    vector<unsigned> ranks;

    unsigned maxPos;
    unsigned minPos;
    unsigned cardCount;

    vector<unsigned> fullCount;
    unsigned maxRank;
    unsigned minRank;
    unsigned minAbsCardNumber;
    unsigned numberNextCard;
    unsigned depthNext;
    unsigned posNext;
    bool firstUpdateFlag;

    // Running index for the iteration over ranks.
    unsigned rankIndex;

    vector<string> names;
    vector<Card> cards;

    bool firstOfRankFlag;
    unsigned numberNextCardNew;
    vector<Card> cardsNew;
    deque<Card const *> cardsPtrNew;
    deque<Card const *> ranksPtrNew;


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

    string playerName() const;


  public:

    Player();

    ~Player();

    void clear(); // Empties the vectors

    void resize(
      const unsigned cardsIn,
      const CardPosition sideIn);

    void resizeBest(const Player& partner);

    void zero(); // Keeps the vectors, but sets them to zero

    void updateStep(const unsigned rankNew);

    void update(
      const unsigned rank,
      const unsigned absCardNumber);

    void setVoid(const bool forceFlag);

    void setSingleRank();

    void setNames(const bool declarerFlag);

    void setRemainders();

    void setBest(const Player& partner);

    void playFull(const unsigned rankFullIn);
    void restoreFull(const unsigned rankFullIn);

    bool greater(
      const Player& p2,
      const Player& opps) const;
    
    const Card& top() const;

    bool hasRank(const unsigned rankIn) const;

    unsigned length() const;

    unsigned minFullRank() const;
    unsigned maxFullRank() const;

    const vector<Card>& remainder(const unsigned rank) const;

    const Winner& getWinner(
      const unsigned lead,
      const unsigned pard) const;

    const deque<Card const *>& getCards() const;

    bool isVoid() const;

    bool isSingleRanked() const;

    unsigned count(const unsigned rank) const;

    string strRankHeader() const;
    string strRank(const unsigned rank) const;

    wstring wstr() const;
};

#endif
