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

#include "product/Profile.h"
#include "product/ProfilePair.h"
#include "product/ProductStats.h"


Cover::Cover()
{
  Cover::reset();
}


void Cover::reset()
{
  productUnitPtr = nullptr;
  tricks.clear();
  weight = 0;
  mcpw = 0;
  // TODO Think about no numDist at all
  numDist = 0;
  symmFlag = false;
  code = 0;
}


void Cover::set(
  ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& profilePair,
  const bool symmFlagIn)
{
  productUnitPtr = productMemory.enterOrLookup(sumProfile, profilePair);

  symmFlag = symmFlagIn;

  code = profilePair.getCode(sumProfile);
}


void Cover::setExisting(
  const ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& profilePair,
  const bool symmFlagIn)
{
  // The product must already be in memory.
  // This is used when pre-setting a row manually.
  productUnitPtr = productMemory.lookupByTop(sumProfile, profilePair);
  code = profilePair.getCode(sumProfile);
  symmFlag = symmFlagIn;
}


void Cover::prepare(
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  assert(productUnitPtr != nullptr);

  tricks.prepare(
    productUnitPtr->product, 
    symmFlag,
    distProfiles, 
    cases, 
    weight, 
    numDist);
  
  assert(weight > 0);
  mcpw = (productUnitPtr->product.getComplexity() << 20) / weight;
}


bool Cover::symmetrizable(const Profile& sumProfile) const
{
  // We consider the product terms in the order (length, highest top,
  // next top, ...).  The first such term that is set must be in the
  // lower half of its possibilities.  So if length is 5, it can be
  // at most [0, 2].  If length is 4, [0, 1].
  
  assert(productUnitPtr != nullptr);
  return productUnitPtr->product.symmetrizable(sumProfile);
}


bool Cover::symmetrize(const vector<unsigned char>& cases)
{
  // Will invalidate Cover if not symmetrizable!
  assert(! symmFlag);

  if (! tricks.symmetrize(cases, weight, numDist))
    return false;

  // Should still have at least one zero.
  if (numDist == tricks.size())
    return false;

  symmFlag = true;
  // More weight for the same complexity.
  mcpw >>= 1;
  return true;
}


void Cover::tricksOr(Tricks& running) const
{
  if (symmFlag)
    running.orSymm(tricks);
  else
    running |= tricks;
}


// TODO Move to possible?
bool Cover::includes(const Profile& distProfile) const
{
  assert(productUnitPtr != nullptr);
  return productUnitPtr->product.includes(distProfile);
}


bool Cover::possible(
  const Tricks& explained,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned char& weightAdded) const
{
  // explained: The OR'ed vector in CoverRow that is already explained.
  // residuals: The overall tricks in cover tableau that remains.
  // additions: If the cover can be added, the additions to the
  //   explained vector that would arise
  // tricksAdded: The number of tricks in additions

  return tricks.possible(explained, residuals, cases, 
    additions, weightAdded);
}


bool Cover::possible(
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions,
  unsigned char& weightAdded) const
{
  // Same as the previous method with explained unused.
  return tricks.possibleNew(residuals, cases, additions, weightAdded);
}


CoverState Cover::explain(Tricks& tricksSeen) const
{
  return tricks.explain(tricksSeen);
}


void Cover::updateStats(
  ProductStats& productStats,
  const Profile& sumProfile,
  const bool newTableauFlag) const
{
  assert(productUnitPtr != nullptr);
  productStats.store(productUnitPtr->product, sumProfile, newTableauFlag);
}


bool Cover::operator < (const Cover& cover2) const
{
  // TODO Some of the methods called do real work, so we could cache
  // their results.

  assert(productUnitPtr != nullptr);
  assert(cover2.productUnitPtr != nullptr);
  const Product& p1 = productUnitPtr->product;
  const Product& p2 = cover2.productUnitPtr->product;

  if (mcpw < cover2.mcpw)
    return true;
  else if (mcpw > cover2.mcpw)
    return false;
  else if (p1.effectiveDepth() < p2.effectiveDepth())
    // Simpler ones first
    return true;
  else if (p1.effectiveDepth() > p2.effectiveDepth())
    return false;
  else if (symmFlag && ! cover2.symmFlag)
    return true;
  else if (! symmFlag && cover2.symmFlag)
    return false;
  else
    return (code < cover2.code);
}


bool Cover::sameWeight(const Cover& cover2) const
{
  return (weight == cover2.weight);
}


bool Cover::sameTricks(const Cover& cover2) const
{
  return (tricks == cover2.tricks);
}


bool Cover::empty() const
{
  return (weight == 0);
}


bool Cover::full() const
{
  return (weight > 0 && numDist == tricks.size());
}


bool Cover::symmetric() const
{
  return symmFlag;
}


unsigned Cover::getWeight() const
{
  return weight;
}


unsigned Cover::size() const
{
  return tricks.size();
}


unsigned char Cover::effectiveDepth() const
{
  assert(productUnitPtr != nullptr);
  return productUnitPtr->product.effectiveDepth();
}


unsigned char Cover::getComplexity() const
{
  assert(productUnitPtr != nullptr);
  return productUnitPtr->product.getComplexity();
}


unsigned char Cover::minComplexityAdder(const unsigned char resWeight) const
{
  // The covers are ordered by increasing "complexity per weight"
  // (micro-cpw).  We round up the minimum number of covers needed
  // unless we hit an exact divisor.

  const unsigned char projected =
    static_cast<unsigned char>(1 + ((resWeight * mcpw - 1) >> 20));

  // TODO Should this be true now?
  // assert(Cover::getComplexity() <= projected);
  return max(Cover::getComplexity(), projected);
}


string Cover::strHeader() const
{
  assert(productUnitPtr != nullptr);
  stringstream ss;

  ss << productUnitPtr->product.strHeader() <<
    setw(4) << "Wgt" <<
    setw(4) << "Cpx" <<
    setw(10) << "cpw" <<
    setw(4) << "#d" <<
    setw(4) << "#t" << 
    setw(4) << "Sym" << 
    setw(16) << "Code" << 
    "\n";

  return ss.str();
}


string Cover::strLine() const
{
  assert(productUnitPtr != nullptr);
  stringstream ss;

  ss << productUnitPtr->product.strLine() <<
    setw(4) << weight <<
    setw(4) << +productUnitPtr->product.getComplexity() <<
    setw(10) << mcpw <<
    setw(4) << +numDist <<
    setw(4) << +productUnitPtr->product.effectiveDepth() <<
    setw(4) << (symmFlag ? "sym" : "") << 
    setw(16) << code << 
    "\n";
  
  return ss.str();
}


string Cover::strProfile() const
{
  stringstream ss;

  ss << "weight " << weight << "\n";
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


string Cover::str(const Profile& sumProfile) const
{
  assert(productUnitPtr != nullptr);
  const Product& product = productUnitPtr->product;

  if (product.explainable())
  {
    stringstream ss;

    Opponent simplestOpponent = product.simplestOpponent(sumProfile);
    ss << product.strVerbal(sumProfile, simplestOpponent, symmFlag);
    ss << " [" << weight << "]";

    return ss.str();
  }
  else
    return Cover::strTricksShort() + Cover::strLine();
}

