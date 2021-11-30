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

    bool checkMinimals(
      const vector<CombEntry>& centries,
      const list<CombReference>& minimals) const;

    void checkReductions(
      const vector<CombEntry>& centries,
      const vector<Combination>& uniqs,
      const CombEntry& centry,
      const Strategies& strategies,
      const Distribution& distribution) const;

    bool getMinimalRanges(
      const vector<CombEntry>& centries,
      const vector<Combination>& uniqs,
      const CombEntry& centry,
      list<unsigned char>& rankLowest,
      unsigned char& range) const;


  public:

    bool checkAndFixMinimals(
      const vector<CombEntry>& centries,
      list<CombReference>& minimals) const;

    void checkAllMinimals(vector<CombEntry>& centries);

    void checkAllReductions(
      const unsigned cards,
      const vector<CombEntry>& centries,
      const vector<Combination>& uniqs,
      const Distributions& distributions) const;
};

#endif
