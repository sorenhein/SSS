#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <vector>

#include "struct.h"

using namespace std;

struct CombEntry;


class Ranks
{
  private:

    struct ReducedRankInfo
    {
      unsigned rank;
      unsigned count;

      ReducedRankInfo()
      {
        ReducedRankInfo::clear();
      }

      void clear()
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
      vector<unsigned> fullCount;
      unsigned maxRank;
      unsigned minRank;
      unsigned maxPos;
      unsigned minPos;
      bool singleRank;
      unsigned len;

      void update(
        const unsigned position,
        const unsigned rank,
        bool& flag)
      {
        ranks[position].add(rank);
        maxRank = rank;
        maxPos = position;
        if (flag)
        {
          minRank = rank;
          minPos = position;
          flag = false;
        }
        len++;
      }

      void setVoid(const bool forceFlag)
      {
        if (forceFlag || len == 0)
        {
          ranks[0].add(0);
          minRank = 0;
          maxRank = 0;
          minPos = 0;
          maxPos = 0;
        }
      }

      void setSingleRank()
      {
        singleRank = (len >= 1 && minRank == maxRank);
      }
    };


    PositionInfo north;
    PositionInfo south;
    PositionInfo opps;

    unsigned holding;
    unsigned cards;
    unsigned maxRank;


    void setConstants();

    void setRanks();

    bool dominates(
      const vector<ReducedRankInfo>& vec1,
      const unsigned max1,
      const vector<ReducedRankInfo>& vec2,
      const unsigned max2) const;

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
      PlayEntry& play) const;

    void setPlaysSideWithVoid(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      vector<PlayEntry>& plays,
      unsigned& playNo);

    void setPlaysSideWithoutVoid(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      vector<PlayEntry>& plays,
      unsigned& playNo);

    void setPlaysSide(
      PositionInfo& leader,
      PositionInfo& partner,
      const SidePosition side,
      vector<PlayEntry>& plays,
      unsigned& playNo);

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

    void clear();

    void set(
      const unsigned holdingIn,
      CombEntry& combEntry);

    CombinationType setPlays(
      vector<PlayEntry>& plays,
      unsigned& playNo,
      unsigned& terminalValue);

    string str() const;
};

#endif
