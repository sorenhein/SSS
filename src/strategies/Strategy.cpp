/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Strategy.h"
#include "result/Ranges.h"

#include "../plays/Play.h"
#include "../Survivor.h"



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

bool Strategy::cumulateCommon(
  const Strategy& strat2,
  const bool earlyStopFlag,
  CumulateType methodPtr,
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
    const CompareDetail c = ((* iter1).*methodPtr)(* iter2);
    if (earlyStopFlag && (c & WIN_FIRST_PRIMARY))
      return false;

    cumul |= c;
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
  return Strategy::cumulateCommon(strat2, earlyStopFlag,
    &Result::compareInDetail, cumul);
}


bool Strategy::cumulatePrimary(
  const Strategy& strat2,
  const bool earlyStopFlag,
  unsigned& cumul) const
{
  return Strategy::cumulateCommon(strat2, earlyStopFlag,
    &Result::comparePrimaryInDetail, cumul);
}


void Strategy::cumulateSecondary(
  const Strategy& strat2,
  unsigned& cumul) const
{
  Strategy::cumulateCommon(strat2, false,
    &Result::compareSecondaryInDetail, cumul);
}


bool Strategy::operator == (const Strategy& strat2) const
{
  // For diagnostics.  It is effectively also a complete and
  // basic method, but it carries the conventional name.

  const unsigned n = results.size();
  assert(strat2.results.size() == n);

  if (weightInt != strat2.weightInt)
    return false;

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


CompareType Strategy::compareCompleteBasic(const Strategy& strat2) const
{
  // This is mainly for diagnostics.
  assert(strat2.results.size() == results.size());

  unsigned cumul;
  Strategy::cumulate(strat2, false, cumul);

  return compressCompareDetail(cumul);
}


Compare Strategy::compareSecondary(const Strategy& strat2) const
{
  // As we ignore tricks, this is in a sense basic.
  assert(strat2.results.size() == results.size());

  unsigned cumul;
  Strategy::cumulateSecondary(strat2, cumul);

  return compressCompareSecondaryDetail(cumul);
}


bool Strategy::lessEqualCompleteStudied(const Strategy& strat2) const
{
  if (! study.maybeLessEqualStudied(strat2.study))
    return false;
  else
    return Strategy::lessEqualCompleteBasic(strat2);
}


bool Strategy::lessEqualPrimaryStudied(const Strategy& strat2) const
{
  // This uses studied results if possible, otherwise the basic method.
  if (! study.maybeLessEqualStudied(strat2.study))
    return false;
  else
  {
    unsigned cumul;
    return Strategy::cumulatePrimary(strat2, true, cumul);
  }
}


CompareType Strategy::compareCompleteStudied(
  const Strategy& strat2) const
{
  // The first method only goes as far as the studied results allows.
  // Unlike in the other methods:
  //                  >=   ==   <=   !=
  // WIN_EQUAL        poss poss poss poss
  // WIN_FIRST        poss           poss
  // WIN_SECOND                 poss poss
  // WIN_DIFFERENT                   poss

  const CompareType c = study.comparePartialPrimaryStudied(strat2.study);
  if (c == WIN_FIRST)
  {
    if (strat2.lessEqualCompleteBasic(* this))
      return WIN_FIRST;
    else
      return WIN_DIFFERENT;
  }
  else if (c == WIN_SECOND)
  {
    if (Strategy::lessEqualCompleteBasic(strat2))
      return WIN_SECOND;
    else
      return WIN_DIFFERENT;
  }
  else if (c == WIN_EQUAL)
    return Strategy::compareCompleteBasic(strat2);
  else
    return WIN_DIFFERENT;

}


bool Strategy::lessEqualPrimaryScrutinized(const Strategy& strat2) const
{
  // The caller must ensure that the strategies have been scrutinized.
  return study.lessEqualScrutinized(strat2.study);
}


Compare Strategy::comparePrimaryScrutinized(const Strategy& strat2) const
{
  return study.comparePrimaryScrutinized(strat2.study);
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

void Strategy::initRanges(Ranges& ranges) const
{
  // TODO "false" should mean no winners, but currently ignored.
  ranges.init(results, true);
}


void Strategy::extendRanges(Ranges& ranges) const
{
  ranges.extend(results);
}


/************************************************************
 *                                                          *
 * adapt and helper methods (including study)               *
 *                                                          *
 ************************************************************/

void Strategy::updateSingle(
  const Play& play,
  const unsigned char fullNo)
{
  auto& result = results.front();
  result.update(play, fullNo);
  weightInt = result.tricks();
}


void Strategy::updateSameLength(
  const Play& play,
  const Survivors& survivors)
{
  // for (auto& res: results)
    // res.update(play);

  auto iter1 = results.begin();
  auto iter2 = survivors.distNumbers.begin();

  while (iter1 != results.end())
  {
    iter1->update(play, iter2->fullNo);
    weightInt += play.trickNS;
    iter1++;
    iter2++;
  }
}


void Strategy::updateAndGrow(
  const Play& play,
  const Survivors& survivors)
{
  // Make an indexable vector copy of the results that need to grow.
  vector<Result> resultsOld;
  resultsOld.resize(results.size());

  unsigned r = 0;
  for (auto& res: results)
  {
    // The distribution will get overwritten below
    res.update(play, 0);
    resultsOld[r++] = res;
  }

  // Overwrite the old results list.
  results.resize(survivors.distNumbers.size());
  auto iterSurvivors = survivors.distNumbers.begin();
  weightInt = 0;

  for (auto& res: results)
  {
    // Use the survivor's full distribution number and the 
    // corresponding result entry as the trick count.
    res = resultsOld[iterSurvivors->reducedNo];
    res.setDist(iterSurvivors->fullNo);

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

    Strategy::updateSingle(play, survivors.distNumbers.front().fullNo);
  }
  else if (eastVoidFlag)
  {
    // Only keep the last result.
    if (len1 > 1)
      results.erase(results.begin(), prev(results.end()));

    Strategy::updateSingle(play, survivors.distNumbers.front().fullNo);
  }
  else if (survivors.sizeFull() == len1)
  {
    // No rank reduction.
    Strategy::updateSameLength(play, survivors);
  }
  else
  {
    // This is the general case.  It takes ~55%.
    Strategy::updateAndGrow(play, survivors);
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

  Strategy::adaptResults(play, survivors);

  study.study(results);
}


void Strategy::addComponentwise(const Strategy& strat2)
{
  // This method does not have any usual interpretation in terms
  // of defense (*=) or declarer play (+=).  It is used in order
  // to pick the component-wise maximm (+=) in the case where at
  // least one defender is void, so declarer can pick the best result
  // (and there is only one result, actually).

  assert(results.size() == strat2.results.size()); 

  auto iter1 = results.begin();
  auto iter2 = strat2.results.begin();

  weightInt = 0;
  while (iter1 != results.end())
  {
    * iter1 += * iter2;
    weightInt += iter1->tricks();
    iter1++;
    iter2++;
  }
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

