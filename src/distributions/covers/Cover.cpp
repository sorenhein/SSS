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
  mcpw = 0;
  symmFlag = false;
  code = 0;
}


void Cover::set(
  ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& profilePair,
  const bool symmFlagIn)
{
  // The product may or may not already be in memory.
  // This is used when setting a row algorithmically.
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


bool Cover::prepare(
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  assert(productUnitPtr != nullptr);

  if (! tricks.prepare(
    productUnitPtr->product, 
    symmFlag,
    distProfiles, 
    cases))
  {
    return false;
  }
  
  mcpw = (productUnitPtr->product.getComplexity() << 20) / tricks.getWeight();
  return true;
}


bool Cover::symmetrizable(const Profile& sumProfile) const
{
  // We consider the product terms in the order (length, highest top,
  // next top, ...).
  assert(productUnitPtr != nullptr);
  return productUnitPtr->product.symmetrizable(sumProfile);
}


bool Cover::symmetrize()
{
  // Will invalidate Cover if not symmetrizable!
  assert(! symmFlag);

  if (! tricks.symmetrize())
    return false;

  symmFlag = true;

  // More weight for the same complexity.
  mcpw >>= 1;
  return true;
}


void Cover::tricksOr(
  Tricks& running,
  const vector<unsigned char>& cases) const
{
  if (symmFlag)
    running.orSymm(tricks, cases);
  else
    running.orNormal(tricks, cases);
}


bool Cover::possible(
  const Tricks& explained,
  const Tricks& residuals,
  const vector<unsigned char>& cases,
  Tricks& additions) const
{
  // explained: The OR'ed vector in CoverRow that is already explained.
  // residuals: The overall tricks in cover tableau that remains.
  // additions: If the cover can be added, the additions to the
  //   explained vector that would arise

  return tricks.possible(explained, residuals, cases, additions);
}


bool Cover::possible(const Tricks& residuals) const
{
  return (tricks <= residuals);
}


void Cover::updateStats(
  ProductStats& productStats,
  const Profile& sumProfile,
  const bool newTableauFlag) const
{
  assert(productUnitPtr != nullptr);
  productStats.store(productUnitPtr->product, sumProfile, newTableauFlag);
}


bool Cover::sameWeight(const Cover& cover2) const
{
  return (tricks.getWeight() == cover2.tricks.getWeight());
}


bool Cover::sameTricks(const Cover& cover2) const
{
  return (tricks == cover2.tricks);
}


bool Cover::symmetric() const
{
  return symmFlag;
}


unsigned Cover::size() const
{
  return tricks.size();
}


bool Cover::operator < (const Cover& cover2) const
{
  if (mcpw < cover2.mcpw)
    return true;
  else if (mcpw > cover2.mcpw)
    return false;

  assert(productUnitPtr != nullptr);
  assert(cover2.productUnitPtr != nullptr);
  const Product& p1 = productUnitPtr->product;
  const Product& p2 = cover2.productUnitPtr->product;

  if (p1.effectiveDepth() < p2.effectiveDepth())
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


const Tricks& Cover::getTricks() const
{
  return tricks;
}


unsigned char Cover::effectiveDepth() const
{
  assert(productUnitPtr != nullptr);
  return productUnitPtr->product.effectiveDepth();
}


unsigned Cover::getWeight() const
{
  return tricks.getWeight();
}


unsigned char Cover::getComplexity() const
{
  assert(productUnitPtr != nullptr);
  return productUnitPtr->product.getComplexity();
}


unsigned char Cover::minComplexityAdder(const unsigned resWeight) const
{
  // The covers are ordered by increasing "complexity per weight"
  // (micro-cpw).  We round up the minimum number of covers needed
  // unless we hit an exact divisor.

  return static_cast<unsigned char>(1 + ((resWeight * mcpw - 1) >> 20));
}


string Cover::strHeader() const
{
  assert(productUnitPtr != nullptr);
  stringstream ss;

  ss << productUnitPtr->product.strHeader() <<
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
  assert(productUnitPtr != nullptr);
  stringstream ss;

  ss << productUnitPtr->product.strLine() <<
    setw(4) << tricks.getWeight() <<
    setw(4) << +productUnitPtr->product.getComplexity() <<
    setw(10) << mcpw <<
    setw(4) << +productUnitPtr->product.effectiveDepth() <<
    setw(4) << (symmFlag ? "sym" : "") << 
    setw(16) << code << 
    "\n";
  
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


string Cover::str(const Profile& sumProfile) const
{
  assert(productUnitPtr != nullptr);
  const Product& product = productUnitPtr->product;

  if (product.explainable())
  {
    stringstream ss;

    Opponent simplestOpponent = product.simplestOpponent(sumProfile);
    ss << product.strVerbal(sumProfile, simplestOpponent, symmFlag);
    ss << " [" << tricks.getWeight() << "]";

    return ss.str();
  }
  else
    return Cover::strTricksShort() + Cover::strLine();
}

