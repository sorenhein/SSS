/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERTABLEAU_H
#define SSS_COVERTABLEAU_H

#include <list>
#include <vector>
#include <string>

#include "CoverRow.h"

using namespace std;

/* A tableau is a set of additive rows.
 */

class Result;
struct StackTableau;


class CoverTableau
{
  private:

    list<CoverRow> rows;

    vector<unsigned char> residuals;

    unsigned char residualsSum;


  public:

    CoverTableau();

    void reset();

    void setTricks(const list<Result>& tricks);

    bool attemptGreedy(const CoverNew& cover);

    void attemptExhaustive(
      list<CoverNew>::const_iterator& coverIter,
      list<StackTableau>& stack,
      list<CoverTableau>& solutions) const;

    bool operator < (const CoverTableau& tableau2) const;

    bool complete() const;

    unsigned char getComplexity() const;

    unsigned char numRows() const;

    unsigned char numCovers() const;

    string str(
      const unsigned char lengthActual,
      const vector<unsigned char>& topsActual) const;
};


struct StackTableau
{
  CoverTableau tableau;

  list<CoverNew>::const_iterator coverIter;
};


#endif
