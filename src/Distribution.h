#ifndef SSS_DISTRIBUTION_H
#define SSS_DISTRIBUTION_H

#include <string>
#include <list>

#include "struct.h"

using namespace std;


class Distribution
{
  private:

    struct RankEntry
    {
      unsigned rank;
      unsigned count;
    };

    struct StackInfo
    {
      vector<RankEntry> ranks; // For a single player (West)
      unsigned len; // Number of cards in ranks
      unsigned seen; // Number of EW cards already seen
      unsigned rankNext;
      unsigned cases; // Combinatorial count
    };

    struct DistInfo
    {
      vector<unsigned> west; // Ranks
      vector<unsigned> east;
      unsigned lenWest;
      unsigned lenEast;
      unsigned used; // Number of E-W cards already seen
      unsigned rankNext;
      unsigned cases; // Combinatorial count

      DistInfo(unsigned cards = 0)
      {
        if (cards)
        {
          west.resize(cards);
          east.resize(cards);
          for (unsigned r = 0; r < cards; r++)
          {
            west[r] = 0;
            east[r] = 0;
          }
        }
        lenWest = 0;
        used = 0;
        rankNext = 0;
        cases = 1;
      }

      string str() const
      {
        string s;
        s = "W " + to_string(lenWest) + ": ";
        for (unsigned i = west.size(); i-- > 0; )
        {
          if (west[i] > 0)
          {
            for (unsigned j = 0; j < west[i]; j++)
              s += to_string(i);
          }
        }
        s += ", E " + to_string(lenEast) + ": ";
        for (unsigned i = east.size(); i-- > 0; )
        {
          if (east[i] > 0)
          {
            for (unsigned j = 0; j < east[i]; j++)
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
      const vector<RankInfo>& oppsRank);

    unsigned set(
      const unsigned cards,
      const unsigned holding2); // Binary, not trinary format

    string rank2str(
      const vector<unsigned>& ranks,
      const vector<string>& names) const;

    string str() const;

};

#endif
