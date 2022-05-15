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
#include "../CoverStack.h"

#include "../../../strategies/result/Result.h"

// TODO
Edata edata;


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


bool CoverTableau::attempt(
  const vector<unsigned char>& cases,
  set<Cover>::const_iterator& coverIter,
  CoverStack<Cover>& stack,
  CoverTableau& solution)
{
  // Returns true if this must be the last use of this cover.
  // Check whether we can make a complete solution with the cover.
edata.numCompares++;
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
    }
    else if (! row.possibleAdd(* coverIter, residuals, cases, 
        additions, rawWeightAdded))
    {
edata.numCompares++;
      // The row does not fit.
    }
    else if (additions.getWeight() < residuals.getWeight())
    {
edata.numCompares++;
      // The cover can be added, but does not make a solution yet.
      stack.emplace(coverIter, * this, additions, rawWeightAdded, rno);
    }
    else if (complexity.match(coverIter->getComplexity(),
        row.getComplexity(), coverIter->getWeight(), solution.complexity))
    {
edata.numCompares++;
edata.numSolutions++;
      // The cover makes a solution which beats the previous one.
      solution = * this;
      solution.extendRow(* coverIter, additions, rawWeightAdded, rno);
    }
    else
edata.numCompares++;

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
edata.numCompares++;
  return (CoverTableau::attemptRow(rowIter, stack, solution) == COVER_DONE);
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
  // return (! rows.empty() && residuals.getWeight() == 0);
  return (residuals.getWeight() == 0);
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

