/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "CoverNew.h"
#include "Profile.h"


CoverNew::CoverNew()
{
  CoverNew::reset();
}


void CoverNew::reset()
{
  productPtr = nullptr;
  tricks.clear();
  weight = 0;
  numDist = 0;
}


void CoverNew::set(
  ProductMemory& productMemory,
  const Profile& sumProfile,
  const ProfilePair& profilePair)
{
  productPtr = productMemory.enterOrLookup(sumProfile, profilePair);

  // We throw away a lot of covers, so it is a bit of a waste
  // to calculate this now.  But it is convenient.
  simplestOpponent = productPtr->simplestOpponent(sumProfile);
}


void CoverNew::prepare(
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  assert(productPtr != nullptr);
  tricks.prepare(* productPtr, distProfiles, cases, weight, numDist);
}


bool CoverNew::possible(
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


CoverState CoverNew::explain(Tricks& tricksSeen) const
{
  return tricks.explain(tricksSeen);
}


bool CoverNew::earlier(const CoverNew& cover2) const
{
  // TODO Some of the methods called do real work, so we could cache
  // their results.

  assert(productPtr != nullptr);
  assert(cover2.productPtr != nullptr);
  const Product& p1 = * productPtr;
  const Product& p2 = * cover2.productPtr;

  if (weight > cover2.weight)
    // Heavier ones first
    return true;
  else if (weight < cover2.weight)
    return false;
  else if (p1.getTopSize() < p2.getTopSize())
    // Simpler ones first
    return true;
  else if (p1.getTopSize() > p2.getTopSize())
    return false;
  else if (p1.getComplexity() < p2.getComplexity())
    // Simpler ones first
    return true;
  else if (p1.getComplexity() > p2.getComplexity())
    return false;
  else if (numDist > cover2.numDist)
    // Ones that touch more distributions first
    return true;
  else if (numDist < cover2.numDist)
    return false;
  else if (p1.getRangeSum() <= p2.getRangeSum())
    /// Narrower covers
    return true;
  else
    return false;
}


bool CoverNew::sameWeight(const CoverNew& cover2) const
{
  return (weight == cover2.weight);
}


bool CoverNew::sameTricks(const CoverNew& cover2) const
{
  return (tricks == cover2.tricks);
}


bool CoverNew::empty() const
{
  return (weight == 0);
}


bool CoverNew::full() const
{
  return (weight > 0 && numDist == tricks.size());
}


unsigned CoverNew::getWeight() const
{
  return weight;
}


unsigned CoverNew::size() const
{
  return tricks.size();
}


unsigned char CoverNew::getNumDist() const
{
  return numDist;
}


unsigned char CoverNew::getTopSize() const
{
  assert(productPtr != nullptr);
  return productPtr->getTopSize();
}


unsigned char CoverNew::getComplexity() const
{
  assert(productPtr != nullptr);
  return productPtr->getComplexity();
}


string CoverNew::strHeader() const
{
  assert(productPtr != nullptr);
  stringstream ss;

  ss << productPtr->strHeader() <<
    setw(8) << "Weight" <<
    setw(8) << "Cmplx" <<
    setw(8) << "Dists" <<
    setw(8) << "Tops" << "\n";

  return ss.str();
}


string CoverNew::strLine(const Profile& sumProfile) const
{
  assert(productPtr != nullptr);
  stringstream ss;

  ss << productPtr->strLine(sumProfile) <<
    setw(8) << weight <<
    setw(8) << +productPtr->getComplexity() <<
    setw(8) << +numDist <<
    setw(8) << +CoverNew::getTopSize() << "\n";
  
  return ss.str();
}


string CoverNew::strLine() const
{
  assert(productPtr != nullptr);
  stringstream ss;

  ss << productPtr->strLine() <<
    setw(8) << weight <<
    setw(8) << +productPtr->getComplexity() <<
    setw(8) << +numDist <<
    setw(8) << +CoverNew::getTopSize() << "\n";
  
  return ss.str();
}


string CoverNew::strProfile() const
{
  stringstream ss;

  ss << "weight " << weight << "\n";
  ss << tricks.strList();

  return ss.str();
}


string CoverNew::strHeaderTricksShort() const
{
  stringstream ss;
  ss << setw(tricks.size()+2) << left << "Tricks";
  return ss.str();
}


string CoverNew::strTricksShort() const
{
  return tricks.strShort();
}


string CoverNew::str(const Profile& sumProfile) const
{
  assert(productPtr != nullptr);
  if (productPtr->explainable())
  {
    stringstream ss;

    // TODO This is where symmFlag would enter
    ss << productPtr->strVerbal(sumProfile, simplestOpponent, false) <<
      " [" << +numDist << ", " << 
      weight << "]";

    return ss.str();
  }
  else
    return CoverNew::strTricksShort() + CoverNew::strLine();
}

