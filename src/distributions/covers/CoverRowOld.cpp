/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverRowOld.h"
#include "Cover.h"


CoverRowOld::CoverRowOld()
{
  CoverRowOld::reset();
}


void CoverRowOld::reset()
{
  coverPtrs.clear();
  tricks.clear();
  weight = 0;
  numDist = 0;
  complexity = 0;
}


void CoverRowOld::resize(const unsigned len)
{
  tricks.resize(len);
}


void CoverRowOld::fillDirectly(
  list<Cover const *>& coverPtrsIn,
  const vector<unsigned char>& cases,
  const unsigned indexIn)
{
  // This method is used for manually set cover rows only.
  // There is no need to check against trick vectors.
  coverPtrs = coverPtrsIn;
  indexInternal = indexIn;

  for (auto coverPtr: coverPtrs)
    coverPtr->tricksOr(tricks);
  
  tricks.weigh(cases, weight, numDist);
  assert(weight != 0);
}


CoverState CoverRowOld::explain(Tricks& tricksSeen) const
{
  // If tricks <= tricksSeen elementwise, tricks is subtracted out.
  return tricks.explain(tricksSeen);
}


bool CoverRowOld::operator <= (const CoverRowOld& cover2) const
{
  return (tricks <= cover2.tricks);
}


unsigned CoverRowOld::size() const
{
  return coverPtrs.size();
}


unsigned CoverRowOld::index() const
{
  return indexInternal;
}


const Tricks& CoverRowOld::getTricks() const
{
  return tricks;
}


unsigned CoverRowOld::getWeight() const
{
  return weight;
}


unsigned char CoverRowOld::getNumDist() const
{
  return numDist;
}


unsigned CoverRowOld::getComplexity() const
{
  return complexity;
}


unsigned char CoverRowOld::getOverlap() const
{
  // The overlap is the sum of the individual cover counts,
  // minus the count of the row.
  unsigned char overlap = 0;
  for (auto coverPtr: coverPtrs)
    overlap += coverPtr->getNumDist();

  return overlap - numDist;
}


string CoverRowOld::strInternal() const
{
  stringstream ss;

  ss <<
    "weight " << +weight <<
    ", dists " << +numDist <<
    ", complexity " << +complexity << "\n";

  ss << tricks.strList();

  return ss.str();
}


string CoverRowOld::strHeader() const
{
  return
    coverPtrs.front()->strHeaderTricksShort() +
    coverPtrs.front()->strHeader();
}


string CoverRowOld::str(const Profile& sumProfile) const
{
  string s = coverPtrs.front()->str(sumProfile);

  for (auto iter = next(coverPtrs.begin()); iter != coverPtrs.end(); iter++)
    s += "; or\n  " + (* iter)->str(sumProfile);

  return s;
}
