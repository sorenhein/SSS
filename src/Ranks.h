#ifndef SSS_RANKS_H
#define SSS_RANKS_H

#include <vector>
#include <map>

using namespace std;

struct CombEntry;


class Ranks
{
  private:

    struct RankInfo
    {
      unsigned count;
      vector<char> cards;

      RankInfo()
      {
        RankInfo::clear();
      }

      void clear()
      {
        count = 0;
      }

      void add(const char card)
      {
        cards[count] = card;
        count++;
      }
    };

    vector<RankInfo> north;
    vector<RankInfo> south;
    vector<RankInfo> opps;

    unsigned maxRank;


    void setRanks(
      const unsigned holding,
      const unsigned cards);

    bool dominates(
      const vector<RankInfo>& vec1,
      const vector<RankInfo>& vec2) const;

    unsigned canonical(
      const vector<RankInfo>& vec1,
      const vector<RankInfo>& vec2,
      const unsigned cards,
      vector<char>& canonical2comb) const;

    string strRankInfo(
      const RankInfo& rankInfo,
      const string& pos) const;

  public:

    Ranks();

    ~Ranks();

    void resize(const unsigned cards);

    void clear();

    void set(
      const unsigned holding,
      const unsigned cards,
      CombEntry& combEntry);

    string str() const;
};

#endif
