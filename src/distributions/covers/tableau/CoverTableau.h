/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_COVERTABLEAU_H
#define SSS_COVERTABLEAU_H

// A tableau is a set of additive rows with some overlap.

#include <list>
#include <set>
#include <vector>
#include <string>

#include "../CoverRow.h"
#include "../Tricks.h"
#include "../Complexity.h"
#include "../CoverState.h"


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

    Complexity complexity;

    
    template<class T, typename C>
    CoverState attemptRow(
      C& candIter,
      list<T>& stack,
      Tricks& additions,
      CoverTableau& solution);

    void addRow(
      const Cover& cover,
      const Tricks& additions);

    void addRow(
      const CoverRow& row,
      const Tricks& additions);

    void extendRow(
      const Cover& cover,
      const Tricks& additions,
      const unsigned rowNo);


  public:

   CoverTableau();

    void reset();

    void init(
      const Tricks& tricks,
      const unsigned char tmin);

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

    unsigned getResidualWeight() const;

    string strBracket() const;

    string strResiduals() const;

    string str(const Profile& sumProfile) const;
};


template<class T, typename C>
CoverState CoverTableau::attemptRow(
  C& candIter,
  list<T>& stack,
  Tricks& additions,
  CoverTableau& solution)
{
  // Returns true if a solution is found by adding candIter as a new row, 
  // even if the solution is inferior to the existing one.  
  // This method works both for the exhaustive search (T == StackEntry) 
  // and for the row search (T == RowStackEntry).

  if (! candIter->possible(residuals, additions))
    return COVER_IMPOSSIBLE;

  if (additions.getWeight() < residuals.getWeight())
  {
    stack.emplace_back(T());
    T& entry = stack.back();
    entry.iter = candIter;

    CoverTableau& tableau = entry.tableau;
    tableau = * this;
    tableau.addRow(* candIter, additions);
    return COVER_OPEN;
  }
  else if (solution.rows.empty())
  {
    // We have a solution for sure, as it is the first one.
    solution = * this;
    solution.addRow(* candIter, additions);
    return COVER_DONE;
  }
  else
  {
    // We can use this CoverTableau, as the stack element is about
    // to be popped anyway.
    CoverTableau::addRow(* candIter, additions);
    if (complexity < solution.complexity)
      solution = * this;
    return COVER_DONE;
  }
}

#endif
