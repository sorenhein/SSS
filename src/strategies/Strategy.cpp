#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Strategy.h"

#include "../plays/Play.h"

#include "../Survivor.h"

// TMP
#include "../stats/Timer.h"
extern vector<Timer> timersStrat;

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

  summary.clear();
  studiedFlag = false;
}


void Strategy::resize(const unsigned len)
{
  results.resize(len);
}


void Strategy::eraseRest(list<Result>::iterator iter)
{
  results.erase(iter, results.end());
}


void Strategy::logTrivial(
  const Result& trivialEntry,
  const unsigned len)
{
  results.clear();
  for (unsigned i = 0; i < len; i++)
  {
    results.emplace_back(Result());
    Result& te = results.back();
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
    results.emplace_back(Result());
    Result& te = results.back();
    te.dist = distributions[i];
    te.tricks = tricks[i];
    weightInt += tricks[i];
  }
}


unsigned Strategy::numGroups() const
{
  if (results.empty())
    return 0;

  // May be 1.
  return static_cast<unsigned>(sqrt(static_cast<float>(results.size())));
}


void Strategy::study(const unsigned groups)
{
  if (groups == 0)
  {
    if (! studiedFlag)
    {
      assert(! studiedFlag);
    }
    return;
  }

  // May already be studied?  Don't redo?

vector<unsigned> copy;
if (studiedFlag)
{
  assert(summary.size() == groups);
  copy = summary;
}

  summary.clear();
  summary.resize(groups);

  unsigned i = 0;
  for (auto& result: results)
  {
    summary[i % groups] += result.tricks;
    i++;
  }
  studiedFlag = true;

if (! copy.empty())
{
  for (i = 0; i < summary.size(); i++)
  {
    if (summary[i] != copy[i])
    {
      assert(false);
    }
  }
}

}


void Strategy::restudy()
{
  const unsigned groups = Strategy::numGroups();

  summary.clear();
  summary.resize(groups);

  unsigned i = 0;
  for (auto& result: results)
  {
    summary[i % groups] += result.tricks;
    i++;
  }
  studiedFlag = true;
}


bool Strategy::operator == (const Strategy& tv2) const
{
// timersStrat[11].start();
  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = tv2.results.cbegin();

  while (iter1 != results.end())
  {
    if (* iter1 != * iter2)
{
// timersStrat[11].stop();
      return false;
}

    iter1++;
    iter2++;
  }
// timersStrat[11].stop();
  return true;
}


bool Strategy::greaterEqual(const Strategy& strat2) const
{
  if (studiedFlag)
  {
    for (unsigned i = 0; i < summary.size(); i++)
    {
      if (summary[i] < strat2.summary[i])
        return false;
    }
  }
  else
  {
    // At least 2 groups?
    assert(results.size() < 4);
  }
  
  // Do the full comparison.
  return (* this >= strat2);
}


bool Strategy::operator >= (const Strategy& tv2) const
{
// timersStrat[12].start();
  assert(tv2.results.size() == results.size());

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = tv2.results.cbegin();

  while (iter1 != results.end())
  {
    if (* iter1 < * iter2)
{
// timersStrat[12].stop();
      return false;
}

    iter1++;
    iter2++;
  }
// timersStrat[12].stop();
  return true;
}


bool Strategy::operator > (const Strategy& tv2) const
{
// timersStrat[13].start();
  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = tv2.results.cbegin();

  bool greaterFlag = false;
  while (iter1 != results.end())
  {
    if (* iter1 < * iter2)
    {
// timersStrat[13].stop();
      return false;
    }
    else if (* iter1 > * iter2)
      greaterFlag = true;

    iter1++;
    iter2++;
  }
// timersStrat[13].stop();
  return greaterFlag;
}


Compare Strategy::compare(const Strategy& tv2) const
{
  // Returns COMPARE_LESS_THAN if *this < tv2.

// timersStrat[14].start();
  const unsigned n = results.size();
  assert(tv2.results.size() == n);

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = tv2.results.cbegin();

  bool possibleLT = true;
  bool possibleGT = true;

  while (iter1 != results.end())
  {
    if (* iter1 > * iter2)
      possibleLT = false;
    else if (* iter1 < * iter2)
      possibleGT = false;
    
    if (! possibleLT && ! possibleGT)
    {
// timersStrat[14].stop();
      return COMPARE_INCOMMENSURATE;
    }

    iter1++;
    iter2++;
  }

  if (possibleLT)
  {
// timersStrat[14].stop();
    return (possibleGT ? COMPARE_EQUAL : COMPARE_LESS_THAN);
  }
  else
  {
// timersStrat[14].stop();
    return COMPARE_GREATER_THAN;
  }
}


void Strategy::operator *=(const Strategy& tv2)
{
  // Here we don't have to have the same length or distributions.
  
// timersStrat[15].start();
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

  Strategy::restudy();

// timersStrat[15].stop();
}


void Strategy::initRanges(Ranges& ranges)
{
// timersStrat[16].start();
  ranges.resize(results.size());
  auto iter = results.begin();
  auto riter = ranges.begin();

  for (; iter != results.end(); iter++, riter++)
  {
    riter->dist = iter->dist;
    riter->lower = iter->tricks;
    riter->upper = iter->tricks;
    riter->minimum = iter->tricks;
  }
// timersStrat[16].stop();
}


void Strategy::extendRanges(Ranges& ranges)
{
  assert(results.size() == ranges.size());

  auto iter = results.begin();
  auto riter = ranges.begin();

  for (; iter != results.end(); iter++, riter++)
  {
    assert(iter->dist == riter->dist);
    if (iter->tricks < riter->lower)
    {
      riter->lower = iter->tricks;
      riter->minimum = iter->tricks;
    }
    if (iter->tricks > riter->upper)
      riter->upper = iter->tricks;
  }
}


void Strategy::purgeRanges(
  Ranges& ranges,
  const Ranges& parentRanges)
{
  // Removes those distributions from results for which the range
  // is strictly worse than the lowest parent range (i.e. the best
  // range for the defense).
  // There may be more entries in parentRanges than in ranges,
  // but not the other way round.  There are the same number of
  // results and ranges.

  assert(results.size() == ranges.size());

  auto iterResults = results.begin();
  auto iterRanges = ranges.begin();
  auto iterParentRanges = parentRanges.begin();

  while (iterResults != results.end() &&
      iterParentRanges != parentRanges.end())
  {
    assert(iterResults->dist == iterRanges->dist);

    while (iterParentRanges != parentRanges.end() &&
        iterParentRanges->dist < iterResults->dist)
      iterParentRanges++;

    assert(iterParentRanges->dist == iterResults->dist);

    if (* iterParentRanges < * iterRanges)
    {
      weightInt -= iterResults->tricks;
      iterResults = results.erase(iterResults);
      iterRanges = ranges.erase(iterRanges);
      iterParentRanges++;
    }
  }
}


void Strategy::erase(list<Result>::iterator iter)
{
  // No error checking.
  weightInt -= iter->tricks;
  results.erase(iter);
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
  vector<Result> resultsOld;
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
// timersStrat[17].start();
  // Our Strategy results may stem from a rank-reduced child combination.
  // The survivors may have more entries because they come from the
  // parent combination.
  // Our Strategy may be about to get cross-multiplied onto another
  // parent combination.  So it needs to have the full number of
  // entries, and the results list needs to grow.

  bool westVoidFlag, eastVoidFlag;
  play.setVoidFlags(westVoidFlag, eastVoidFlag);
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
    for (auto& res: results)
      res.winners.flip();
  }

  // Update the winners.
    for (auto& res: results)
      res.winners.update(play);

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

  Strategy::restudy();
// timersStrat[17].stop();
}


unsigned Strategy::size() const
{
  return results.size();
}


unsigned Strategy::weight() const
{
  return weightInt;
}


void Strategy::checkWeights() const
{
  unsigned w = 0;
  for (auto& res: results)
    w += res.tricks;

  assert(w == weightInt);
}


string Strategy::str(const string& title) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";
  
  ss << 
    setw(4) << left << "Dist" <<
    setw(6) << "Tricks" << "\n";

  for (auto& res: results)
    ss <<
      setw(4) << res.dist <<
      setw(6) << res.tricks << "\n";

  return ss.str();
}


bool Strategy::isStudiedTMP() const
{
  return studiedFlag;
}


unsigned Strategy::studyParameter() const
{
  return summary.size();
}

