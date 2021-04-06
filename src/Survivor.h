#ifndef SSS_SURVIVOR_H
#define SSS_SURVIVOR_H

#include <list>


using namespace std;


// Survivor is used in the context of rank-reduced distributions.
// It can happen that a NS card leads two EW ranks to collapse after
// the trick.  The parent trick has a number of possible distributions,
// say 4, and the child trick only has 3 tricks.  These have to be
// mapped to one another.  There is a Survivor for each parent
// distribution.  The fullNo is the parent number and the reducedNo
// is the child number.

struct Survivor
{
  unsigned fullNo;
  unsigned reducedNo;
};


struct Survivors
{
  list<Survivor> distNumbers;
  unsigned reducedSize;

  void clear()
  {
    distNumbers.clear();
    reducedSize = 0;
  };

  void resize(const unsigned len)
  {
    distNumbers.resize(len);
  };

  void push_back(const Survivor& survivor)
  {
    distNumbers.push_back(survivor);
  };

  unsigned sizeFull() const
  {
    return distNumbers.size();
  };

  unsigned sizeReduced() const
  {
    return reducedSize;
  };
};

#endif
