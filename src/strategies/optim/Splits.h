#ifndef SSS_SPLITS_H
#define SSS_SPLITS_H

#include <list>
#include <string>

#include "../Strategies.h"

using namespace std;


class Splits
{
  private:

    // Contains the splits of a Strategies into overlapping and
    // unique components, relative to another Strategies.
    // Also contains numbered pointers to the Strategy's whence
    // they came, and a cross matrix of pre-calculated comparisons
    // of the own Strategy's.

    Strategies own;

    Strategies shared;

    vector<Strategy const *> ownPtrs;

    vector<vector<CompareDetail>> matrix;

    unsigned count;


    void pushDistribution(
      Strategies& strats,
      const StratData& stratData);

    void splitDistributions(
      Strategies& strategies,
      const Strategy& counterpart);

    void setPointers();

    void setMatrix(ComparatorType comp);


  public:

    Splits();

    ~Splits();

    void reset();

    void split(
      Strategies& strategies,
      const Strategy& counterpart,
      ComparatorType comp);

    const list<Strategy>& sharedStrategies() const;

    const Strategy& ownStrategy(const unsigned index) const;

    Compare compare(
      const unsigned index1,
      const unsigned index2) const;

    CompareDetail compareDetail(
      const unsigned index1,
      const unsigned index2) const;

    unsigned weight(const unsigned index) const;
};

#endif
