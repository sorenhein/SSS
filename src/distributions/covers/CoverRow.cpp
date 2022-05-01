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
    coverPtr->tricksOr(tricks, cases);
    assert(complexity + coverPtr->getComplexity() > complexity);
    complexity += coverPtr->getComplexity();
  }

  weight = tricks.getWeight();
  assert(weight != 0);
}


bool CoverRow::possibleAdd(
  const Cover& cover,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions) const
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

  return cover.possible(tricks, residuals, cases, additions);
}


bool CoverRow::possible(
  const Tricks& residuals,
  Tricks& additions) const
{
  // residuals: The overall tricks in cover tableau that remains.
  // additions: If the cover can be added, the additions to the
  //   explained vector that would arise
  // weightAdded: The number of cases in additions

  if (tricks <= residuals)
  {
    // TODO If we put more into CoverTableau, do we need the copy?
    // Or copy in caller?
    additions = tricks;
    return true;
  }
  else
    return false;
}


void CoverRow::add(
  const Cover& cover,
  const Tricks& additions,
  Tricks& residuals)
{
  coverPtrs.push_back(&cover);

  // additions are disjoint from tricks here.
  tricks += additions;
  residuals -= additions;

  // TODO Keep checking until we're sure we don't get overflow.
  assert(complexity + cover.getComplexity() > complexity);

  complexity += cover.getComplexity();
  weight += additions.getWeight();
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


bool CoverRow::operator <= (const Tricks& residuals) const
{
  // TODO Move CoverRow::possible and Cover::possible to something
  // like this.  Then CoverTableau::attemptRow in .h would have to do
  // something more.
  return (tricks <= residuals);
}


const Tricks& CoverRow::getTricks() const
{
  return tricks;
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
  const unsigned resWeight) const
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

