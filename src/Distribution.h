#ifndef SSS_DISTRIBUTIONS_H
#define SSS_DISTRIBUTIONS_H

#include <string>

#include "struct.h"

using namespace std;


class Distribution
{
  private:

    struct DistInfo
    {
      vector<unsigned> west; // Ranks
      vector<unsigned> east;
      unsigned lenWest;
      unsigned lenEast;
      unsigned used; // Number of E-W cards already seen
      unsigned rankNext;
      unsigned cases; // Combinatorial count

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

    void set(
      const unsigned cards,
      const vector<RankInfo>& oppsRank);

    string rank2str(
      const vector<unsigned>& ranks,
      const vector<string>& names) const;

    string str() const;

};

#endif
