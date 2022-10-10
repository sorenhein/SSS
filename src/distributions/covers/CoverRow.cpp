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

#include "../../languages/Dictionary.h"
#include "../../languages/connections/words.h"

#include "../../utils/table.h"

extern Dictionary dictionary;

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
  verbal = VERBAL_GENERAL;
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
  Tricks& residuals,
  const CoverVerbal verbalIn)
{
  if (coverPtrs.empty())
    verbal = verbalIn;
  else
    assert(verbal == verbalIn);

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
  Tricks& residuals,
  const CoverVerbal verbalIn)
{
  CoverRow::add(cover, cover.getTricks(), cover.getWeight(), residuals,
    verbalIn);
}


void CoverRow::setVerbal()
{
  if (coverPtrs.size() != 1)
    verbal = VERBAL_GENERAL;
  else
    verbal = coverPtrs.front()->verbal();
}


bool CoverRow::sameTops(const CoverRow& row2) const
{
  // TODO For now
  if (CoverRow::size() != 1)
    return false;

  if (row2.size() != 1)
    return false;

  return coverPtrs.front()->sameTops(* row2.coverPtrs.front());
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


bool CoverRow::operator < (const CoverRow& row2) const
{
  const unsigned mcpw1 = (complexity << 20) / weight;
  const unsigned mcpw2 = (row2.complexity << 20) / row2.weight;

  if (mcpw1 < mcpw2)
    return true;
  else if (mcpw1 > mcpw2)
    return false;
  else if (row2.tricks <= tricks)
    // Just to make sure we don't alias.
    return false;
  else
    return true;
}


bool CoverRow::lowerVerbal(const CoverRow& row2) const
{
  return (verbal < row2.verbal);
}


const Tricks& CoverRow::getTricks() const
{
  return tricks;
}


unsigned char CoverRow::effectiveDepth() const
{
  return 1;
}


bool CoverRow::lengthConsistent(
  [[maybe_unused]] const CoverLength& specificLength) const
{
  // Need this for manual covers.
  return true;
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


string CoverRow::strEnum(const CoverVerbal verbalIn) const
{
  if (verbalIn == VERBAL_GENERAL)
    return "A";
  else if (verbalIn == VERBAL_HEURISTIC)
    return "B";
  else if (verbalIn == VERBAL_LENGTH_ONLY)
    return "C";
  else if (verbalIn == VERBAL_ONE_TOP_ONLY)
    return "D";
  else if (verbalIn == VERBAL_LENGTH_AND_ONE_TOP)
    return "E";
  else if (verbalIn == VERBAL_HIGH_TOPS_EQUAL)
    return "F";
  else if (verbalIn == VERBAL_ANY_TOPS_EQUAL)
    return "G";
  else if (verbalIn == VERBAL_SINGULAR)
    return "H";
  else
  {
    assert(false);
    return "";
  }
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
  bool adjustFlag = (verbal == VERBAL_GENERAL || verbal == VERBAL_HEURISTIC);
  const string& orWord = dictionary.words.get(WORDS_CONJUNCTION).text;
  CoverVerbal verbalAdj;

  stringstream ss;

  if (adjustFlag)
  {
    verbalAdj = coverPtrs.front()->verbal();

    ss << 
      "*" << 
      CoverRow::strEnum(verbalAdj) <<
      " " << 
      coverPtrs.front()->str(sumProfile, ranksNames, verbalAdj);

    for (auto iter = next(coverPtrs.begin()); iter != coverPtrs.end(); iter++)
      ss << "; " << orWord << "\n  " << 
        (* iter)->str(sumProfile, ranksNames, (* iter)->verbal());
  }
  else
  {
    ss << 
      "*" << 
      CoverRow::strEnum(verbal) <<
      " " << 
      coverPtrs.front()->str(sumProfile, ranksNames, verbal);

    for (auto iter = next(coverPtrs.begin()); iter != coverPtrs.end(); iter++)
      ss << "; " << orWord << "\n  " << 
        (* iter)->str(sumProfile, ranksNames, verbal);
  }

  return ss.str() + "\n";
}


string CoverRow::strNumerical() const
{
  stringstream ss;
  const string& orWord = dictionary.words.get(WORDS_CONJUNCTION).text;

  ss << "* " << coverPtrs.front()->strNumerical();

  for (auto iter = next(coverPtrs.begin()); iter != coverPtrs.end(); iter++)
    ss << "; " << orWord << "\n  " << (* iter)->strNumerical();

  return ss.str() + "\n";
}

