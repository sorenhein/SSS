#ifndef SSS_DISTRIBUTION_H
#define SSS_DISTRIBUTION_H

#include <string>
#include <list>

#include "struct.h"

using namespace std;


class Distribution
{
  private:

    vector<unsigned> full2reduced;

    struct RankEntry
    {
      unsigned rank;
      unsigned count;

      RankEntry()
      {
        count = 0;
      }
    };

    struct SideInfo
    {
      vector<RankEntry> ranks;
      unsigned len; // Sum of rank counts
    };

    struct StackInfo
    {
      SideInfo west;
      unsigned seen; // Number of EW cards already seen
      unsigned rankNext;
      unsigned cases; // Combinatorial count

      StackInfo(unsigned cards = 0)
      {
        if (cards)
        {
          west.ranks.resize(cards);
          for (unsigned r = 0; r < cards; r++)
          {
            west.ranks[r].rank = r;
            west.ranks[r].count = 0;
          }
        }
        west.len = 0;
        cases = 1;
      }

    };

    struct DistInfo
    {
      SideInfo west;
      SideInfo east;
      unsigned cases; // Combinatorial count

      DistInfo(unsigned cards = 0)
      {
        if (cards)
        {
          west.ranks.resize(cards);
          east.ranks.resize(cards);
          for (unsigned r = 0; r < cards; r++)
          {
            west.ranks[r].rank = r;
            west.ranks[r].count = 0;
            east.ranks[r].rank = r;
            east.ranks[r].count = 0;
          }
        }
        west.len = 0;
        east.len = 0;
        cases = 1;
      }

      string str() const
      {
        string s;
        s = "W " + to_string(west.len) + ": ";
        for (unsigned i = west.ranks.size(); i-- > 0; )
        {
          if (west.ranks[i].count > 0)
          {
            for (unsigned j = 0; j < west.ranks[i].count; j++)
              s += to_string(i);
          }
        }
        s += ", E " + to_string(east.len) + ": ";
        for (unsigned i = east.ranks.size(); i-- > 0; )
        {
          if (east.ranks[i].count > 0)
          {
            for (unsigned j = 0; j < east.ranks[i].count; j++)
              s += to_string(i);
          }
        }
        s += " (" + to_string(cases) + ")\n";
        return s;
      }
    };

    vector<vector<unsigned>> binomial;

    vector<DistInfo> distributions;


    void setBinomial();

    void setRanks(
      const unsigned cards,
      const unsigned holding2,
      vector<unsigned>& oppsFullRank,
      vector<RankEntry>& oppsReducedRank,
      unsigned& len);

    void mirror(
      const unsigned len,
      const unsigned lenMid,
      unsigned& distIndex);

    string strStack(const list<DistInfo>& stack) const;

    string strDist(const unsigned DistIndex) const;


  public:

    Distribution();

    ~Distribution();

    void reset();

    unsigned set(
      const unsigned cards,
      const unsigned holding2); // Binary, not trinary format

    string rank2str(
      const vector<RankEntry>& ranks,
      const vector<string>& names) const;

    string str() const;

};

#endif
