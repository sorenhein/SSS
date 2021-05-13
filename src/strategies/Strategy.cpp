#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Strategy.h"

#include "../plays/Play.h"

// TODO Dominance can probably be implemented more efficiently.
// For example the "weight" (sum of all trick counts) indicates
// whether one vector can dominate another or not.  This can be
// extended to halves or thirds etc. of the vectors.  So if we
// can track these statistics efficiently, we can do a comparison
// of these statistics before doing the full loop.


Strategy::Strategy()
{
  Strategy::reset();
}


Strategy::~Strategy()
{
}


void Strategy::reset()
{
  results.clear();
  weightInt = 0;
}


void Strategy::logTrivial(
  const TrickEntry& trivialEntry,
  const unsigned len)
{
  results.clear();
  for (unsigned i = 0; i < len; i++)
  {
    results.emplace_back(TrickEntry());
    TrickEntry& te = results.back();
    te.dist = i;
    te.tricks = trivialEntry.tricks;
    te.winners = trivialEntry.winners;
  }
  weightInt = trivialEntry.tricks * len;
}


void Strategy::log(
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


bool Strategy::operator == (const Strategy& tv2) const
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


bool Strategy::operator >= (const Strategy& tv2) const
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


bool Strategy::operator > (const Strategy& tv2) const
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


Compare Strategy::compare(const Strategy& tv2) const
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


void Strategy::operator *=(const Strategy& tv2)
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
        // Take the one with the lower number of tricks.
        weightInt += iter2->tricks - iter1->tricks;
        iter1->tricks = iter2->tricks;
        iter1->winners = iter2->winners;
      }
      else if (iter1->tricks == iter2->tricks)
      {
        // Opponents can choose among the two winners.
        iter1->winners *= iter2->winners;
      }
      iter1++;
      iter2++;
    }
  }
}


void Strategy::bound(
  Strategy& constants,
  Strategy& lower,
  Strategy& upper) const
{
  // Each of the three vectors is a running collection of TrickEntry
  // elements summarizing the distributions (in a parent Strategies).
  // - constants is the constant results.  If tricks deviate,
  //   the corresponding distribution is removed below.
  // - lower is the lowest trick value for the distribution.
  //   There is always an entry for each distribution.
  // - upper is the highest trick value for the distribution.

  auto iter = results.begin();
  auto iterConst = constants.results.begin();
  auto iterLower = lower.results.begin();
  auto iterUpper = upper.results.begin();

  while (iter != results.end())
  {
    if (iter->tricks < iterLower->tricks)
    {
      assert(iter->dist == iterLower->dist);
      iterLower->tricks = iter->tricks;
    }
    else if (iter->tricks > iterUpper->tricks)
    {
      assert(iter->dist == iterUpper->dist);
      iterUpper->tricks = iter->tricks;
    }

    if (iter->dist == iterConst->dist)
    {
      if (iter->tricks == iterConst->tricks)
        iterConst++;
      else
      {
        // Only constants has a vector "meaning", so we only keep 
        // track of its weight.
        constants.weightInt -= iterConst->tricks;
        iterConst = constants.results.erase(iterConst);
      }
    }
    else
    {
      if (iter->dist >= iterConst->dist)
      {
        cout << "HERE\n";
        cout << Strategy::str("Strategy");
        cout << constants.str("constants");
        cout << lower.str("lower");
        cout << upper.str("upper");
        cout << "iter " << iter->dist << ": " << iter->tricks << endl;
        cout << "iterConst " << iterConst->dist << ": " << iterConst->tricks << endl;
        cout << "iterLower " << iterLower->dist << ": " << iterLower->tricks << endl;
        cout << "iterUpper " << iterUpper->dist << ": " << iterUpper->tricks << endl;
      }
      assert(iter->dist < iterConst->dist);
    }
    
    iter++;
    iterLower++;
    iterUpper++;
  }
}


void Strategy::constrain(Strategy& constants) const
{
  // The constants vector is a running collection of TrickEntry
  // elements for those distributions (in a Strategies) that have
  // constant results.  If the result of this current Strategy
  // differs from constants for a given distribution, that 
  // distribution is removed from constants.
  //
  // If there are distributions in constants that are not in this,
  // they are removed.
  // 
  // In the case where this Strategy contains the minima, it
  // is component-wise <= constants.  Therefore there is no harm
  // in comparing the tricks >= rather than == below.
  //
  // This behavior is useful when limiting non-constant plays.

  auto iter1 = results.begin();
  auto iter2 = constants.results.begin();
  constants.weightInt = 0;

  while (iter2 != constants.results.end())
  {
    while (iter1 != results.end() && iter1->dist < iter2->dist)
      iter1++;

    if (iter1 == results.end())
    {
      iter2 = constants.results.erase(iter2, constants.results.end());
      return;
    }

    if (iter1->dist == iter2->dist)
    {
      if (iter1->tricks >= iter2->tricks)
      {
        constants.weightInt += iter2->tricks;
        iter2++;
      }
      else
        iter2 = constants.results.erase(iter2);
    }
    else
    {
      while (iter2 != constants.results.end() && 
          iter1->dist > iter2->dist)
        iter2 = constants.results.erase(iter2);
    }
  }
}


unsigned Strategy::purge(const Strategy& constants)
{
  // Removes results corresponding to all distributions in
  // constants.  The distributions don't all have to be present
  // in results, but those that are will be removed.
  // Returns number of purges.

  auto iter1 = results.begin();
  auto iter2 = constants.results.begin();
  unsigned num = 0;

  while (iter2 != constants.results.end())
  {
    while (iter1 != results.end() && iter1->dist < iter2->dist)
      iter1++;

    if (iter1 == results.end())
      return num;

    if (iter1->dist == iter2->dist)
    {
      assert(iter1->tricks >= iter2->tricks);
      weightInt -= iter1->tricks;
      iter1 = results.erase(iter1);
      iter2++;
      num++;
    }
    else
    {
      while (iter2 != constants.results.end() && 
          iter1->dist > iter2->dist)
        iter2++;
    }
  }
  return num;
}


void Strategy::updateSingle(
  const unsigned fullNo,
  const unsigned trickNS)
{
  auto& result = results.front();
  result.dist = fullNo;
  result.tricks += trickNS;
  weightInt = result.tricks;
}


void Strategy::updateSameLength(
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


void Strategy::updateAndGrow(
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
    res.winners = resultsOld[iterSurvivors->reducedNo].winners;
    weightInt += res.tricks;
    iterSurvivors++;
  }
}


void Strategy::adapt(
  const Play& play,
  const Survivors& survivors)
{
  // Our Strategy results may stem from a rank-reduced child combination.
  // The survivors may have more entries because they come from the
  // parent combination.
  // Our Strategy may be about to get cross-multiplied onto another
  // parent combination.  So it needs to have the full number of
  // entries, and the results list needs to grow.

  bool westVoidFlag, eastVoidFlag;
  if (play.side == POSITION_NORTH)
  {
    westVoidFlag = play.rhoPtr->isVoid();
    eastVoidFlag = play.lhoPtr->isVoid();
  }
  else
  {
    westVoidFlag = play.lhoPtr->isVoid();
    eastVoidFlag = play.rhoPtr->isVoid();
  }

  assert(! westVoidFlag || ! eastVoidFlag);

  const unsigned len1 = results.size();
  if (westVoidFlag || eastVoidFlag)
  {
    assert(survivors.sizeFull() == 1);
    assert(len1 >= 1);
  }
  else
    assert(survivors.sizeReduced() == len1);

  if (play.rotateFlag)
  {
    results.reverse();

    // We also have to to fix the NS winner orientation.
    for (auto& te: results)
      te.winners.flip();
  }

  // Update the winners.
    for (auto& te: results)
      te.winners.update(play);

  if (westVoidFlag)
  {
    // Only keep the first result.
    if (len1 > 1)
      results.erase(next(results.begin()), results.end());

    Strategy::updateSingle(survivors.distNumbers.front().fullNo, 
      play.trickNS);
  }
  else if (eastVoidFlag)
  {
    // Only keep the last result.
    if (len1 > 1)
      results.erase(results.begin(), prev(results.end()));

    Strategy::updateSingle(survivors.distNumbers.front().fullNo, 
      play.trickNS);
  }
  else if (survivors.sizeFull() == len1)
  {
    // No rank reduction.
    Strategy::updateSameLength(survivors, play.trickNS);
  }
  else
  {
    // This is the general case.
    Strategy::updateAndGrow(survivors, play.trickNS);
  }
}


unsigned Strategy::size() const
{
  return results.size();
}


unsigned Strategy::weight() const
{
  return weightInt;
}


string Strategy::str(const string& title) const
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

