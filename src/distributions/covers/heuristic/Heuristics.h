/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_HEURISTICS_H
#define SSS_HEURISTICS_H

#include <set>
#include <string>

#include "Partial.h"

#include "../Tricks.h"
#include "../Cover.h"
#include "../Complexity.h"

class CoverStore;
class CoverTableau;
class Explain;

using namespace std;


class Heuristics
{
  struct PartialBest
  {
    Partial const * ptr1;
    Partial const * ptr2;
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
      Partial const * partial1Ptr,
      Partial const * partial2Ptr,
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

  struct Dominant
  {
    PartialBest partialBest;
    Tricks tricks;
  };


  private:

    multiset<Partial> partials;


    void emplace(
      Cover const * coverPtr,
      const Tricks& additions,
      const unsigned rawWeightAdder);

    bool combineSimply(
      const Heuristics& heur2,
      CoverTableau& partialSolution,
      bool& combinedFlag) const;

    bool combineSimply(
      const Heuristics& heur2,
      list<CoverTableau>& partialSolutions,
      bool& combinedFlag) const;

    bool insertDominant(
      list<Dominant>& dominant,
      const Tricks& tricks,
      Partial const * partial1Ptr,
      Partial const * partial2Ptr,
      const bool flag1In,
      const bool flag2In,
      const bool flagIndepIn) const;

    void setPartialSolution(
      const PartialBest& partialBest,
      const vector<unsigned char>& cases,
      CoverTableau& partialSolution) const;


  public:

    Heuristics();

    void findHeaviestN(
      const CoverStore& coverStore,
      const Tricks& tricks,
      const vector<unsigned char>& cases,
      const Explain& explain,
      const size_t numHeaviest);
    
    bool combine(
      const Heuristics& heur2,
      const Tricks& tricks,
      const vector<unsigned char>& cases,
      CoverTableau& partialSolution) const;

    bool combine(
      const Heuristics& heur2,
      const Tricks& tricks,
      const vector<unsigned char>& cases,
      list<CoverTableau>& partialSolutions) const;

    string str() const;
};

#endif
