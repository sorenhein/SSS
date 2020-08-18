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
      int count;
      vector<string> cards;

      RankInfo()
      {
        RankInfo::clear();
      }

      void clear()
      {
        count = 0;
        cards.clear();
      }

    };

    vector<RankInfo> north;
    vector<RankInfo> south;
    vector<RankInfo> opps;

    int maxRank;


    void setRanks(
      const int holding,
      const int cards);

    bool dominates(
      const vector<RankInfo>& vec1,
      const vector<RankInfo>& vec2) const;

    int canonical(
      const vector<RankInfo>& vec1,
      const vector<RankInfo>& vec2,
      const int cards,
      map<string, string>& canonical2comb) const;

    string strRankInfo(
      const RankInfo& rankInfo,
      const string& pos) const;

  public:

    Ranks();

    ~Ranks();

    void resize(const int cards);

    void clear();

    void set(
      const int holding,
      const int cards,
      CombEntry& combEntry);

    string str() const;
};

#endif
