#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Tvector.h"

// TODO Dominance can probably be implemented more efficiently.
// For example the "weight" (sum of all trick counts) indicates
// whether one vector can dominate another or not.  This can be
// extended to halves or thirds etc. of the vectors.  So if we
// can track these statistics efficiently, we can do a comparison
// of these statistics before doing the full loop.


Tvector::Tvector()
{
  Tvector::reset();
}


Tvector::~Tvector()
{
}


void Tvector::reset()
{
  results.clear();
}


void Tvector::logTrivial(
  const unsigned value,
  const unsigned len)
{
  results.clear();
  for (unsigned i = 0; i < len; i++)
  {
    results.emplace_back(TrickEntry());
    TrickEntry& te = results.back();
    te.dist = i;
    te.tricks = value;
  }
  weightInt = value * len;
}


void Tvector::log(
  const vector<unsigned>& distributions,
  const vector<unsigned>& tricks)
{
  assert(distributions.size() == tricks.size());

  weightInt = 0;
  for (unsigned i = 0; i < distributions.size(); i++)
  {
    results.emplace_back(TrickEntry());
    TrickEntry& te = results.back();
    te.dist = distributions[i];
    te.tricks = tricks[i];
    weightInt += tricks[i];
  }
}


bool Tvector::operator == (const Tvector& tv2) const
{
  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<TrickEntry>::const_iterator iter1 = results.cbegin();
  list<TrickEntry>::const_iterator iter2 = tv2.results.cbegin();

  while (iter1 != results.end())
  {
    if (* iter1 != * iter2)
      return false;

    iter1++;
    iter2++;
  }
  return true;
}


bool Tvector::operator >= (const Tvector& tv2) const
{
  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<TrickEntry>::const_iterator iter1 = results.cbegin();
  list<TrickEntry>::const_iterator iter2 = tv2.results.cbegin();

  while (iter1 != results.end())
  {
    if (* iter1 < * iter2)
      return false;

    iter1++;
    iter2++;
  }
  return true;
}


bool Tvector::operator > (const Tvector& tv2) const
{
  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<TrickEntry>::const_iterator iter1 = results.cbegin();
  list<TrickEntry>::const_iterator iter2 = tv2.results.cbegin();

  bool greaterFlag = false;
  while (iter1 != results.end())
  {
    if (* iter1 < * iter2)
      return false;
    else if (* iter1 > * iter2)
      greaterFlag = true;

    iter1++;
    iter2++;
  }
  return greaterFlag;
}


Compare Tvector::compare(const Tvector& tv2) const
{
  // Returns COMPARE_LESS_THAN if *this < tv2.

  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<TrickEntry>::const_iterator iter1 = results.cbegin();
  list<TrickEntry>::const_iterator iter2 = tv2.results.cbegin();

  bool possibleLT = true;
  bool possibleGT = true;

  while (iter1 != results.end())
  {
    if (* iter1 > * iter2)
      possibleLT = false;
    else if (* iter1 < * iter2)
      possibleGT = false;
    
    if (! possibleLT && ! possibleGT)
      return COMPARE_INCOMMENSURATE;

    iter1++;
    iter2++;
  }

  if (possibleLT)
    return (possibleGT ? COMPARE_EQUAL : COMPARE_LESS_THAN);
  else
    return COMPARE_GREATER_THAN;
}


void Tvector::operator *=(const Tvector& tv2)
{
  // Here we don't have to have the same length or distributions.
  
  auto iter1 = results.begin();
  auto iter2 = tv2.results.begin();

  while (iter2 != tv2.results.end())
  {
    if (iter1 == results.end() || iter1->dist > iter2->dist)
    {
      results.insert(iter1, * iter2);
      weightInt += iter2->tricks;
      iter2++;
    }
    else if (iter1->dist < iter2->dist)
    {
      iter1++;
    }
    else
    {
      if (iter1->tricks > iter2->tricks)
      {
        weightInt += iter2->tricks - iter1->tricks;
        iter1->tricks = iter2->tricks;
      }
      iter1++;
      iter2++;
    }
  }
}


void Tvector::constrict(Tvector& constants) const
{
  // The constants vector is a running collection of TrickEntry
  // elements for those distributions (in a Tvectors) that have
  // constant results.  If the result of this current Tvector
  // differs from constants for a given distribution, that 
  // distribution is removed from constants.  A distribution that
  // is in constants must also be in this Tvector.

  auto iter1 = results.begin();
  auto iter2 = constants.results.begin();

  while (iter2 != constants.results.end())
  {
    while (iter1 != results.end() && iter1->dist < iter2->dist)
      iter1++;

    assert(iter1 != results.end());
    assert(iter1->dist == iter2->dist);

    if (iter1->tricks == iter2->tricks)
      iter2++;
    else
      iter2 = constants.results.erase(iter2);
  }
}


void Tvector::lower(Tvector& minima) const
{
  // This returns the distribution-wise minimum.
  assert(results.size() == minima.size());

  auto iter1 = results.begin();
  auto iter2 = minima.results.begin();

  while (iter2 != minima.results.end())
  {
    assert(iter1->dist == iter2->dist);
    
    if (iter1->tricks < iter2->tricks)
      iter2->tricks = iter1->tricks;
    
    iter1++;
    iter2++;
  }
}


void Tvector::updateSingle(
  const unsigned fullNo,
  const unsigned trickNS)
{
  auto& result = results.front();
  result.dist = fullNo;
  result.tricks += trickNS;
  weightInt = result.tricks;
}


void Tvector::updateSameLength(
  const Survivors& survivors,
  const unsigned trickNS)
{
  auto iter1 = results.begin();
  auto iter2 = survivors.distNumbers.begin();

  // This is just an optimization for speed.
  if (trickNS)
  {
    while (iter1 != results.end())
    {
      iter1->dist = iter2->fullNo;
      iter1->tricks += trickNS; 
      weightInt += trickNS;
      iter1++;
      iter2++;
    }
  }
  else
  {
    while (iter1 != results.end())
    {
      iter1->dist = iter2->fullNo;
      iter1++;
      iter2++;
    }
  }
}


void Tvector::updateAndGrow(
  const Survivors& survivors,
  const unsigned trickNS)
{
  // Make an indexable vector copy of the results that need to grow.
  vector<TrickEntry> resultsOld;
  resultsOld.resize(results.size());

  unsigned r = 0;
  for (auto& res: results)
    resultsOld[r++] = res;

  // Overwrite the old results list.
  results.resize(survivors.distNumbers.size());
  auto iterSurvivors = survivors.distNumbers.begin();
  weightInt = 0;

  for (auto& res: results)
  {
    // Use the survivor's full distribution number and the 
    // corresponding result entry as the trick count.
    res.dist = iterSurvivors->fullNo;
    res.tricks = resultsOld[iterSurvivors->reducedNo].tricks + trickNS;
    weightInt += res.tricks;
    iterSurvivors++;
  }
}


void Tvector::adapt(
  const Survivors& survivors,
  const unsigned trickNS,
  const bool lhoVoidFlag,
  const bool rhoVoidFlag,
  const bool rotateFlag)
{
  // Our Tvector results may stem from a rank-reduced child combination.
  // The survivors may have more entries because they come from the
  // parent combination.
  // Our Tvector may be about to get cross-multiplied onto another
  // parent combination.  So it needs to have the full number of
  // entries, and the results list needs to grow.

  assert(! lhoVoidFlag || ! rhoVoidFlag);

  const unsigned len1 = results.size();
  if (lhoVoidFlag || rhoVoidFlag)
  {
    assert(survivors.sizeFull() == 1);
    assert(len1 >= 1);
  }
  else
  {
    if (len1 != survivors.sizeReduced())
      cout << "results length " << len1 << ", reduced survivors " <<
        survivors.sizeReduced() << endl;
    assert(survivors.sizeReduced() == len1);
  }

  if (rotateFlag)
    results.reverse();

  // LHO and RHO void flags pertain to the this rotation state
  // (parent's frame of reference).

  if (lhoVoidFlag)
  {
    // Only keep the first result.
    if (len1 > 1)
      results.erase(next(results.begin()), results.end());

    Tvector::updateSingle(survivors.distNumbers.front().fullNo, trickNS);
  }
  else if (rhoVoidFlag)
  {
    // Only keep the last result.
    if (len1 > 1)
      results.erase(results.begin(), prev(results.end()));

    Tvector::updateSingle(survivors.distNumbers.front().fullNo, trickNS);
  }
  else if (survivors.sizeFull() == len1)
  {
    // No rank reduction.
    Tvector::updateSameLength(survivors, trickNS);
  }
  else
  {
    // This is the general case.
    // TODO
    // assert(false);
    Tvector::updateAndGrow(survivors, trickNS);
  }
}


unsigned Tvector::size() const
{
  return results.size();
}


unsigned Tvector::weight() const
{
  return weightInt;
}


string Tvector::str(const string& title) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";
  
  ss << 
    setw(4) << left << "Dist" <<
    setw(6) << "Tricks" << "\n";

  for (const auto& te: results)
    ss <<
      setw(4) << te.dist <<
      setw(6) << te.tricks << "\n";

  return ss.str();
}

