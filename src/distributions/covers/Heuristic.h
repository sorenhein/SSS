/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_HEURISTIC_H
#define SSS_HEURISTIC_H

#include <set>

#include "Tricks.h"

class CoverStore;
class Cover;
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
  };


  struct PartialBest
  {
    PartialCover const * ptr1;
    PartialCover const * ptr2;
    bool flag1;
    bool flag2;
    bool flagIndep;

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
};

#endif
