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
#include "../CoverCategory.h"


using namespace std;

class Cover;
class CoverRow;
class Profile;
class RowMatches;
class RanksNames;
class ProductStats;
enum CoverVerbal: unsigned;
template<typename T> class CoverStack;
template<typename T> struct StackEntry;


class CoverTableau
{
  friend StackEntry<Cover>;
  friend StackEntry<CoverRow>;
  friend class Covers;
  friend class RowMatches;
  friend class Heuristics;

  private:

    unsigned char tricksMin;

    Tricks residuals;

    Complexity complexity;

    // For an unfinished tableau, this is a lower bound based on the
    // candidates that still remain for that specific tableau.
    Complexity lowerBound;

    bool trivialFlag;

    // TODO These are only needed for string outputs.
    size_t depth;
    CoverSymmetry tableauSymmetry;

    
    template<class T>
    CoverState attemptRow(
      typename set<T>::const_iterator& candIter,
      CoverStack<T>& stack,
      CoverTableau& solution);

  protected:

    // Only RowMatches uses this (and can destroy it).
    list<CoverRow> rows;

    void addRow(
      const Cover& cover,
      const CoverVerbal);

    void addRow(
      const CoverRow& row,
      const CoverVerbal verbal = VERBAL_GENERAL); 

    void extendRow(
      const Cover& cover,
      const Tricks& additions,
      const unsigned rawWeightAdded,
      const unsigned rowNo,
      const CoverVerbal verbal);

    // Dummy method (don't know how to avoid).
    void extendRow(
      const CoverRow& row,
      const Tricks& additions,
      const unsigned rawWeightAdded,
      const unsigned rowNo,
      const CoverVerbal verbal);


  public:

   CoverTableau();

    void reset();

    void init(
      const Tricks& tricks,
      const unsigned char tmin);

    void resize(const size_t numDist);

    // TODO TMP
    void initStrData(
      size_t depthIn,
      CoverSymmetry coverSymmetry);

    void setMinTricks(const unsigned char tmin);

    void setTrivial(const unsigned char tmin);

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

    CoverTableau& operator += (const CoverTableau& tableau2);

    unsigned char headroom(const CoverTableau& solution) const;

    void project(const unsigned char minCompAdder);

    void setVerbal(const Profile& sumProfile);

    void sortVerbally();

    // This takes projections into account -- see code.
    bool operator < (const CoverTableau& ct2) const;

    bool compareAgainstPartial(
      const CoverTableau& partial,
      const unsigned complexityAdder = 0) const;

    void partitionResiduals(
      Tricks& tricksSymmetric,
      Tricks& tricksAntisymmetric,
      const vector<unsigned char>& cases) const;

    void sliceResiduals(
      vector<Tricks>& tricksWithinLength,
      vector<Tricks>& tricksOfLength,
      const vector<unsigned char>& cases) const;

    void updateStats(
      const Profile& sumProfile,
      ProductStats& productStats,
      const bool newTableauFlag) const;

    bool used() const;

    bool complete() const;

    unsigned lowerComplexityBound() const;

    unsigned getResidualWeight() const;

    string strBracket() const;

    string strResiduals() const;

    string str(
      const Profile& sumProfile,
      const RanksNames& ranksNames) const;

string lowerTMP() const;
};

#endif
