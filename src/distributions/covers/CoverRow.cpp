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
#include "CoverCategory.h"

#include "../../utils/table.h"

CoverRow::CoverRow()
{
  CoverRow::reset();
}


void CoverRow::reset()
{
  coverPtrs.clear();
  tricks.clear();
  weight = 0;
  rawWeight = 0;
  complexity = 0;
}


void CoverRow::resize(const size_t len)
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
    rawWeight += coverPtr->getWeight();
  }

  weight = tricks.getWeight();
  assert(weight != 0);
}


bool CoverRow::possibleAdd(
  const Cover& cover,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned& rawWeightAdder) const
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

  return cover.possible(tricks, residuals, cases, 
    additions, rawWeightAdder);
}


bool CoverRow::possible(const Tricks& residuals) const
{
  return (tricks <= residuals);
}


void CoverRow::add(
  const Cover& cover,
  const Tricks& additions,
  const unsigned rawWeightAdder,
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
  rawWeight += rawWeightAdder;
}


void CoverRow::add(
  const Cover& cover,
  Tricks& residuals)
{
  CoverRow::add(cover, cover.getTricks(), cover.getWeight(), residuals);
}


bool CoverRow::sameTops(const CoverRow& rows2) const
{
  // TODO For now
  if (CoverRow::size() != 1)
    return false;

  if (rows2.size() != 1)
    return false;

  return coverPtrs.front()->sameTops(* rows2.coverPtrs.front());
}


unsigned char CoverRow::minimumByTops(
  const Opponent voidSide,
  const Profile& sumProfile) const
{
  // TODO Extend to general rows.
  if (coverPtrs.size() != 1)
  {
    // Return something that won't be mistaken for being adjacent to
    // a void.
    return (voidSide == OPP_WEST ?
      numeric_limits<unsigned char>::max() :
      numeric_limits<unsigned char>::min());
  }
  else
    return coverPtrs.front()->minimumByTops(voidSide, sumProfile);
}


void CoverRow::updateStats(
  ProductStats& productStats,
  const Profile& sumProfile,
  const bool newTableauFlag) const
{
  for (auto coverPtr: coverPtrs)
    coverPtr->updateStats(productStats, sumProfile, newTableauFlag);
}


size_t CoverRow::size() const
{
  return coverPtrs.size();
}


bool CoverRow::symmetric() const
{
  return (tricks.symmetry() == EXPLAIN_SYMMETRIC);
}


CoverSymmetry CoverRow::symmetry() const
{
  return tricks.symmetry();
}


bool CoverRow::symmetrizable(const Profile& sumProfile) const
{
  // TODO For now, only 1-cover rows.
  if (coverPtrs.size() != 1)
    return false;
  else
    return coverPtrs.front()->symmetrizable(sumProfile);
}


bool CoverRow::operator < (const CoverRow& rows2) const
{
  const unsigned mcpw1 = (complexity << 20) / weight;
  const unsigned mcpw2 = (rows2.complexity << 20) / rows2.weight;

  if (mcpw1 < mcpw2)
    return true;
  else if (mcpw1 > mcpw2)
    return false;
  else if (rows2.tricks <= tricks)
    // Just to make sure we don't alias.
    return false;
  else
    return true;
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


unsigned CoverRow::getRawWeight() const
{
  return rawWeight;
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


string CoverRow::str(
  const Profile& sumProfile,
  const RanksNames& ranksNames) const
{
  stringstream ss;

  ss << "* " << coverPtrs.front()->str(sumProfile, ranksNames);

  for (auto iter = next(coverPtrs.begin()); iter != coverPtrs.end(); iter++)
    ss << "; or\n  " << (* iter)->str(sumProfile, ranksNames);

  return ss.str() + "\n";
}


string CoverRow::strNumerical() const
{
  stringstream ss;

  ss << "* " << coverPtrs.front()->strNumerical();

  for (auto iter = next(coverPtrs.begin()); iter != coverPtrs.end(); iter++)
    ss << "; or\n  " << (* iter)->strNumerical();

  return ss.str() + "\n";
}

