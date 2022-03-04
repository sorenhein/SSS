/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERS_H
#define SSS_COVERS_H

#include <list>
#include <vector>
#include <string>

#include "Cover.h"
#include "CoverNew.h"

class Distribution;
class CoverMemory;
class CoverTableau;
class Result;
class ResExpl;


using namespace std;


class Covers
{
  private:

    struct CoverStackInfo
    {
      vector<unsigned char> topsLow;
      vector<unsigned char> topsHigh;

      unsigned char minWest; // Sum of West's top minima
      unsigned char minEast; // Sum of East's top minima
      unsigned char maxDiff; // Large difference max-min for a top
      unsigned char maxWest; // Largest West maximum
      unsigned char maxEast; // Largest East maximum
      unsigned char topNext; // Running top number
 
      CoverStackInfo(const vector<unsigned char>& topTotals)
      {
        topsLow.resize(topTotals.size(), 0);
        topsHigh = topTotals;
 
        minWest = 0;
        minEast = 0;
        maxDiff = 0;
        maxWest = 0;
        maxEast = 0;

        // Never need to set the lowest rank explicitly.
        topNext = 1;
      };
    };


    list<Cover> covers;

    list<CoverNew> coversNew;


    void setup(
      const list<Result>& results,
      vector<unsigned char>& tricks,
      unsigned char& tricksMin) const;

    void prune();

    string strDebug(
      const string& title,
      const vector<unsigned char>& tricks) const;


  public:

    Covers();

    void reset();

    void prepare(
      const CoverMemory& coverMemory,
      const unsigned char maxLength,
      const unsigned char maxTops,
      const vector<unsigned char>& lengths,
      const vector<unsigned char>& tops,
      const vector<unsigned char>& cases);

    void prepareNew(
      const vector<unsigned char>& lengths,
      vector<vector<unsigned> const *>& topPtrs,
      const vector<unsigned char>& cases,
      const unsigned char maxLength,
      const vector<unsigned char>& topTotals);

    CoverState explain(
      const list<Result>& results,
      ResExpl& resExpl) const;

    void explainGreedy(
      const list<Result>& results,
      const unsigned numStrategyTops,
      CoverTableau& tableau) const;

    void explainExhaustive(
      const list<Result>& results,
      const unsigned numStrategyTops,
      CoverTableau& tableau) const;
};

#endif
