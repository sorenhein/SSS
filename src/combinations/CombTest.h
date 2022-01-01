#ifndef SSS_COMBTEST_H
#define SSS_COMBTEST_H

#include <vector>

using namespace std;

class Distributions;
class Distribution;
class Strategies;
class Combination;
struct CombEntry;
struct CombReference;


class CombTest
{
  private:

    void checkReductions(
      const vector<CombEntry>& centries,
      const vector<Combination>& uniqs,
      const CombEntry& centry,
      const Strategies& strategies,
      const unsigned char maxRank,
      const Distribution& distribution) const;

    void dumpInputs(
      const string& title,
      const vector<CombEntry>& centries,
      const CombEntry& centry,
      const Strategies& strategies,
      const unsigned char maxRank) const;

    void dumpSpans(
      list<unsigned char> ranksHigh,
      list<unsigned char> winRanksLow) const;

  public:

    void checkAllMinimals(vector<CombEntry>& centries) const;

    void checkAllReductions(
      const unsigned cards,
      const vector<CombEntry>& centries,
      const vector<Combination>& uniqs,
      const Distributions& distributions) const;
};

#endif
