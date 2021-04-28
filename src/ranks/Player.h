#ifndef SSS_PLAYER_H
#define SSS_PLAYER_H

#include <vector>
#include <deque>

#include "../strategies/Card.h"

using namespace std;


class Player
{
  protected:

    /* Example with 9 cards and 6 ranks:
     *      
     *      AK8
     *           (missing Q976)
     *      JT
     *                      card
     *             ----------------------
     * player card rank depth number name  card#
     *   opp  6    1    1      opp:0  '6'      0
     *   opp  7    1    0      opp:1  '7'      1
     * North  8    2    0    North:0  '8'      2
     *   opp  9    3    0      opp:2  '9'      3
     * South  T    4    1    South:0  'T'      4
     * South  J    4    0    South:1  'J'      5
     *   opp  Q    5    0      opp:3  'Q'      6
     * North  K    6    1    North:1  'K'      7
     * North  A    6    0    North:2  'A'      8
     *
     * cards (these are actual cards, not references/pointers)
     *        North South   Opps
     *     0     #2    #4     #0
     *     1     #7    #5     #1
     *     2     #8           #3
     *     3                  #6
     *
     * cardsPtr (these are pointers to entries in the respective cards)
     *        North South   Opps
     *     0   c[0]   c[0]  c[0]
     *     1   c[1]   c[1]  c[1]
     *     2   c[2]         c[2]
     *     3                c[3]
     *
     * ranksPtr (these are pointers to entries in the respective cards)
     *        North South   Opps
     *     0   c[0]  c[1]   c[1], so 8 / J / 7
     *     1   c[2]         c[2], so A /   / 9
     *     2                c[3], so   /   / Q
     *
     * Looping over all entries in cardsPtr yields all the cards,
     * including those of the same rank.  Looping over all entries
     * in ranksPtr only yields the top cards of each rank.
     *
     * North and South have a void entry in cards[0] if and only if
     * that player is actually void.  In this case cards has 1 entry.
     * Opps never has a void entry.  Its cards will be empty if opps
     * have no cards.
     *
     * rankInfo (pointers are as in ranksPtr)
     *             North               South               Opps
     *       -----------------   -----------------   -----------------
     *  rank count names   ptr   count names   ptr   count names   ptr
     *     0     0     -     -       0     -     -       1    ""     -
     *     1     0     -     -       0     -     -       2    76  c[1]
     *     2     1     8  c[0]       0     -     -       0     -     -
     *     3     0     -     -       0     -     -       1     9  c[2]
     *     4     0     -     -       2    JT  c[1]       0     -     -
     *     5     0     -     -       0     -     -       1     Q  c[3]
     *     6     2    AK  c[2]       0     -     -       0     -     -
     *
     */

    Side side;
    unsigned len;

    deque<Card> cards;
    deque<Card const *> cardsPtr;
    deque<Card const *> ranksPtr;

    struct RankInfo
    {
      unsigned count; // Number of player's cards with that rank
      string names; // Names of all such cards

      RankInfo()
      {
        RankInfo::clear();
      };

      void clear()
      {
        count = 0;
        names = "";
      };
    };

    vector<RankInfo> rankInfo;

    // This are long-term variables.
    bool singleRank;
    unsigned minRank;
    unsigned maxRank;
    unsigned minAbsCardNumber;

    // These are only used during updates, but are global for ease.
    unsigned numberNextCard;
    unsigned depthNext;
    unsigned posNext;
    bool firstUpdateFlag;
    bool firstOfRankFlag;


    void setVoid(const bool forceFlag);

    void setSingleRank();

    void setNames(const bool declarerFlag);

    string playerName() const;


  public:

    Player();

    ~Player();

    void resize(
      const unsigned cardsIn,
      const Side sideIn);

    void zero(); // Keeps rankInfo, but sets stored vectors to zero

    void updateStep(const unsigned rankNew);

    void update(
      const unsigned rank,
      const unsigned absCardNumber);

    void restoreRank(const unsigned rank);

    bool greater(
      const Player& p2,
      const Player& opps) const;
    
    const Card& top() const;

    bool hasRank(const unsigned rankIn) const;

    unsigned length() const;

    unsigned minFullRank() const;
    unsigned maxFullRank() const;

    bool isVoid() const;

    bool isSingleRanked() const;

    unsigned count(const unsigned rank) const;

    string strRankHeader() const;
    string strRank(const unsigned rank) const;

    wstring wstr() const;
};

#endif
