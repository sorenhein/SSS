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
template<typename T> class CoverStack;
template<typename T> struct StackEntry;


class CoverTableau
{
  friend StackEntry<Cover>;
  friend StackEntry<CoverRow>;

  private:

    list<CoverRow> rows;

    unsigned char tricksMin;

    Tricks residuals;

    Complexity complexity;

    // For an unfinished tableau, this is a lower bound based on the
    // candidates that still remain for that specific tableau.
    Complexity lowerBound;

    
    template<class T, typename C>
    CoverState attemptRow(
      C& candIter,
      CoverStack<T>& stack,
      CoverTableau& solution);

  protected:

    void addRow(const Cover& cover);

    void addRow(const CoverRow& row); 

    void extendRow(
      const Cover& cover,
      const Tricks& additions,
      const unsigned rawWeightAdded,
      const unsigned rowNo);

    // Dummy method (don't know how to avoid).
    void extendRow(
      const CoverRow& row,
      const Tricks& additions,
      const unsigned rawWeightAdded,
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
      CoverStack<Cover>& stack,
      CoverTableau& solution);

    bool attempt(
      const vector<unsigned char>& cases,
      set<CoverRow>::const_iterator& rowIter,
      CoverStack<CoverRow>& stack,
      CoverTableau& solution);

    unsigned char headroom(const CoverTableau& solution) const;

    void project(const unsigned char minCompAdder);

    // This takes projections into account -- see code.
    bool operator < (const CoverTableau& ct2) const;

    bool compareAgainstPartial(const CoverTableau& partial) const;

    void updateStats(
      const Profile& sumProfile,
      ProductStats& productStats,
      const bool newTableauFlag) const;

    bool complete() const;

    unsigned getResidualWeight() const;

    string strBracket() const;

    string strResiduals() const;

    string str(const Profile& sumProfile) const;

string lowerTMP() const;
};


// TMP TODO
struct Edata
{
  size_t stackActual;
  unsigned firstFix;
  unsigned numSolutions;
  size_t stackMax;
  unsigned numSteps;
  unsigned numCompares;
  size_t numBranches;

  void reset()
  {
    stackActual = 0;
    firstFix = 0;
    numSolutions = 0;
    stackMax = 0;
    numSteps = 0;
    numCompares = 0;
    numBranches = 0;
  };

  string strHeader() const
  {
    stringstream ss;
    ss << 
      setw(2) << "" <<
      setw(12) << "Stack" <<
      setw(8) << "TTFF" <<
      setw(8) << "Numsol" <<
      setw(12) << "Smax" <<
      setw(12) << "Comps" <<
      setw(12) << "Steps" <<
      setw(8) << "Branch" << "\n";
    return ss.str();
  };

  string str(const string& ID) const
  {
    stringstream ss;
    ss << 
      setw(2) << ID <<
      setw(12) << stackActual <<
      setw(8) << firstFix <<
      setw(8) << numSolutions <<
      setw(12) << stackMax <<
      setw(12) << numCompares <<
      setw(12) << numSteps <<
      setw(8) << setprecision(2) << fixed <<
        static_cast<float>(numBranches) / static_cast<float>(numSteps) << "\n";
    return ss.str();
  };
};


template<class T, typename C>
CoverState CoverTableau::attemptRow(
  C& candIter,
  CoverStack<T>& stack,
  CoverTableau& solution)
{
  // Returns true if a solution is found by adding candIter as a new row, 
  // even if the solution is inferior to the existing one.  
  // This method works both for the exhaustive search 
  // (T == CoverStackEntry) and for the row search 
  // (T == RowStackEntry).

  if (! (candIter->getTricks() <= residuals))
    return COVER_IMPOSSIBLE;

  if (candIter->getWeight() < residuals.getWeight())
  {
    stack.emplace(candIter, * this);
    return COVER_OPEN;
  }
  else if (solution.rows.empty())
  {
    // We have a solution for sure, as it is the first one.
    solution = * this;
    solution.addRow(* candIter);
edata.numSolutions++;
    return COVER_DONE;
  }
  else
  {
    // We can use this CoverTableau, as the stack element is about
    // to be popped anyway.
    CoverTableau::addRow(* candIter);
    if (complexity < solution.complexity)
    {
      solution = * this;
edata.numSolutions++;
    }
    return COVER_DONE;
  }
}

#endif
