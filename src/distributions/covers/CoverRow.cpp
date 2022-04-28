/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverRow.h"
#include "Cover.h"


CoverRow::CoverRow()
{
  CoverRow::reset();
}


void CoverRow::reset()
{
  coverPtrs.clear();
  tricks.clear();
  weight = 0;
  complexity = 0;
}


void CoverRow::resize(const unsigned len)
{
  tricks.resize(len);
}


void CoverRow::fillDirectly(
  list<Cover const *>& coverPtrsIn,
  const vector<unsigned char>& cases)
{
  // This method is used to manually set cover rows only.
  // There is no need to check against trick vectors.
  coverPtrs = coverPtrsIn;

  complexity = 0;
  for (auto coverPtr: coverPtrs)
  {
    coverPtr->tricksOr(tricks);
    assert(complexity + coverPtr->getComplexity() > complexity);
    complexity += coverPtr->getComplexity();
  }

  tricks.weigh(cases, weight);
  assert(weight != 0);
}


bool CoverRow::possibleAdd(
  const Cover& cover,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned char& weightAdded) const
{
  // Do not update any internal states.  Try to add the cover with
  // its residuals and see what this would add incrementally to the row.
  // This is used in the exhaustive search for the least complex covers.

  assert(tricks.size() == residuals.size());
  assert(additions.size() == residuals.size());
  assert(cases.size() == residuals.size());

  // This is just an optimization.  If a cover has already been
  // OR'ed onto the row, it won't help to OR it again.
  if (! coverPtrs.empty() && coverPtrs.back() == &cover)
    return false;

  return cover.possible(tricks, residuals, cases, additions, weightAdded);
}


bool CoverRow::possible(
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned char& weightAdded) const
{
  // residuals: The overall tricks in cover tableau that remains.
  // additions: If the cover can be added, the additions to the
  //   explained vector that would arise
  // weightAdded: The number of cases in additions

  return tricks.possible(residuals, cases, additions, weightAdded);
}


void CoverRow::add(
  const Cover& cover,
  const Tricks& additions,
  const vector<unsigned char>& cases,
  Tricks& residuals,
  unsigned char& residualWeight)
{
  coverPtrs.push_back(&cover);

  tricks.add(additions, cases, residuals, residualWeight);

  // TODO Keep checking until we're sure we don't get overflow.
  assert(complexity + cover.getComplexity() > complexity);

  complexity += cover.getComplexity();
}


void CoverRow::updateStats(
  ProductStats& productStats,
  const Profile& sumProfile,
  const bool newTableauFlag) const
{
  for (auto coverPtr: coverPtrs)
    coverPtr->updateStats(productStats, sumProfile, newTableauFlag);
}


unsigned CoverRow::size() const
{
  return coverPtrs.size();
}


bool CoverRow::operator < (const CoverRow& rows2) const
{
  return (
      (complexity << 20) / weight < 
      (rows2.complexity << 20) / rows2.weight);
}


unsigned char CoverRow::effectiveDepth() const
{
  return 1;
}


unsigned CoverRow::getWeight() const
{
  return weight;
}


unsigned char CoverRow::getComplexity() const
{
  return complexity;
}


unsigned char CoverRow::minComplexityAdder(
  const unsigned char resWeight) const
{
  // The rows are ordered by increasing "complexity per weight"
  // (micro-cpw).  We round up the minimum number of covers needed
  // unless we hit an exact divisor.

  const unsigned mcpw = (complexity << 20) / weight;

  return static_cast<unsigned char>(1 + ((resWeight * mcpw - 1) >> 20));
}


string CoverRow::strHeader() const
{
  return 
    coverPtrs.front()->strHeaderTricksShort() +
    coverPtrs.front()->strHeader();
}


string CoverRow::str(const Profile& sumProfile) const
{
  stringstream ss;

  ss << "* " << coverPtrs.front()->str(sumProfile);

  for (auto iter = next(coverPtrs.begin()); iter != coverPtrs.end(); iter++)
    ss << "; or\n  " << (* iter)->str(sumProfile);

  return ss.str() + "\n";
}

