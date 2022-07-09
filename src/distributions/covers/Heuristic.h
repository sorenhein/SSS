/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_HEURISTIC_H
#define SSS_HEURISTIC_H

#include <set>
#include <string>

#include "Tricks.h"
#include "Cover.h"
#include "Complexity.h"

class CoverStore;
// class Cover;
class CoverTableau;
class Explain;

using namespace std;


class Heuristic
{
  struct PartialCover
  {
    Cover const * coverPtr;
    Tricks additions;
    unsigned rawWeightAdder;

    bool operator < (const PartialCover& pc2) const
    {
      return (rawWeightAdder < pc2.rawWeightAdder);
    };

    void addCoverToComplexity(Complexity& complexity) const
    {
      // Add cover to a single row.
      assert(coverPtr != nullptr);
      complexity.addCoverSingleRow(
        coverPtr->getComplexity(),
        additions.getWeight());
    };

    void addRowToComplexity(Complexity& complexity) const
    {
      // Add a whole new row.
      assert(coverPtr != nullptr);
      complexity.addRow(
        coverPtr->getComplexity(),
        additions.getWeight());
    };

    string str() const
    {
      if (coverPtr == nullptr)
        return "nullptr";

      string s = coverPtr->strNumerical();
      s += additions.strSpaced();
      s += "Weight " + to_string(rawWeightAdder) + "\n";
      return s;
    };
  };


  struct PartialBest
  {
    PartialCover const * ptr1;
    PartialCover const * ptr2;
    bool flag1;
    bool flag2;
    bool flagIndep;

    PartialBest()
    {
      flag1 = false;
      flag2 = false;
      flagIndep = false;
    };

    void set(
      PartialCover const * partial1Ptr,
      PartialCover const * partial2Ptr,
      const bool flag1In,
      const bool flag2In,
      const bool flagIndepIn)
    {
      ptr1 = partial1Ptr;
      ptr2 = partial2Ptr;
      flag1 = flag1In;
      flag2 = flag2In;
      flagIndep = flagIndepIn;
    };
  };


  private:

    multiset<PartialCover> partials;


    void emplace(
      Cover const * coverPtr,
      const Tricks& additions,
      const unsigned rawWeightAdder);

    bool combineSimply(
      const Heuristic& heur2,
      CoverTableau& partialSolution,
      bool& combinedFlag) const;

    void setPartialSolution(
      const PartialBest& partialBest,
      const vector<unsigned char>& cases,
      CoverTableau& partialSolution) const;


  public:

    Heuristic();

    void findHeaviestN(
      const CoverStore& coverStore,
      const Tricks& tricks,
      const vector<unsigned char>& cases,
      const Explain& explain,
      const size_t numHeaviest);
    
    bool combine(
      const Heuristic& heur2,
      const Tricks& tricks,
      const vector<unsigned char>& cases,
      CoverTableau& partialSolution) const;

    string str() const;
};

#endif
