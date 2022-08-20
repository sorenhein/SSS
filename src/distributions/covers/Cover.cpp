/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Cover.h"
#include "CoverCategory.h"

#include "product/ProfilePair.h"
#include "product/ProductStats.h"
#include "product/ProductMemory.h"

#include "../../ranks/RanksNames.h"

#include "../../utils/table.h"


Cover::Cover()
{
  Cover::reset();
}


void Cover::reset()
{
  factoredProductPtr = nullptr;
  tricks.clear();
  mcpw = 0;
  coverSymmetry = EXPLAIN_SYMMETRY_UNSET;
  symmetrizeFlag = false;
  code = 0;
}


void Cover::set(
  ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& profilePair,
  const bool symmetricFlag,
  const bool symmetrizeFlagIn)
{
  // The product may or may not already be in memory.
  // This is used when setting a row algorithmically.
  factoredProductPtr = 
    productMemory.enterOrLookup(sumProfile, profilePair);

  if (symmetricFlag)
    coverSymmetry = EXPLAIN_SYMMETRIC;
  else
    coverSymmetry = EXPLAIN_GENERAL;

  symmetrizeFlag = symmetrizeFlagIn;

  code = profilePair.getCode(sumProfile);
}


void Cover::setExisting(
  const ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& profilePair,
  const bool symmetrizeFlagIn)
{
  // The product must already be in memory.
  // This is used when pre-setting a row manually.
  factoredProductPtr = productMemory.lookupByTop(sumProfile, profilePair);
  code = profilePair.getCode(sumProfile);
  symmetrizeFlag = symmetrizeFlagIn;
}


bool Cover::setByProduct(
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  assert(factoredProductPtr != nullptr);

  if (! tricks.setByProduct(
    * factoredProductPtr,
    symmetrizeFlag,
    distProfiles, 
    cases))
  {
    return false;
  }

  if (coverSymmetry != EXPLAIN_SYMMETRIC)
  {
    if (! symmetrizeFlag && tricks.symmetry() == EXPLAIN_ANTI_SYMMETRIC)
      coverSymmetry = EXPLAIN_ANTI_SYMMETRIC;
    else
      coverSymmetry = EXPLAIN_GENERAL;
  }
  
  mcpw = (Cover::getComplexity() << 20) / tricks.getWeight();
  return true;
}


void Cover::tricksOr(
  Tricks& running,
  const vector<unsigned char>& cases) const
{
  if (symmetrizeFlag)
    running.orSymm(tricks, cases);
  else
    running.orNormal(tricks, cases);
}


bool Cover::possible(
  const Tricks& explained,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned& rawWeightAdder) const
{
  // explained: The OR'ed vector in CoverRow that is already explained.
  // residuals: The overall tricks in cover tableau that remains.
  // additions: If the cover can be added, the additions to the
  //   explained vector that would arise

  rawWeightAdder = tricks.getWeight();
  return tricks.possible(explained, residuals, cases, additions);
}


void Cover::updateStats(
  ProductStats& productStats,
  const Profile& sumProfile,
  const bool newTableauFlag) const
{
  assert(factoredProductPtr != nullptr);
  productStats.store(* factoredProductPtr, sumProfile, newTableauFlag);
}


bool Cover::sameWeight(const Cover& cover2) const
{
  return (tricks.getWeight() == cover2.tricks.getWeight());
}


bool Cover::sameTricks(const Cover& cover2) const
{
  return (tricks == cover2.tricks);
}


bool Cover::sameTops(const Cover& cover2) const
{
  assert(factoredProductPtr != nullptr);
  assert(cover2.factoredProductPtr != nullptr);

  if (symmetrizeFlag != cover2.symmetrizeFlag)
    return false;
  else
    return factoredProductPtr->sameTops(* cover2.factoredProductPtr);
}


unsigned char Cover::minimumByTops(
  const Opponent voidSide,
  const Profile& sumProfile) const
{
  assert(factoredProductPtr != nullptr);
  return factoredProductPtr->minimumByTops(voidSide, sumProfile);
}


bool Cover::symmetrizable(const Profile& sumProfile) const
{
  // We consider the product terms in the order (length, highest top,
  // next top, ...).
  assert(factoredProductPtr != nullptr);
  return factoredProductPtr->symmetrizable(sumProfile);
}


bool Cover::symmetrize()
{
  // Will invalidate Cover if not symmetrizable!
  assert(! symmetrizeFlag);

  if (! tricks.symmetrize())
    return false;

  symmetrizeFlag = true;

  // More weight for the same complexity.
  mcpw >>= 1;
  return true;
}


bool Cover::symmetrized() const
{
  return symmetrizeFlag;
}


bool Cover::symmetric() const
{
  return ((coverSymmetry == EXPLAIN_SYMMETRIC) || symmetrizeFlag);
}


bool Cover::antiSymmetric() const
{
  return ((coverSymmetry == EXPLAIN_ANTI_SYMMETRIC) && ! symmetrizeFlag);
}


CoverSymmetry Cover::symmetry() const
{
  if (symmetrizeFlag)
    return EXPLAIN_SYMMETRIC;
  else
    return coverSymmetry;
}


bool Cover::singular() const
{
  // Either a single distribution, or two symmetrically.
  const size_t numDist = tricks.nonzero();

  if (numDist == 1 || (Cover::symmetric() && numDist == 2))
    return true;
  else
    return false;
}


CoverComposition Cover::composition() const
{
  assert(factoredProductPtr != nullptr);
  return factoredProductPtr->composition();
}


bool Cover::lengthConsistent(const CoverLength& specificLength) const
{
  assert(factoredProductPtr != nullptr);

  if (! specificLength.flag)
    return true;
  else
    return factoredProductPtr->lengthConsistent(specificLength.length);
}


bool Cover::explainable() const
{
  // TODO Potentially again activeCount <= 5 in Product.
  return (Cover::verbal() != VERBAL_GENERAL);
}


size_t Cover::size() const
{
  return tricks.size();
}


bool Cover::operator < (const Cover& cover2) const
{
  if (mcpw < cover2.mcpw)
    return true;
  else if (mcpw > cover2.mcpw)
    return false;

  if (Cover::effectiveDepth() < cover2.effectiveDepth())
    // Simpler ones first
    return true;
  else if (Cover::effectiveDepth() > cover2.effectiveDepth())
    return false;
  else if (symmetrizeFlag && ! cover2.symmetrizeFlag)
    return true;
  else if (! symmetrizeFlag && cover2.symmetrizeFlag)
    return false;
  else
    return (code < cover2.code);
}


const Tricks& Cover::getTricks() const
{
  return tricks;
}


unsigned char Cover::effectiveDepth() const
{
  assert(factoredProductPtr != nullptr);
  return factoredProductPtr->effectiveDepth();
}


unsigned Cover::getWeight() const
{
  return tricks.getWeight();
}


unsigned char Cover::getComplexity() const
{
  assert(factoredProductPtr != nullptr);
  // Aesthetic adjustment: A single holding is quite simple,
  // no matter how many terms it takes to describe it.
  const unsigned char c = factoredProductPtr->getComplexity();
  if (Cover::singular())
    return min(c, static_cast<unsigned char>(3));
  else
    return c;
}


unsigned char Cover::minComplexityAdder(const unsigned resWeight) const
{
  // The covers are ordered by increasing "complexity per weight"
  // (micro-cpw).  We round up the minimum number of covers needed
  // unless we hit an exact divisor.

  return static_cast<unsigned char>(1 + ((resWeight * mcpw - 1) >> 20));
}


bool Cover::discardSymmetric(const Profile& sumProfile) const
{
  // See comment in Product.
  assert(factoredProductPtr != nullptr);
  return (Cover::singular() && 
    factoredProductPtr->discardSymmetric(sumProfile));
}


CoverVerbal Cover::verbal() const
{
  assert(factoredProductPtr != nullptr);

  const CoverVerbal fverbal = factoredProductPtr->verbal();
  if (fverbal == VERBAL_LENGTH_ONLY || ! Cover::singular())
    return fverbal;
  else
    return VERBAL_SINGULAR;
}


string Cover::strHeader() const
{
  assert(factoredProductPtr != nullptr);
  stringstream ss;

  ss << factoredProductPtr->strHeader() <<
    setw(4) << "Wgt" <<
    setw(4) << "Cpx" <<
    setw(10) << "cpw" <<
    setw(4) << "#t" << 
    setw(4) << "Sym" << 
    setw(16) << "Code" << 
    "\n";

  return ss.str();
}


string Cover::strLine() const
{
  assert(factoredProductPtr != nullptr);
  stringstream ss;

  ss << factoredProductPtr->strLine() <<
    setw(4) << tricks.getWeight() <<
    setw(4) << +Cover::getComplexity() <<
    setw(10) << mcpw <<
    setw(4) << +Cover::effectiveDepth() <<
    setw(4) << (symmetrizeFlag ? "sym" : "") << 
    setw(16) << code;
  
  return ss.str();
}


string Cover::strProfile() const
{
  stringstream ss;

  ss << "weight " << tricks.getWeight() << "\n";
  ss << tricks.strList();

  return ss.str();
}


string Cover::strHeaderTricksShort() const
{
  stringstream ss;
  ss << setw(tricks.size()+2) << left << "Tricks";
  return ss.str();
}


string Cover::strTricksShort() const
{
  return tricks.strShort();
}


string Cover::str(
  const Profile& sumProfile,
  const RanksNames& ranksNames,
  const CoverVerbal verbal) const
{
  assert(factoredProductPtr != nullptr);
  assert(ranksNames.used());

  stringstream ss;
  if (verbal == VERBAL_GENERAL || verbal == VERBAL_HEURISTIC)
  {
    ss << Cover::strTricksShort() + Cover::strLine();
  }
  else
  {
    ss << factoredProductPtr->strVerbal(
      sumProfile, ranksNames, verbal, Cover::symmetric());
  }

  ss << " [" << +Cover::getComplexity() << 
    "/" << tricks.getWeight() << "]";

  return ss.str();
}


string Cover::strNumerical() const
{
  assert(factoredProductPtr != nullptr);
  stringstream ss;

  ss << Cover::strTricksShort() + Cover::strLine();
  ss << " [" << +Cover::getComplexity() << 
    "/" << tricks.getWeight() << "]";

  return ss.str();
}

