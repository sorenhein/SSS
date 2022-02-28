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
  vector<unsigned char>& residuals)
{
  coverPtrs.push_back(&cover);

  // additions are disjoint from tricks.
  for (unsigned i = 0; i < additions.size(); i++)
  {
    tricks[i] += additions[i];
    residuals[i] -= additions[i];
  }

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


unsigned char CoverRow::getComplexity() const
{
  return complexity;
}


string CoverRow::strHeader() const
{
  return 
    coverPtrs.front()->strHeaderTricksShort() +
    coverPtrs.front()->strHeader();
}


// TODO What if these are not available?  How much less clear
// do the outputs become?
string CoverRow::strLines(
  const unsigned char lengthActual,
  const vector<unsigned char>& topsActual) const
{
  stringstream ss;

  for (auto& cptr: coverPtrs)
    ss << 
      cptr->strTricksShort() << 
      cptr->strLine(lengthActual, topsActual);

  return ss.str();
}
