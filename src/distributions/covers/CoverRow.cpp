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
  numDist = 0;
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
  // This method is used for manually set cover rows only.
  // There is no need to check against trick vectors.
  coverPtrs = coverPtrsIn;

  for (auto coverPtr: coverPtrs)
    coverPtr->tricksOr(tricks);

  tricks.weigh(cases, weight, numDist);
  assert(weight != 0);
}


bool CoverRow::attempt(
  const Cover& cover,
  const Tricks& residuals,
  Tricks& additions,
  unsigned char& tricksAdded) const
{
  // Do not update any internal states.  Try to add the cover with
  // its residuals and see what this would add incrementally to the row.
  // This is used in the exhaustive search for the least complex covers.

  assert(tricks.size() == residuals.size());
  assert(additions.size() == residuals.size());

  // This is just an optimization.  If a cover has already been
  // OR'ed onto the row, it won't help to OR it again.
  if (! coverPtrs.empty() && coverPtrs.back() == &cover)
    return false;

  return cover.possible(tricks, residuals, additions, tricksAdded);
}


void CoverRow::add(
  const Cover& cover,
  const Tricks& additions,
  Tricks& residuals,
  unsigned char& residualsSum)
{
  coverPtrs.push_back(&cover);

  tricks.add(additions, residuals, residualsSum, numDist);

  // TODO Keep checking until we're sure we don't get overflow.
  assert(complexity + cover.getComplexity() > complexity);

  complexity += cover.getComplexity();
}


bool CoverRow::possible(
  const Tricks& explained,
  const Tricks& residuals,
  Tricks& additions,
  unsigned char& tricksAdded) const
{
  // explained: The OR'ed vector in CoverRow that is already explained.
  // residuals: The overall tricks in cover tableau that remains.
  // additions: If the cover can be added, the additions to the
  //   explained vector that would arise
  // tricksAdded: The number of tricks in additions

  return tricks.possible(explained, residuals, additions, tricksAdded);
}


CoverState CoverRow::explain(Tricks& tricksSeen) const
{
  // If tricks <= tricksSeen elementwise, tricks is subtracted out.
  return tricks.explain(tricksSeen);
}


void CoverRow::updateStats(
  ProductStats& productStats,
  const Profile& sumProfile,
  const bool newTableauFlag) const
{
  for (auto coverPtr: coverPtrs)
    coverPtr->updateStats(productStats, sumProfile, newTableauFlag);
}


bool CoverRow::operator <= (const CoverRow& coverRow2) const
{
  return (tricks <= coverRow2.tricks);
}


unsigned CoverRow::size() const
{
  return coverPtrs.size();
}


const Tricks& CoverRow::getTricks() const
{
  return tricks;
}


unsigned CoverRow::getWeight() const
{
  return weight;
}


unsigned char CoverRow::getNumDist() const
{
  return numDist;
}


unsigned char CoverRow::getComplexity() const
{
  return complexity;
}


unsigned char CoverRow::getOverlap() const
{
  // The overlap is the sum of the individual cover counts,
  // minus the count of the row.
  unsigned char overlap = 0;
  for (auto coverPtr: coverPtrs)
    overlap += coverPtr->getNumDist();

  return overlap - numDist;
}


string CoverRow::strInternal() const
{
  stringstream ss;

  ss << 
    "weight " << +weight <<
    ", dists " << +numDist <<
    ", complexity " << +complexity << "\n";

  ss << tricks.strList();

  return ss.str();
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

