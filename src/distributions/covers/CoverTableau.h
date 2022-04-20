/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERTABLEAU_H
#define SSS_COVERTABLEAU_H

#include <list>
#include <set>
#include <vector>
#include <string>

#include "CoverRow.h"
#include "Tricks.h"

#include "product/Profile.h"

using namespace std;

/* A tableau is a set of additive rows.
 */

class Result;
struct StackTableau;
struct ResTableau;
class ResExpl;
class ProductStats;


class CoverTableau
{
  private:

    Profile sumProfile;

    list<CoverRow> rows;

    Tricks residuals;

    unsigned char residualsSum;

    unsigned char tricksMin;


  public:

    CoverTableau();

    void reset();

    void setBoundaries(const Profile& sumProfile);

    void setTricks(
      const Tricks& tricks,
      const unsigned char tmin);

    void setMinTricks(const unsigned char tmin);

    bool attemptGreedy(const Cover& cover);

    void attemptExhaustiveRow(
      list<CoverRow>::const_iterator& rowIter,
      const unsigned coverNo,
      list<ResTableau>& stack,
      list<CoverTableau>& solutions,
      unsigned char& lowestComplexity) const;

    void attemptExhaustive(
      set<Cover>::const_iterator& coverIter,
      const unsigned coverNo,
      list<StackTableau>& stack,
      list<CoverTableau>& solutions,
      unsigned char& lowestComplexity) const;

    void updateStats(
      ProductStats& productStats,
      const bool newTableauFlag) const;

    // void toResExpl(ResExpl& resExpl) const;

    bool operator < (const CoverTableau& tableau2) const;

    bool complete() const;

    unsigned char getComplexity() const;

    unsigned char getOverlap() const;

    unsigned char numRows() const;

    unsigned char numCovers() const;

    unsigned char getResidual() const;

    string str() const;

    string strResiduals() const;
};


struct StackTableau
{
  CoverTableau tableau;

  set<Cover>::const_iterator coverIter;

  // May not need this long-term.  May in fact go inside Cover.
  unsigned coverNumber;
};

struct ResTableau
{
  CoverTableau tableau;

  list<CoverRow>::const_iterator rowIter;

  unsigned rowNumber;
};

#endif
