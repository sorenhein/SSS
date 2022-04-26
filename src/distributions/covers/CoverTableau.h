/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERTABLEAU_H
#define SSS_COVERTABLEAU_H

/* A tableau is a set of additive rows with some overlap.
 */

#include <list>
#include <set>
#include <vector>
#include <string>

#include "CoverRow.h"
#include "Tricks.h"
#include "Complexity.h"


using namespace std;

class Cover;
class Profile;
class ProductStats;

struct StackEntry;
struct RowStackEntry;


class CoverTableau
{
  private:

    list<CoverRow> rows;
    unsigned char tricksMin;

    Tricks residuals;
    unsigned char residualWeight;

    Complexity complexity;


    void addRow(
      const Cover& cover,
      const Tricks& tricks,
      const vector<unsigned char>& cases);



  public:

   CoverTableau();

    void reset();

    void init(
      const Tricks& tricks,
      const unsigned char tmin,
      const vector<unsigned char>& cases);

    void setMinTricks(const unsigned char tmin);

    bool attempt(
      const vector<unsigned char>& cases,
      set<Cover>::const_iterator& coverIter,
      list<StackEntry>& stack,
      CoverTableau& solution);

    bool attemptManually(
      const vector<unsigned char>& cases,
      list<CoverRow>::const_iterator& rowIter,
      list<RowStackEntry>& stack,
      CoverTableau& solution);

    unsigned char headroom(const CoverTableau& solution) const;

    void updateStats(
      const Profile& sumProfile,
      ProductStats& productStats,
      const bool newTableauFlag) const;

    bool complete() const;

    unsigned char getResidualWeight() const;

    string strBracket() const;

    string strResiduals() const;

    string str(const Profile& sumProfile) const;
};


// TODO CoverStacks.h, included from CoverTableau.cpp, Covers.cpp
struct StackEntry
{
  CoverTableau tableau;

  set<Cover>::const_iterator coverIter;
};

struct RowStackEntry
{
  CoverTableau tableau;

  list<CoverRow>::const_iterator rowIter;
};

#endif
