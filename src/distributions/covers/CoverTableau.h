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

    
    template<class T, typename C>
    bool attemptRow(
      const vector<unsigned char>& cases,
      C& candIter,
      list<T>& stack,
      Tricks& additions,
      unsigned char weightAdded,
      CoverTableau& solution);

    void addRow(
      const Cover& cover,
      const Tricks& tricks,
      const vector<unsigned char>& cases);

    void addRow(
      const CoverRow& row,
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

    bool attempt(
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


template<class T, typename C>
bool CoverTableau::attemptRow(
  const vector<unsigned char>& cases,
  C& candIter,
  list<T>& stack,
  Tricks& additions,
  unsigned char weightAdded,
  CoverTableau& solution)
{
  // Return true if a solution is found, even if it is inferior to
  // the existing one.  This method works both for the exhaustive
  // search (where T is StackEntry) and for the row search
  // (where T is RowStackEntry).

  if (! candIter->possible(residuals, cases, additions, weightAdded))
    return false;

  if (weightAdded < residualWeight)
  {
    stack.emplace_back(T());
    T& entry = stack.back();
    entry.iter = candIter;

    CoverTableau& tableau = entry.tableau;
    tableau = * this;
    tableau.addRow(* candIter, additions, cases);

    tableau.residuals -= additions;
    tableau.residualWeight -= weightAdded;


    return false;
  }
  else if (solution.rows.empty())
  {
    // We have a solution for sure, as it is the first one.
    solution = * this;
    solution.addRow(* candIter, additions, cases);
    return true;
  }
  else
  {
    // We can use this CoverTableau, as the stack element is about
    // to be popped anyway.
    
    CoverTableau::addRow(* candIter, additions, cases);

    if (complexity < solution.complexity)
      solution = * this;
    return true;
  }
}

#endif
