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

#include "../Cover.h"
#include "../CoverStacks.h"

#include "../../../strategies/result/Result.h"


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


void CoverTableau::addRow(
  const Cover& cover,
  const Tricks& additions)
{
  rows.emplace_back(CoverRow());
  CoverRow& row = rows.back();
  row.resize(residuals.size());
  row.add(cover, additions, residuals);
  complexity.addRow(row.getComplexity());
}


void CoverTableau::addRow(
  const CoverRow& row,
  const Tricks& additions)
{
  rows.push_back(row);
  complexity.addRow(row.getComplexity());
  residuals -= additions;
}


void CoverTableau::extendRow(
  const Cover& cover,
  const Tricks& additions,
  const unsigned rowNo)
{
  // A bit fumbly: Advance to the same place as we were at.
  list<CoverRow>::iterator riter;
  unsigned r;
  for (riter = rows.begin(), r = 0; r < rowNo; riter++, r++);

  riter->add(cover, additions, residuals);

  complexity.addCover(cover.getComplexity(), riter->getComplexity());
}


bool CoverTableau::attempt(
  const vector<unsigned char>& cases,
  set<Cover>::const_iterator& coverIter,
  list<StackEntry>& stack,
  CoverTableau& solution)
{
  // Returns true if this must be the last use of this cover.
  Tricks additions;
  additions.resize(residuals.size());

  // Check whether we can make a complete solution with the cover.
  const CoverState state = CoverTableau::attemptRow(
    coverIter, stack, additions, solution);
  if (state == COVER_OPEN)
    return false;
  else if (state == COVER_DONE)
    return true;

  // If not, try to add the cover to existing rows.
  unsigned rno = 0;
  for (auto& row: rows)
  {
    if (row.size() >= 2)
    {
      // A row becomes too difficult to read for a human if it
      // has more than two options.
    }
    else if (! row.possibleAdd(* coverIter, residuals, cases, additions))
    {
      // The row does not fit.
    }
    else if (additions.getWeight() < residuals.getWeight())
    {
      // The cover can be added, but does not make a solution yet.
      stack.emplace_back(StackEntry());
      StackEntry& entry = stack.back();
      entry.iter = coverIter;
      entry.tableau = * this;
      entry.tableau.extendRow(* coverIter, additions, rno);
    }
    else if (complexity.match(coverIter->getComplexity(),
        row.getComplexity(), solution.complexity))
    {
      // The cover makes a solution which beats the previous one.
      solution = * this;
      solution.extendRow(* coverIter, additions, rno);
    }

    rno++;
  }

  return false;
}


bool CoverTableau::attempt(
  [[maybe_unused]] const vector<unsigned char>& cases,
  list<CoverRow>::const_iterator& rowIter,
  list<RowStackEntry>& stack,
  CoverTableau& solution)
{
  // Return true if a solution is found, even if it is inferior to
  // the existing one.
  Tricks additions;
  additions.resize(residuals.size());

  return (CoverTableau::attemptRow(rowIter, stack,
    additions, solution) == COVER_DONE);
}


unsigned char CoverTableau::headroom(const CoverTableau& solution) const
{
  return complexity.headroom(solution.complexity);
}


void CoverTableau::updateStats(
  const Profile& sumProfile,
  ProductStats& productStats,
  const bool newTableauFlag) const
{
  for (auto& row: rows)
    row.updateStats(productStats, sumProfile, newTableauFlag);
}


bool CoverTableau::complete() const
{
  return (! rows.empty() && residuals.getWeight() == 0);
}


unsigned CoverTableau::getResidualWeight() const
{
  return residuals.getWeight();
}


string CoverTableau::strBracket() const
{
  unsigned weight = 0;
  for (auto& row: rows)
    weight += row.getWeight();

  stringstream ss;
  ss << "[c " << complexity.str() << ", w " << weight << "]";
  return ss.str();
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

