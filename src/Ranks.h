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

    struct PositionInfo
    {
      vector<RankInfo> ranks;
      unsigned max;
      unsigned min;
      bool singleRank;
      unsigned len;

      void update(
        const unsigned rank,
        const char name,
        bool& flag)
      {
        ranks[rank].add(name);
        max = rank;
        if (flag)
        {
          min = rank;
          flag = false;
        }
        len++;
      }

      void setVoid(const bool flag)
      {
        if (flag)
        {
          ranks[0].add('-');
          max = 0;
        }
      }

      void setSingleRank()
      {
        singleRank = (len >= 1 && min == max);
      }
    };

    PositionInfo north;
    PositionInfo south;
    PositionInfo opps;

    unsigned cards;
    unsigned maxRank;


    void setRanks(const unsigned holding);

    bool dominates(
      const vector<RankInfo>& vec1,
      const vector<RankInfo>& vec2) const;

    unsigned canonical(
      const vector<RankInfo>& vec1,
      const vector<RankInfo>& vec2,
      vector<char>& canonical2comb) const;

    void canonicalUpdate(
      const vector<RankInfo>& vec1,
      const vector<RankInfo>& vec2,
      const vector<RankInfo>& oppsIn,
      const unsigned cardsNew,
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
      const vector<RankInfo>& vec1,
      const vector<RankInfo>& vec2,
      PlayEntry& play) const;

    void setPlaysSide(
      const PositionInfo& leader,
      const PositionInfo& partner,
      const SidePosition side,
      list<PlayEntry>& plays) const;

    string strRankInfo(
      const RankInfo& rankInfo,
      const string& pos) const;

  public:

    Ranks();

    ~Ranks();

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
