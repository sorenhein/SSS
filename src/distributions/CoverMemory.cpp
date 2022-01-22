/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <cassert>

#include "CoverMemory.h"


CoverMemory::CoverMemory()
{
  CoverMemory::reset();
}


void CoverMemory::reset()
{
  specs.clear();
}


CoverSpec& CoverMemory::add(
  const unsigned char cards,
  const unsigned char tops1)
{
  assert(cards < specs.size());
  assert(tops1 < specs[cards].size());
  specs[cards][tops1].emplace_back(CoverSpec());
  return specs[cards][tops1].back();
}



void CoverMemory::prepare(const unsigned char maxCards)
{
  specs.resize(maxCards+1);
  for (unsigned char c = 0; c <= maxCards; c++)
    specs[c].resize(2);

  CoverSpec& spec1 = CoverMemory::add(7, 1);
  spec1.mode = COVER_LENGTHS_AND_TOPS;
  spec1.westLength.setValues(4, COVER_GREATER_EQUAL);
  spec1.westTop1.set(0, COVER_EQUAL);

  CoverSpec& spec2 = CoverMemory::add(7, 1);
  spec2.mode = COVER_LENGTHS_AND_TOPS;
  spec2.westLength.setValues(3, COVER_LESS_EQUAL);
  spec2.westTop1.set(1, COVER_EQUAL);

  CoverSpec& spec3 = CoverMemory::add(7, 1);
  spec3.mode = COVER_LENGTHS_ONLY;
  spec3.westLength.setValues(3, COVER_LESS_EQUAL);

  CoverSpec& spec4 = CoverMemory::add(7, 1);
  spec4.mode = COVER_TOPS_ONLY;
  spec4.westTop1.set(1, COVER_EQUAL);

  CoverSpec& spec5 = CoverMemory::add(7, 1);
  spec5.mode = COVER_LENGTHS_AND_TOPS;
  spec5.westLength.setValues(6, COVER_EQUAL);
  spec5.westTop1.set(0, COVER_EQUAL);
}


list<CoverSpec>::const_iterator CoverMemory::begin(
  const unsigned cards,
  const unsigned tops1) const
{
  assert(cards < specs.size());
  assert(tops1 < specs[cards].size());

  return specs[cards][tops1].begin();
}


list<CoverSpec>::const_iterator CoverMemory::end(
  const unsigned cards,
  const unsigned tops1) const
{
  assert(cards < specs.size());
  assert(tops1 < specs[cards].size());

  return specs[cards][tops1].end();
}


string CoverMemory::str(
  const unsigned cards,
  const unsigned tops1) const
{
  string s = "";

  for (auto iter = CoverMemory::begin(cards, tops1); 
      iter != CoverMemory::end(cards, tops1); iter++)
  {
    s += iter->strLength() + + " (oper) " + iter->strTop1() + "\n";
  }
    
  return s; 
}

