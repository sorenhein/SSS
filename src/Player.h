#ifndef SSS_PLAYER_H
#define SSS_PLAYER_H

#include <vector>

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

    // remainders maps winners in subsequent tricks to winners
    // in the current reference frame.
    vector<vector<Sidewinner>> remainders;

    // The best card(s) that NS play to this trick, whether or not
    // they win it.  If they win, then the winner is interesting.
    // The indices are the first and second plays to this trick.
    vector<vector<Winner>> best;

    // The numbers of each relevant NS card.
    vector<unsigned> numRank;


  public:

    Player();

    ~Player();

    void resize(const unsigned cardsIn);

    void clear(); // Empties the vectors
    void zero(); // Keeps the vectors, but sets them to zero

    void update(
      const unsigned position,
      const unsigned rank,
      bool& firstFlag);

    void setVoid(const bool forceFlag);
    void setSingleRank();

    bool greater(
      const Player& p2,
      const Player& opps) const;
};

#endif
