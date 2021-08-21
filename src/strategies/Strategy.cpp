/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
// #include <math.h>
#include <cassert>

#include "Strategy.h"
#include "result/Ranges.h"

#include "../plays/Play.h"
#include "../Survivor.h"

// TMP
#include "../utils/Timer.h"
extern vector<Timer> timersStrat;


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
  study.reset();
}


void Strategy::resize(const unsigned len)
{
  results.resize(len);
}


unsigned Strategy::size() const
{
  return results.size();
}


bool Strategy::empty() const
{
  return results.empty();
}


unsigned Strategy::weight() const
{
  return weightInt;
}


void Strategy::push_back(const Result& result)
{
  results.push_back(result);
  weightInt += result.tricks();
  study.unstudy();
}


list<Result>::iterator Strategy::erase(list<Result>::iterator& iter)
{
  // No error checking.
  weightInt -= iter->tricks();
  study.unstudy();
  return results.erase(iter);
}


void Strategy::eraseRest(list<Result>::iterator iter)
{
  study.unstudy();
  results.erase(iter, results.end());
}


void Strategy::logTrivial(
  const Result& trivial,
  const unsigned char len)
{
  results.clear();
  study.unstudy();

  for (unsigned char i = 0; i < len; i++)
  {
    results.emplace_back(trivial);
    results.back().setDist(i);
  }
  weightInt = trivial.tricks() * len;
}


void Strategy::restudy()
{
  study.study(results);
}


void Strategy::scrutinize(const Ranges& ranges)
{
  study.scrutinize(results, ranges);
}


/************************************************************
 *                                                          *
 * Comparison methods                                       *
 *                                                          *
 ************************************************************/

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


bool Strategy::cumulate(
  const Strategy& strat2,
  const bool earlyStopFlag,
  unsigned& cumul) const
{
  // This method supports others that perform complete comparisons.
  // It returns true if the trick comparison still permits <=,
  // even though the secondary comparison perhaps would not.
  // If true, cumul is the cumulative bit vector of returns for
  // further processing.  If true, the flag causes the method to
  // return (false) as soon as <= is no longer possible, and the
  // cumulator will then not be complete.

  assert(strat2.results.size() == results.size());

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = strat2.results.cbegin();

  cumul = WIN_NEUTRAL_OVERALL;
  while (iter1 != results.end())
  {
    const CompareDetail c = iter1->compareInDetail(* iter2);
    if (earlyStopFlag && (c & WIN_FIRST_PRIMARY))
      return false;

    cumul |= c;
    iter1++;
    iter2++;
  }
  return true;

}


bool Strategy::greaterEqualCumulator(
  const Strategy& strat2,
  unsigned& cumul) const
{
  // This method supports others that perform complete comparisons.
  // This one returns true if the trick comparison still permits >=,
  // and if so, cumul is the cumulative bit vector of returns for
  // further processing.
  assert(strat2.results.size() == results.size());

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = strat2.results.cbegin();

  cumul = WIN_NEUTRAL_OVERALL;
  while (iter1 != results.end())
  {
    const CompareDetail c = iter1->compareInDetail(* iter2);
    if (c & WIN_SECOND_PRIMARY)
      return false;

    cumul |= c;
    iter1++;
    iter2++;
  }
  return true;
}


bool Strategy::lessEqualCompleteBasic(const Strategy& strat2) const
{
  // This is complete in the sense that it includes both the primary
  // and secondary aspects of the Result's.  It is basic in the sense
  // that it does not rely on studied or scrutinizes Result's.

  unsigned cumul;
  if (! Strategy::cumulate(strat2, true, cumul))
    return false;
  
  if (cumul & WIN_SECOND_PRIMARY)
    return true;
  else if (cumul & WIN_FIRST_SECONDARY)
    return false;
  else if (cumul & WIN_DIFFERENT_SECONDARY)
    return false;
  else
    return true;
}


bool Strategy::operator >= (const Strategy& strat2) const
{
  // This uses studied results if possible, otherwise the basic method.
  // if (! study.maybeGreaterEqual(strat2.study))
  if (! strat2.study.maybeLessEqualStudied(study))
    return false;
  else
    return strat2.lessEqualCompleteBasic(* this);
    // return Strategy::greaterEqual(strat2);
}


unsigned Strategy::makeCumulator(const Strategy& strat2) const
{
  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = strat2.results.cbegin();

  unsigned cumul = WIN_NEUTRAL_OVERALL;
  while (iter1 != results.end())
  {
    cumul |= iter1->compareInDetail(* iter2);
    iter1++;
    iter2++;
  }

  return cumul;
}


CompareType Strategy::compare(const Strategy& strat2) const
{
  // This is only for diagnostics.
  assert(strat2.results.size() == results.size());

  unsigned cumul = Strategy::makeCumulator(strat2);

  // Can this go in a ComparerDetail class, or somewhere else?
  // Or even in a table lookup (64)?

  if (cumul & WIN_DIFFERENT_PRIMARY)
    return WIN_DIFFERENT;

  if (cumul & WIN_FIRST_PRIMARY)
  {
    if (cumul & WIN_SECOND_PRIMARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (cumul & WIN_SECOND_PRIMARY)
    return WIN_SECOND;

  if (cumul & WIN_DIFFERENT_SECONDARY)
    return WIN_DIFFERENT;

  if (cumul & WIN_FIRST_SECONDARY)
  {
    if (cumul & WIN_SECOND_SECONDARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (cumul & WIN_SECOND_SECONDARY)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


CompareDetail Strategy::compareDetail(const Strategy& strat2) const
{
  // This is only for diagnostics.
  assert(strat2.results.size() == results.size());

  unsigned cumul = Strategy::makeCumulator(strat2);

  // Can this go in a ComparerDetail class, or somewhere else?
  // Or even in a table lookup (64)?

  if (cumul & WIN_DIFFERENT_PRIMARY)
    return WIN_DIFFERENT_PRIMARY;

  if (cumul & WIN_FIRST_PRIMARY)
  {
    if (cumul & WIN_SECOND_PRIMARY)
      return WIN_DIFFERENT_PRIMARY;
    else
      return WIN_FIRST_PRIMARY;
  }
  else if (cumul & WIN_SECOND_PRIMARY)
    return WIN_SECOND_PRIMARY;

  if (cumul & WIN_DIFFERENT_SECONDARY)
    return WIN_DIFFERENT_SECONDARY;

  if (cumul & WIN_FIRST_SECONDARY)
  {
    if (cumul & WIN_SECOND_SECONDARY)
      return WIN_DIFFERENT_SECONDARY;
    else
      return WIN_FIRST_SECONDARY;
  }
  else if (cumul & WIN_SECOND_SECONDARY)
    return WIN_SECOND_SECONDARY;
  else
    return WIN_EQUAL_OVERALL;
}


bool Strategy::greaterEqualByProfile(const Strategy& strat2) const
{
  // return study.greaterEqualByProfile(strat2.study);
  return strat2.study.lessEqualScrutinized(study);
}


bool Strategy::greaterEqualByStudy(const Strategy& strat2) const
{
  // This uses studied results if possible, otherwise the basic method.
  // if (! study.maybeGreaterEqual(strat2.study))
  if (! strat2.study.maybeLessEqualStudied(study))
    return false;
  else
    return Strategy::greaterEqualByTricks(strat2);
}


bool Strategy::greaterEqualByTricks(const Strategy& strat2) const
{
  // The ByProfile method is preferable if the profile is available.
  unsigned cumul;
  return Strategy::greaterEqualCumulator(strat2, cumul);
}


///// ----------------  The new comparators? ------------

bool Strategy::lessEqualPrimaryScrutinized(const Strategy& strat2) const
{
  // The caller must ensure that the strategies have been scrutinized.
  return study.lessEqualScrutinized(strat2.study);
}


Compare Strategy::comparePrimaryScrutinized(const Strategy& strat2) const
{
  return study.comparePrimaryScrutinized(strat2.study);
}


Compare Strategy::compareSecondary(const Strategy& strat2) const
{
  assert(strat2.results.size() == results.size());

  unsigned cumul = Strategy::makeCumulator(strat2);

  // Can this go in a ComparerDetail class, or somewhere else?
  // Or even in a table lookup (64)?

  if ((cumul & WIN_FIRST_PRIMARY) ||
      (cumul & WIN_SECOND_PRIMARY) ||
      (cumul & WIN_DIFFERENT_PRIMARY))
  {
    assert(false);
  }

  if (cumul & WIN_DIFFERENT_SECONDARY)
    return WIN_DIFFERENT;

  if (cumul & WIN_FIRST_SECONDARY)
  {
    if (cumul & WIN_SECOND_SECONDARY)
      return WIN_DIFFERENT;
    else
      return WIN_FIRST;
  }
  else if (cumul & WIN_SECOND_SECONDARY)
    return WIN_SECOND;
  else
    return WIN_EQUAL;
}


/************************************************************
 *                                                          *
 * *= and multiply methods (including study)                *
 *                                                          *
 ************************************************************/

void Strategy::operator *= (const Strategy& strat2)
{
  // Here we don't have to have the same length or distributions.
  auto iter1 = results.begin();
  auto iter2 = strat2.results.begin();

  while (iter2 != strat2.results.end())
  {
    if (iter1 == results.end() || iter1->dist() > iter2->dist())
    {
      results.insert(iter1, * iter2);
      weightInt += iter2->tricks();
      iter2++;
    }
    else if (iter1->dist() < iter2->dist())
    {
      iter1++;
    }
    else 
    {
      if (iter1->tricks() > iter2->tricks())
        weightInt += static_cast<unsigned>
          (iter2->tricks() - iter1->tricks());
      * iter1 *= * iter2;

      iter1++;
      iter2++;
    }
  }

  study.study(results);
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
          weightInt += it->tricks();
      }
      break;
    }
    else if (iter2 == strat2.results.end())
    {
      results.insert(results.end(), iter1, strat1.results.end());
      for (auto it = iter1; it != strat1.results.end(); it++)
        weightInt += it->tricks();
      break;
    }

    if (iter1->dist() < iter2->dist())
    {
      results.push_back(* iter1);
      weightInt += iter1->tricks();
      iter1++;
    }
    else if (iter1->dist() > iter2->dist())
    {
      results.push_back(* iter2);
      weightInt += iter2->tricks();
      iter2++;
    }
    else if (iter1->tricks() < iter2->tricks())
    {
      // Take the one with the lower number of tricks.
      results.push_back(* iter1);
      weightInt += iter1->tricks();
      iter1++;
      iter2++;
    }
    else if (iter1->tricks() > iter2->tricks())
    {
      results.push_back(* iter2);
      weightInt += iter2->tricks();
      iter1++;
      iter2++;
    }
    else
    {
      // Opponents can choose among the two winners.
      results.push_back(* iter1);
      results.back() *= * iter2;
      weightInt += iter1->tricks();
      iter1++;
      iter2++;
    }
  }

  study.study(results);
}


/************************************************************
 *                                                          *
 * Range methods                                            *
 *                                                          *
 ************************************************************/

void Strategy::initRanges(Ranges& ranges)
{
  // TODO "false" should mean no winners, but currently ignored.
  ranges.init(results, true);
}


void Strategy::extendRanges(Ranges& ranges)
{
  ranges.extend(results);
}


/************************************************************
 *                                                          *
 * adapt and helper methods (including study)               *
 *                                                          *
 ************************************************************/

void Strategy::updateSingle(
  const unsigned char fullNo,
  const unsigned char trickNS)
{
  auto& result = results.front();
  result.update(fullNo, trickNS);
  weightInt = result.tricks();
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
      iter1->update(iter2->fullNo, trickNS);
      weightInt += trickNS;
      iter1++;
      iter2++;
    }
  }
  else
  {
    while (iter1 != results.end())
    {
      iter1->setDist(iter2->fullNo);
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
    res = resultsOld[iterSurvivors->reducedNo];
    res.update(iterSurvivors->fullNo, trickNS);

    weightInt += res.tricks();
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
      res.flip();
  }

  // Update the winners.  This takes about 12% of the method time.
  // TODO This should become part of adaptResults and should then
  // make it more general: Put the winners update into Result.
  for (auto& res: results)
    res.update(play);

  Strategy::adaptResults(play, survivors);

  study.study(results);
}


/************************************************************
 *                                                          *
 * Winners                                                  *
 *                                                          *
 ************************************************************/

const Result Strategy::resultLowest() const
{
  Result resLowest;
  for (auto& res: results)
    res.multiplyWinnersOnto(resLowest);

  return resLowest;
}


/************************************************************
 *                                                          *
 * string methods                                           *
 *                                                          *
 ************************************************************/

string Strategy::str(
  const string& title,
  const bool rankFlag) const
{
  if (results.empty())
    return "";

  stringstream ss;
  ss << results.front().strHeader(title);

  for (auto& res: results)
    ss << res.str(rankFlag);

  return ss.str();
}

