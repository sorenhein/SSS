#ifndef SSS_DISTRIBUTIONS_H
#define SSS_DISTRIBUTIONS_H

#include <string>

#include "struct.h"

using namespace std;


class Distributions
{
  private:

    struct Distribution
    {
      vector<unsigned> west; // Ranks
      vector<unsigned> east;
      unsigned lenWest;
      unsigned lenEast;
      unsigned cases; // Combinatorial count
    };

    vector<vector<unsigned>> binomial;

    vector<Distribution> distributions;


    void setBinomial();

    void mirror(
      const unsigned len,
      const unsigned lenMid,
      unsigned& distIndex);


  public:

    Distributions();

    ~Distributions();

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
