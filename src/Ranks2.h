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

    struct PositionInfo
    {
      vector<RankInfo2> ranks;
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

    vector<unsigned> full2reducedNorth;
    vector<unsigned> full2reducedSouth;
    vector<unsigned> full2reducedOpps;

    PositionInfo north;
    PositionInfo south;
    PositionInfo opps;

    unsigned cards;
    unsigned maxRank;

    void setRanks(const unsigned holding);

    bool dominates(
      const vector<RankInfo2>& vec1,
      const unsigned max1,
      const vector<RankInfo2>& vec2,
      const unsigned max2) const;

    unsigned canonical(
      const vector<RankInfo2>& vec1,
      const vector<RankInfo2>& vec2,
      const vector<unsigned>& full2reduced1,
      const vector<unsigned>& full2reduced2,
      vector<char>& canonical2comb) const;

    void canonicalUpdate(
      const vector<RankInfo2>& vec1,
      const vector<RankInfo2>& vec2,
      const vector<RankInfo2>& oppsIn,
      const unsigned cardsNew,
      unsigned& holding3,
      unsigned& holding2) const;

    bool trivial(unsigned& terminalValue) const;

    bool leadOK(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const unsigned leadPos,
      const unsigned lead) const;

    bool oppOK(
      const unsigned card,
      const unsigned count,
      const bool alreadyPlayed) const;

    bool pardOK(
      const PositionInfo& partner,
      const unsigned toBeat,
      const unsigned pardPos,
      const unsigned pard) const;

    void updateHoldings(
      const vector<RankInfo2>& vec1,
      const vector<RankInfo2>& vec2,
      const unsigned max1,
      const unsigned max2,
      const unsigned leadPos,
      const unsigned lhoPos,
      const unsigned pardPos,
      const unsigned rhoPos,
      PlayEntry& play) const;

    void setPlaysSide(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      list<PlayEntry>& plays) const;

    string strRankInfo(
      const vector<RankInfo2>& rankInfo,
      const unsigned rank,
      const string& player) const;

  public:

    Ranks2();

    ~Ranks2();

    void resize(const unsigned cardsIn);

    void clear();

    void set(
      const unsigned holding,
      CombEntry& combEntry);

    CombinationType setPlays(
      list<PlayEntry>& plays,
      unsigned& terminalValue) const;

    string str() const;
};

#endif
