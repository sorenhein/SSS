#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <math.h>
#include <cassert>

#include "Strategy.h"

#include "../plays/Play.h"

#include "../Survivor.h"


// A major time drain is the component-wise comparison of results.  
// There is an experimental implementation in which 5 result entries
// are gathered into a 10-bit vector, and two such 10-bit vectors
// are compared in a 20-bit lookup.  The lookup table must be global
// and initialized once.

#define LOOKUP_GROUP 5
#define LOOKUP_BITS (LOOKUP_GROUP + LOOKUP_GROUP)
#define LOOKUP_SIZE (LOOKUP_BITS + LOOKUP_BITS)

mutex mtxStrategy;
static bool init_flag = false;
vector<unsigned char> lookupGE;


// TMP
#include "../stats/Timer.h"
extern vector<Timer> timersStrat;


Strategy::Strategy()
{
  mtxStrategy.lock();
  if (! init_flag)
  {
    Strategy::setConstants();
    init_flag = true;
  }
  mtxStrategy.unlock();

  Strategy::reset();
}


Strategy::~Strategy()
{
}


void Strategy::setConstants()
{
  // In order to speed up the comparison of Strategy's, we group
  // their trick excesses (over the minimum) into a profile list,
  // where each entry combines five entries into 10 bits.
  // We can then look up two 10-bit profiles and get a partial answer.
  
  lookupGE.resize(1 << LOOKUP_SIZE);
  for (unsigned i = 0; i < (1 << LOOKUP_BITS); i++)
  {
    for (unsigned j = 0; j < (1 << LOOKUP_BITS); j++)
    {
      unsigned flagGE = 1;
      unsigned i0 = i;
      unsigned j0 = j;
      for (unsigned p = 0; p < LOOKUP_GROUP; p++)
      {
        // Break each index down into a two-bit number.
        const unsigned entry1 = (i0 & 0x3);
        const unsigned entry2 = (j0 & 0x3);
        if (entry1 < entry2)
        {
          // Can no longer be >=.
          flagGE = 0;
          break;
        }
        else
        {
          i0 >>= 2;
          j0 >>= 2;
        }
      }

      if (flagGE)
        lookupGE[(i << LOOKUP_BITS) | j] = 1;
    }
  }
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


void Strategy::logTrivial(
  const Result& trivialEntry,
  const unsigned char len)
{
  results.clear();
  for (unsigned char i = 0; i < len; i++)
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
  const vector<unsigned char>& distributions,
  const vector<unsigned char>& tricks)
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


void Strategy::study()
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


void Strategy::scrutinize(const Ranges& minima)
{
  if (minima.size() < results.size())
  {
  cout << "minima " << minima.size() << " vs. " << results.size() << endl;
  cout << Strategy::str();
  assert(minima.size() >= results.size());
  }
  profiles.clear();

  auto riter = results.begin();
  auto miter = minima.begin();

  unsigned counter = 0;
  unsigned profile = 0;
  while (riter != results.end())
  {
    assert(miter != minima.end());
    if (miter->dist < riter->dist)
    {
      miter++;
      continue;
    }

    assert(riter->dist == miter->dist);

    const unsigned diff = riter->tricks - miter->minimum;
    assert(diff < 4); // Must fit in 2 bits for this to work

    profile = (profile << 2) | diff;
    counter++;

    if (counter == LOOKUP_GROUP - 1)
    {
      profiles.push_back(profile);
      counter = 0;
      profile = 0;
    }

    riter++;
    miter++;
  }

  if (counter > 0)
    profiles.push_back(profile);
}


bool Strategy::operator == (const Strategy& strat2) const
{
  // For diagnostics.
  const unsigned n = results.size();
  assert(strat2.results.size() == n);

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = strat2.results.cbegin();

  while (iter1 != results.end())
  {
    if (* iter1 != * iter2)
      return false;

    iter1++;
    iter2++;
  }
  return true;
}


bool Strategy::greaterEqual(const Strategy& strat2) const
{
  assert(strat2.results.size() == results.size());

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = strat2.results.cbegin();

  while (iter1 != results.end())
  {
    if (* iter1 < * iter2)
      return false;

    iter1++;
    iter2++;
  }
  return true;
}


bool Strategy::greaterEqualByProfile(const Strategy& strat2) const
{
  assert(strat2.results.size() == results.size());

  if (profiles.size() != strat2.profiles.size())
  {
    cout << "Profile size mismatch\n";
    cout << profiles.size() << " vs. " << strat2.profiles.size() << endl;

  }
  assert(profiles.size() == strat2.profiles.size());
  assert(! profiles.empty());

  auto piter1 = profiles.begin();
  auto piter2 = strat2.profiles.begin();
  while (piter1 != profiles.end())
  {
    if (! lookupGE[((* piter1) << 10) | (* piter2)])
      return false;

    piter1++;
    piter2++;
  }

  return true;
}


bool Strategy::operator >= (const Strategy& strat2) const
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
  return Strategy::greaterEqual(strat2);
}


void Strategy::operator *= (const Strategy& strat2)
{
  // Here we don't have to have the same length or distributions.
  auto iter1 = results.begin();
  auto iter2 = strat2.results.begin();

  while (iter2 != strat2.results.end())
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
        weightInt += static_cast<unsigned>(iter2->tricks - iter1->tricks);
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

  Strategy::study();

// timersStrat[15].stop();
}


void Strategy::multiply(
  const Strategy& strat1,
  const Strategy& strat2)
{
  // Here we don't have to have the same length or distributions.
  Strategy::reset();
  auto iter1 = strat1.results.begin();
  auto iter2 = strat2.results.begin();

  while (true)
  {
    if (iter1 == strat1.results.end())
    {
      if (iter2 != strat2.results.end())
      {
        results.insert(results.end(), iter2, strat2.results.end());
        for (auto it = iter2; it != strat2.results.end(); it++)
          weightInt += it->tricks;
      }
      break;
    }
    else if (iter2 == strat2.results.end())
    {
      results.insert(results.end(), iter1, strat1.results.end());
      for (auto it = iter1; it != strat1.results.end(); it++)
        weightInt += it->tricks;
      break;
    }

    if (iter1->dist < iter2->dist)
    {
      results.push_back(* iter1);
      weightInt += iter1->tricks;
      iter1++;
    }
    else if (iter1->dist > iter2->dist)
    {
      results.push_back(* iter2);
      weightInt += iter2->tricks;
      iter2++;
    }
    else if (iter1->tricks < iter2->tricks)
    {
      // Take the one with the lower number of tricks.
      results.push_back(* iter1);
      weightInt += iter1->tricks;
      iter1++;
      iter2++;
    }
    else if (iter1->tricks > iter2->tricks)
    {
      results.push_back(* iter2);
      weightInt += iter2->tricks;
      iter1++;
      iter2++;
    }
    else
    {
      // Opponents can choose among the two winners.
      results.push_back(* iter1);
      results.back().winners *= iter2->winners;
      weightInt += iter1->tricks;
      iter1++;
      iter2++;
    }
  }

  Strategy::study();
}


void Strategy::initRanges(Ranges& ranges)
{
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


list<Result>::iterator Strategy::erase(list<Result>::iterator& iter)
{
  // No error checking.
  weightInt -= iter->tricks;
  return results.erase(iter);
}


void Strategy::eraseRest(list<Result>::iterator iter)
{
  results.erase(iter, results.end());
}


void Strategy::updateSingle(
  const unsigned char fullNo,
  const unsigned char trickNS)
{
  auto& result = results.front();
  result.dist = fullNo;
  result.tricks += trickNS;
  weightInt = result.tricks;
}


void Strategy::updateSameLength(
  const Survivors& survivors,
  const unsigned char trickNS)
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
  const unsigned char trickNS)
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


void Strategy::adaptResults(
  const Play& play,
  const Survivors& survivors)
{
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

  if (westVoidFlag)
  {
    // Only keep the first result.  This and the next take ~ 33%.
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
    // This is the general case.  It takes ~55%.
    Strategy::updateAndGrow(survivors, play.trickNS);
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
  // Overall this is not such an expensive method.

  if (play.rotateFlag)
  {
    results.reverse();

    // We also have to to fix the NS winner orientation.
    for (auto& res: results)
      res.winners.flip();
  }

  // Update the winners.  This takes about 12% of the method time.
    for (auto& res: results)
      res.winners.update(play);

  Strategy::adaptResults(play, survivors);

  Strategy::study();
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

  for (auto& res: results)
    ss <<
      setw(4) << res.dist <<
      setw(6) << +res.tricks << "\n";

  return ss.str();
}

