#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <vector>
#include <list>

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
      vector<char> cards;

      ReducedRankInfo()
      {
        ReducedRankInfo::clear();
        cards.clear();
      }

      void clear()
      {
        count = 0;
      }

      void add(
        const unsigned rankIn,
        const char card)
      {
        cards[count] = card;
        rank = rankIn;
        count++;
      }
    };

    struct FullRankInfo
    {
      unsigned count;
      vector<char> cards;

      FullRankInfo()
      {
        FullRankInfo::clear();
      }

      void clear()
      {
        count = 0;
      }

      void add(const char name)
      {
        cards[count] = name;
        count++;
      }
    };

    struct PositionInfo
    {
      vector<ReducedRankInfo> ranks;
      vector<FullRankInfo> fullCount;
      unsigned maxRank;
      unsigned minRank;
      unsigned maxPos;
      unsigned minPos;
      bool singleRank;
      unsigned len;

      void update(
        const unsigned position,
        const unsigned rank,
        const char name,
        bool& flag)
      {
        ranks[position].add(rank, name);
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
          ranks[0].add(0, '-');
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
      const vector<FullRankInfo>& fullCount1,
      const vector<FullRankInfo>& fullCount2) const;

    void canonicalBoth(
      const vector<FullRankInfo>& fullCount1,
      const vector<FullRankInfo>& fullCount2,
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
      const vector<ReducedRankInfo>& vec1,
      const vector<ReducedRankInfo>& vec2,
      const unsigned max1,
      const unsigned max2,
      const vector<FullRankInfo>& fullCount1,
      const vector<FullRankInfo>& fullCount2,
      PlayEntry& play);

    void setPlaysSideWithVoid(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      vector<FullRankInfo>& fullCount1,
      vector<FullRankInfo>& fullCount2,
      vector<PlayEntry>& plays,
      unsigned& playNo);

    void setPlaysSideWithoutVoid(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      vector<FullRankInfo>& fullCount1,
      vector<FullRankInfo>& fullCount2,
      vector<PlayEntry>& plays,
      unsigned& playNo);

    void setPlaysSide(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      vector<FullRankInfo>& fullCount1,
      vector<FullRankInfo>& fullCount2,
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
