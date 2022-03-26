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
  tricks.clear();
  weight = 0;
  numDist = 0;
}


void CoverNew::resize(const unsigned topNumber)
{
  product.resize(topNumber);
}


void CoverNew::set(
  const Profile& sumProfile,
  const ProfilePair& profilePair)
{
  product.set(sumProfile, profilePair);

  // We throw away a lot of covers, so it is a bit of a waste
  // to calculate this now.  But it is convenient.
  simplestOpponent = product.simplestOpponent(sumProfile);
}


void CoverNew::prepare(
  const vector<Profile>& distProfiles,
  const vector<unsigned char>& cases)
{
  tricks.prepare(product, distProfiles, cases, weight, numDist);
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

  if (weight > cover2.weight)
    // Heavier ones first
    return true;
  else if (weight < cover2.weight)
    return false;
  else if (product.getTopSize() < cover2.product.getTopSize())
    // Simpler ones first
    return true;
  else if (product.getTopSize() > cover2.product.getTopSize())
    return false;
  else if (product.getComplexity() < cover2.product.getComplexity())
    // Simpler ones first
    return true;
  else if (product.getComplexity() > cover2.product.getComplexity())
    return false;
  else if (numDist > cover2.numDist)
    // Ones that touch more distributions first
    return true;
  else if (numDist < cover2.numDist)
    return false;
  else if (product.getRangeSum() <= cover2.product.getRangeSum())
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
  return product.getTopSize();
}


unsigned char CoverNew::getComplexity() const
{
  return product.getComplexity();
}


string CoverNew::strHeader() const
{
  stringstream ss;

  ss << product.strHeader() <<
    setw(8) << "Weight" <<
    setw(8) << "Cmplx" <<
    setw(8) << "Dists" <<
    setw(8) << "Tops" << "\n";

  return ss.str();
}


string CoverNew::strLine(const Profile& sumProfile) const
{
  stringstream ss;

  ss << product.strLine(sumProfile) <<
    setw(8) << weight <<
    setw(8) << +product.getComplexity() <<
    setw(8) << +numDist <<
    setw(8) << +CoverNew::getTopSize() << "\n";
  
  return ss.str();
}


string CoverNew::strLine() const
{
  stringstream ss;

  ss << product.strLine() <<
    setw(8) << weight <<
    setw(8) << +product.getComplexity() <<
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
  if (product.explainable())
  {
    stringstream ss;

    // TODO This is where symmFlag would enter
    ss << product.strVerbal(sumProfile, simplestOpponent, false) <<
      " [" << +numDist << ", " << 
      weight << "]";

    return ss.str();
  }
  else
    return CoverNew::strTricksShort() + CoverNew::strLine();
}

