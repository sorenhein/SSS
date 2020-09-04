#ifndef SSS_RANKS2_H
#define SSS_RANKS2_H

#include <vector>
#include <list>

#include "struct.h"

using namespace std;

struct CombEntry;


class Ranks2
{
  private:

    struct RankInfo2
    {
      unsigned rank;
      unsigned count;
      vector<char> cards;

      RankInfo2()
      {
        RankInfo2::clear();
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

    struct RankInfo3
    {
      unsigned count;
      vector<char> cards;

      RankInfo3()
      {
        RankInfo3::clear();
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
      vector<RankInfo2> ranks;
      vector<RankInfo3> fullCount;
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
      const vector<RankInfo2>& vec1,
      const unsigned max1,
      const vector<RankInfo2>& vec2,
      const unsigned max2) const;

    unsigned canonicalTrinary(
      const vector<RankInfo3>& fullCount1,
      const vector<RankInfo3>& fullCount2) const;

    void canonicalBoth(
      const vector<RankInfo3>& fullCount1,
      const vector<RankInfo3>& fullCount2,
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
      const vector<RankInfo2>& vec1,
      const vector<RankInfo2>& vec2,
      const unsigned max1,
      const unsigned max2,
      const vector<RankInfo3>& fullCount1,
      const vector<RankInfo3>& fullCount2,
      PlayEntry& play);

    void setPlaysSideWithVoid(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      vector<RankInfo3>& fullCount1,
      vector<RankInfo3>& fullCount2,
      vector<PlayEntry>& plays,
      unsigned& playNo);

    void setPlaysSideWithoutVoid(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      vector<RankInfo3>& fullCount1,
      vector<RankInfo3>& fullCount2,
      vector<PlayEntry>& plays,
      unsigned& playNo);

    void setPlaysSide(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      vector<RankInfo3>& fullCount1,
      vector<RankInfo3>& fullCount2,
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

    Ranks2();

    ~Ranks2();

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
