/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_SURVIVORLIST_H
#define SSS_SURVIVORLIST_H

#include <vector>
#include <list>


using namespace std;

struct SideInfo;


// Survivor is used in the context of rank-reduced distributions.
// It can happen that a NS card leads two EW ranks to collapse after
// the trick.  The parent trick has a number of possible distributions,
// say 4, and the child trick only has 3 tricks.  These have to be
// mapped to one another.  There is a Survivor for each parent
// distribution.  The fullNo is the parent number and the reducedNo
// is the child number.

struct Survivor
{
  unsigned char fullNo;
  unsigned char reducedNo;

  bool operator == (const Survivor& s2) const
  {
    return (fullNo == s2.fullNo && reducedNo == s2.reducedNo);
  }

  bool operator != (const Survivor& s2) const
  {
    return ! (* this == s2);
  }
};


class SurvivorList
{
  private:

    list<Survivor> distNumbers;

    unsigned char reducedSize;


  public:

    SurvivorList();

    void clear();

    void resize(const unsigned len);

    void push_back(const Survivor& survivor);

    const Survivor& front() const;

    list<Survivor>::const_iterator begin() const;
    list<Survivor>::const_iterator end() const;

    bool operator == (const SurvivorList& sl2) const;

    void setVoid(const unsigned char);

    void collapse(
      const vector<SideInfo>& distCollapses,
      const SurvivorList& survivorsUnreduced);

    unsigned sizeFull() const;

    unsigned char sizeReduced() const;

    string str() const;
};

#endif
