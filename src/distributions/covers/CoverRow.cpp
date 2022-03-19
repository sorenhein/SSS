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


CoverRow::CoverRow()
{
  CoverRow::reset();
}


void CoverRow::reset()
{
  coverPtrs.clear();
  tricks.clear();
  numDist = 0;
  complexity = 0;
}


void CoverRow::resize(const unsigned len)
{
  tricks.resize(len);
}


bool CoverRow::attempt(
  const CoverNew& cover,
  const vector<unsigned char>& residuals,
  vector<unsigned char>& additions,
  unsigned char& tricksAdded) const
{
  assert(tricks.size() == residuals.size());
  assert(additions.size() == residuals.size());

  // This is just an optimization.  If a cover has already been
  // OR'ed onto the row, it won't help to OR it again.
  if (! coverPtrs.empty() && coverPtrs.back() == &cover)
    return false;

  return cover.possible(tricks, residuals, additions, tricksAdded);
}


void CoverRow::add(
  const CoverNew& cover,
  const vector<unsigned char>& additions,
  vector<unsigned char>& residuals,
  unsigned char& residualsSum)
{
  coverPtrs.push_back(&cover);

  // additions are disjoint from tricks.
  for (unsigned i = 0; i < additions.size(); i++)
  {
    const unsigned char t = additions[i];
    tricks[i] += t;
    residuals[i] -= t;
    residualsSum -= t;
    numDist += t;
  }

  // TODO Check range
  assert(complexity + cover.getComplexity() > complexity);

  complexity += cover.getComplexity();
}


unsigned CoverRow::size() const
{
  return coverPtrs.size();
}


const vector<unsigned char>& CoverRow::getTricks() const
{
  return tricks;
}


unsigned char CoverRow::getWeight() const
{
  return numDist;
}


unsigned char CoverRow::getComplexity() const
{
  return complexity;
}


unsigned char CoverRow::getOverlap() const
{
  // The overlap is the sum of the individual cover weights,
  // minus the weight of the row.
  unsigned char overlap = 0;
  for (auto coverPtr: coverPtrs)
    overlap += coverPtr->getNumDist();

  return overlap - numDist;
}


string CoverRow::strHeader() const
{
  return 
    coverPtrs.front()->strHeaderTricksShort() +
    coverPtrs.front()->strHeader();
}


string CoverRow::strLines(const ProductProfile& sumProfile) const
{
  stringstream ss;

  ss << "* " << coverPtrs.front()->str(sumProfile);

  for (auto iter = next(coverPtrs.begin()); iter != coverPtrs.end(); iter++)
    ss << "; or\n  " << (* iter)->str(sumProfile);

  return ss.str() + "\n";
}


string CoverRow::str(const ProductProfile& sumProfile) const
{
  return 
    CoverRow::strLines(sumProfile);
}

