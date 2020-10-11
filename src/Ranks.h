#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <vector>

#include "const.h"

using namespace std;

struct CombEntry;
struct PlayEntry;
class Plays;


class Ranks
{
  private:

    struct ReducedRankInfo
    {
      unsigned rank;
      unsigned count;

      ReducedRankInfo()
      {
        count = 0;
      }

      void add(const unsigned rankIn)
      {
        rank = rankIn;
        count++;
      }
    };

    struct PositionInfo
    {
      vector<ReducedRankInfo> ranks;
      unsigned maxPos;
      unsigned minPos;

      vector<unsigned> fullCount;
      unsigned maxRank;
      unsigned minRank;

      bool singleRank;
      unsigned len;

      void resize(const unsigned cardsIn)
      {
        // Worst case, leaving room for voids at rank 0.
        ranks.resize(cardsIn+1);
        maxPos = cardsIn;

        fullCount.resize(cardsIn+1);
        maxRank = cardsIn;
      }

      void clear()
      {
        ranks.clear();
        fullCount.clear();
      }

      void zero()
      {
        for (unsigned pos = 0; pos <= maxPos; pos++)
          ranks[pos].count = 0;

        for (unsigned rank = 0; rank <= maxRank; rank++)
          fullCount[rank] = 0;
        
        len = 0;
      }

      void update(
        const unsigned position,
        const unsigned rank,
        bool& firstFlag)
      {
        ranks[position].add(rank);
        maxPos = position;

        fullCount[rank]++;
        maxRank = rank;

        if (firstFlag)
        {
          minPos = position;
          minRank = rank;
          firstFlag = false;
        }

        len++;
      }

      void setVoid(const bool forceFlag)
      {
        if (forceFlag || len == 0)
        {
          ranks[0].add(0);
          minPos = 0;
          maxPos = 0;

          fullCount[0] = 1;
          minRank = 0;
          maxRank = 0;
        }
      }

      void setSingleRank()
      {
        singleRank = (len >= 1 && minRank == maxRank);
      }

      bool operator >= (const PositionInfo& p2) const
      {
        // The rank vectors may not be of the same effective size.
        unsigned pos1 = maxPos + 1;  // One beyond end
        unsigned pos2 = p2.maxPos + 1;

        while (true)
        {
          while (true)
          {
            // If we run out of p2, this wins even if it also runs out.
            if (pos2 == 0)
              return true;
      
            if (p2.ranks[--pos2].count)
              break;
          }

          while (true)
          {
            // Otherwise p2 wins.
            if (pos1 == 0)
              return false;

            if (ranks[--pos1].count)
              break;
          }

          if (ranks[pos1].rank > p2.ranks[pos2].rank)
            return true;
          if (ranks[pos1].rank < p2.ranks[pos2].rank)
            return false;
          if (ranks[pos1].count > p2.ranks[pos2].count)
            return true;
          if (ranks[pos1].count < p2.ranks[pos2].count)
            return false;
        }
      }
    };


    PositionInfo north;
    PositionInfo south;
    PositionInfo opps;

    unsigned holding;
    unsigned cards;
    unsigned maxRank;


    void setConstants();

    void zero();

    void setRanks();

    bool dominates(
      const PositionInfo& first,
      const PositionInfo& second) const;

    unsigned canonicalTrinary(
      const vector<unsigned>& fullCount1,
      const vector<unsigned>& fullCount2) const;

    void canonicalBoth(
      const vector<unsigned>& fullCount1,
      const vector<unsigned>& fullCount2,
      unsigned& holding3,
      unsigned& holding2) const;

    bool trivial(unsigned& terminalValue) const;

    bool leadOK(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const unsigned lead) const;

    bool pardOK(
      const PositionInfo& partner,
      const unsigned toBeat,
      const unsigned pard) const;

    void updateHoldings(
      const PositionInfo& leader,
      const PositionInfo& partner,
      unsigned& holding3,
      bool& rotateFlag) const;

    void setPlaysSideWithVoid(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      Plays& plays);

    void setPlaysSideWithoutVoid(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      Plays& plays);

    void setPlaysSide(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      Plays& plays);

    void strSetFullNames(
      vector<string>& namesNorth,
      vector<string>& namesSouth,
      vector<string>& namesOpps) const;

    string strPosition(
      const string& cards,
      const string& player) const;

  public:

    Ranks();

    ~Ranks();

    void resize(const unsigned cardsIn);

    void set(
      const unsigned holdingIn,
      CombEntry& combEntry);

    CombinationType setPlays(
      Plays& plays,
      unsigned& terminalValue);

    string str() const;
};

#endif
