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
  profile.clear();
  weight = 0;
  numDist = 0;
}


void CoverNew::resize(const unsigned topNumber)
{
  product.resize(topNumber);
}


/*
void CoverNew::set(
  const Profile& sumProfile,
  const Profile& lowerProfile,
  const Profile& upperProfile)
{
  product.set(sumProfile, lowerProfile, upperProfile);

  // We throw away a lot of covers, so it is a bit of a waste
  // to calculate this now.  But it is convenient.
  simplestOpponent = product.simplestOpponent(sumProfile);
}
*/


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
  const unsigned len = distProfiles.size();
  assert(len == cases.size());
  profile.resize(len);

  for (unsigned dno = 0; dno < len; dno++)
  {
    if (product.includes(distProfiles[dno]))
    {
      profile[dno] = 1;
      weight += static_cast<unsigned>(cases[dno]);
      numDist++;
    }
  }
}


bool CoverNew::possible(
  const vector<unsigned char>& explained,
  const vector<unsigned char>& residuals,
  vector<unsigned char>& additions,
  unsigned char& tricksAdded) const
{
  // explained: The OR'ed vector in CoverRow that is already explained.
  // residuals: The overall tricks in cover tableau that remains.
  // additions: If the cover can be added, the additions to the
  //   explained vector that would arise
  // tricksAdded: The number of tricks in additions

  assert(profile.size() == explained.size());
  assert(profile.size() == residuals.size());
  assert(profile.size() == additions.size());

  tricksAdded = 0;
  for (unsigned i = 0; i < profile.size(); i++)
  {
    // If the cover has an entry that has not already been set:
    if (profile[i] && ! explained[i])
    {
      if (residuals[i])
      {
        // We need that entry.
        additions[i] = 1;
        tricksAdded++;
      }
      else
      {
        // We cannot have that entry.
        return false;
      }
    }
    else
      additions[i] = 0;
  }

  // Could still have been fully contained.
  return (tricksAdded > 0);
}


CoverState CoverNew::explain(vector<unsigned char>& tricks) const
{
  assert(tricks.size() == profile.size());

  CoverState state = COVER_DONE;

  for (unsigned i = 0; i < tricks.size(); i++)
  {
    if (profile[i] > tricks[i])
      return COVER_IMPOSSIBLE;
    else if (profile[i] < tricks[i])
      state = COVER_OPEN;
  }

  for (unsigned i = 0; i < tricks.size(); i++)
    tricks[i] -= profile[i];

  return state;
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
  assert(profile.size() == cover2.profile.size());

  for (unsigned i = 0; i < profile.size(); i++)
    if (profile[i] != cover2.profile[i])
      return false;

  return true;
}


bool CoverNew::empty() const
{
  return (weight == 0);
}


bool CoverNew::full() const
{
  return (weight > 0 && numDist == profile.size());
}


unsigned CoverNew::getWeight() const
{
  return weight;
}


unsigned CoverNew::size() const
{
  return profile.size();
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

  for (unsigned i = 0; i < profile.size(); i++)
    ss << i << ": " << +profile[i] << "\n";
  
  return ss.str();
}


string CoverNew::strHeaderTricksShort() const
{
  stringstream ss;
  ss << setw(profile.size()+2) << left << "Tricks";
  return ss.str();
}


string CoverNew::strTricksShort() const
{
  string s;
  for (unsigned i = 0; i < profile.size(); i++)
    s += (profile[i] ? "1" : "-");
  return s + "  ";
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

