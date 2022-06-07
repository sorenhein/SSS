/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverTableau.h"
#include "TableauStats.h"

#include "../Cover.h"
#include "../CoverStore.h"
#include "../RowStore.h"
#include "../CoverStack.h"

#include "../../../strategies/result/Result.h"

TableauStats tableauStats;


CoverTableau::CoverTableau()
{
  CoverTableau::reset();
}


void CoverTableau::reset()
{
  rows.clear();
  tricksMin = 0;

  residuals.clear();

  complexity.reset();
  lowerBound.reset();
}


void CoverTableau::init(
  const Tricks& tricks,
  const unsigned char tmin)
{
  tricksMin = tmin;
  residuals = tricks;
}


void CoverTableau::setMinTricks(const unsigned char tmin)
{
  tricksMin = tmin;
}


void CoverTableau::addRow(const Cover& cover)
{
  rows.emplace_back(CoverRow());
  CoverRow& row = rows.back();
  row.resize(residuals.size());
  row.add(cover, cover.getTricks(), cover.getWeight(), residuals);
  complexity.addRow(row.getComplexity(), row.getRawWeight());

  const unsigned char minCompAdder = 
    cover.minComplexityAdder(residuals.getWeight());
  CoverTableau::project(minCompAdder);
}


void CoverTableau::addRow( const CoverRow& row)
{
  rows.push_back(row);
  complexity.addRow(row.getComplexity(), row.getRawWeight());
  residuals -= row.getTricks();

  const unsigned char minCompAdder = 
    row.minComplexityAdder(residuals.getWeight());
  CoverTableau::project(minCompAdder);
}


void CoverTableau::extendRow(
  const Cover& cover,
  const Tricks& additions,
  const unsigned rawWeight,
  const unsigned rowNo)
{
  // A bit fumbly: Advance to the same place as we were at.
  list<CoverRow>::iterator riter;
  unsigned r;
  for (riter = rows.begin(), r = 0; r < rowNo; riter++, r++);

  riter->add(cover, additions, rawWeight, residuals);

  complexity.addCover(cover.getComplexity(), riter->getComplexity(),
    rawWeight);

  const unsigned char minCompAdder = 
    cover.minComplexityAdder(residuals.getWeight());
  CoverTableau::project(minCompAdder);
}


void CoverTableau::extendRow(
  [[maybe_unused]] const CoverRow& row,
  [[maybe_unused]] const Tricks& additions,
  [[maybe_unused]] const unsigned rawWeight,
  [[maybe_unused]] const unsigned rowNo)
{
  // Need this method to get out of the StackEntry template,
  // or at least I don't know how to avoid it.
  assert(false);
}


template<class T>
CoverState CoverTableau::attemptRow(
  typename set<T>::const_iterator& candIter,
  CoverStack<T>& stack,
  CoverTableau& solution)
{
  // Returns true if a solution is found by adding candIter as a new row, 
  // even if the solution is inferior to the existing one.  
  // This method works both for the exhaustive search 
  // (T == Cover) and for the row search (T == CoverRow).

  if (! (candIter->getTricks() <= residuals))
    return COVER_IMPOSSIBLE;

  if (candIter->getWeight() < residuals.getWeight())
  {
    if (solution.rows.empty() ||
        ! solution.compareAgainstPartial(* this, candIter->getComplexity()))
    {
      auto sit = * stack.emplace(candIter, * this);
    }
    return COVER_OPEN;
  }
  else if (solution.rows.empty())
  {
    // We have a solution for sure, as it is the first one.
    solution = * this;
    solution.addRow(* candIter);
    tableauStats.numSolutions++;
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
      tableauStats.numSolutions++;
    }
    return COVER_DONE;
  }
}


bool CoverTableau::attempt(
  const vector<unsigned char>& cases,
  set<Cover>::const_iterator& coverIter,
  CoverStack<Cover>& stack,
  CoverTableau& solution)
{
  // Returns true if this must be the last use of this cover.
  // Check whether we can make a complete solution with the cover.
  tableauStats.numCompares++;
  const CoverState state = CoverTableau::attemptRow(
    coverIter, stack, solution);
  if (state == COVER_OPEN)
    return false;
  else if (state == COVER_DONE)
    return true;

  // If not, try to add the cover to existing rows.
  Tricks additions;
  additions.resize(residuals.size());

  unsigned rno = 0;
  unsigned rawWeightAdded = 0;
  for (auto& row: rows)
  {
    if (row.size() >= 2)
    {
      // A row becomes too difficult to read for a human if it
      // has more than two options.
      rno++;
      continue;
    }
    else if (! row.possibleAdd(* coverIter, residuals, cases, 
        additions, rawWeightAdded))
    {
      tableauStats.numCompares++;
      // The row does not fit.
      rno++;
      continue;
    }

    if (additions.getWeight() < residuals.getWeight())
    {
      tableauStats.numCompares++;
      if (solution.rows.empty())
      {
        // The cover can definitely be added on the way to a solution.
        stack.emplace(coverIter, * this, additions, rawWeightAdded, rno);
      }
      else 
      {
        // Check that a new stack entry still has a chance against
        // the solution.
        Complexity ctmp = complexity;

        // The remaining residual still needs covers.
        unsigned char mca = coverIter->minComplexityAdder(
          residuals.getWeight() - additions.getWeight());

        // Use the fact that a cover complexity is always at least 2
        // (see Term::set).
        if (mca < 2)
          mca = 2;

        // We could in principle also reproduce the new row
        // complexity, but we stick to the overall complexity.
        ctmp.addCover(coverIter->getComplexity() + mca, 0, 0);

        if (! (solution.complexity < ctmp))
        {
          // Only add if it might still beat the solution.
          auto sit =
            * stack.emplace(coverIter, * this, 
            additions, rawWeightAdded, rno);
        }
      }
    }
    else if (complexity.match(coverIter->getComplexity(),
        row.getComplexity(), coverIter->getWeight(), solution.complexity))
    {
      // The cover makes a solution which beats the previous one.
      tableauStats.numCompares++;
      tableauStats.numSolutions++;
      solution = * this;
      solution.extendRow(* coverIter, additions, rawWeightAdded, rno);
    }
    else
    {
      // The cover makes a solution which loses to the previous one.
      tableauStats.numCompares++;
    }

    rno++;
  }

  return false;
}


bool CoverTableau::attempt(
  [[maybe_unused]] const vector<unsigned char>& cases,
  set<CoverRow>::const_iterator& rowIter,
  CoverStack<CoverRow>& stack,
  CoverTableau& solution)
{
  // Return true if a solution is found, even if it is inferior to
  // the existing one.
  tableauStats.numCompares++;
  return (CoverTableau::attemptRow(rowIter, stack, solution) == COVER_DONE);
}


CoverTableau& CoverTableau::operator += (const CoverTableau& tableau2)
{
  // This is only used for finished tableaux, e.g. for the symmetric
  // and asymmetric parts.
  for (auto& row2: tableau2.rows)
    rows.push_back(row2);

  assert(tricksMin == tableau2.tricksMin);

  // Ignore lowerBound.
  complexity += tableau2.complexity;
  
  return * this;
}


unsigned char CoverTableau::headroom(const CoverTableau& solution) const
{
  return complexity.headroom(solution.complexity);
}


void CoverTableau::project(const unsigned char minCompAdder)
{
  lowerBound = complexity;

  // We cannot reliably adjust up the row complexity nor the raw weight.
  lowerBound.addCover(minCompAdder, 0, 0);
}


bool CoverTableau::operator < (const CoverTableau& ct2) const
{
  if (CoverTableau::complete())
  {
    if (ct2.complete())
      return (complexity < ct2.complexity);
    else
      return (complexity < ct2.lowerBound);
  }
  else
  {
    if (ct2.complete())
      return (lowerBound < ct2.complexity);
    else
      return (lowerBound < ct2.lowerBound);
  }
}


bool CoverTableau::compareAgainstPartial(
  const CoverTableau& partial,
  const unsigned complexityAdder) const
{
  return complexity.compareAgainstPartial(
    partial.complexity, complexityAdder);
}


void CoverTableau::partitionResiduals(
  Tricks& tricksSymmetric,
  Tricks& tricksAntisymmetric,
  const vector<unsigned char>& cases) const
{
  residuals.partition(tricksSymmetric, tricksAntisymmetric, cases);
}


void CoverTableau::updateStats(
  const Profile& sumProfile,
  ProductStats& productStats,
  const bool newTableauFlag) const
{
  for (auto& row: rows)
    row.updateStats(productStats, sumProfile, newTableauFlag);
}


bool CoverTableau::used() const
{
  return (rows.size() > 0);
}


bool CoverTableau::complete() const
{
  return (rows.size() > 0 && residuals.getWeight() == 0);
}


unsigned CoverTableau::lowerComplexityBound() const
{
  return lowerBound.complexitySum();
}


unsigned CoverTableau::getResidualWeight() const
{
  return residuals.getWeight();
}


string CoverTableau::strBracket() const
{
  unsigned weight = 0;
  unsigned rawWeight = 0;
  for (auto& row: rows)
  {
    weight += row.getWeight();
    rawWeight += row.getRawWeight();
  }

  stringstream ss;
  ss << 
    "[c " << complexity.str() << 
    ", w " << weight << "/" << rawWeight << "]";
  return ss.str();
}


string CoverTableau::lowerTMP() const
{
  return lowerBound.str();
}


string CoverTableau::strResiduals() const
{
  return residuals.strSpaced();
}


string CoverTableau::str(const Profile& sumProfile) const
{
  stringstream ss;

  ss << "Always take at least " << +tricksMin << " tricks, and more when\n";
  for (auto& row: rows)
    ss << row.str(sumProfile);

  return ss.str() + "\n";
}

