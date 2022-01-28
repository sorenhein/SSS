/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Strategy.h"
#include "result/Ranges.h"

#include "../plays/Play.h"
#include "../distributions/Distribution.h"
#include "../distributions/covers/Covers.h"
#include "../inputs/Control.h"

#include "../utils/compsec.h"
#include "../const.h"

extern Control control;



Strategy::Strategy()
{
  Strategy::reset();
}


void Strategy::reset()
{
  results.clear();
  weightInt = 0;
  studied.reset();
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


bool Strategy::constantTricks() const
{
  unsigned t = UNSIGNED_NOT_SET;
  bool firstFlag = true;
  for (auto& res: results)
  {
    if (firstFlag)
    {
      t = res.getTricks();
      firstFlag = false;
    }
    else if (t != res.getTricks())
      return false;
  }
  return true;
}


bool Strategy::symmetric() const
{
  auto iter1 = results.begin();
  auto iter2 = results.rbegin();
  while (iter1->getDist() < iter2->getDist())
  {
    if (* iter1 != * iter2)
      return false;
    iter1++;
    iter2++;
  }
  return true;
}


unsigned Strategy::weight() const
{
  return weightInt;
}


void Strategy::push_back(const Result& result)
{
  results.push_back(result);
  weightInt += result.getTricks();
  studied.unstudy();
}


list<Result>::iterator Strategy::erase(list<Result>::iterator& iter)
{
  // No error checking.
  weightInt -= iter->getTricks();
  studied.unstudy();
  return results.erase(iter);
}


list<Result>::const_iterator Strategy::erase(
    list<Result>::const_iterator& iter)
{
  // No error checking.
  weightInt -= iter->getTricks();
  studied.unstudy();
  return results.erase(iter);
}


void Strategy::eraseRest(list<Result>::iterator iter)
{
  studied.unstudy();
  results.erase(iter, results.end());
}


void Strategy::logTrivial(
    const Result& trivial,
    const unsigned char len)
{
  results.clear();
  studied.unstudy();

  for (unsigned char i = 0; i < len; i++)
  {
    results.emplace_back(trivial);
    results.back().setDist(i);
  }
  weightInt = trivial.getTricks() * len;

  Strategy::study();
}


void Strategy::study()
{
  studied.study(results);
}


void Strategy::scrutinize(const Ranges& ranges)
{
  studied.scrutinize(results, ranges);
}


/************************************************************
 *                                                          *
 * Rank-check methods, mostly                               *
 *                                                          *
 ************************************************************/

bool Strategy::detectChangedResults(
  const Reduction& reduction,
  vector<Result>& reducedResultList,
  vector<bool>& changeList) const
{
  // Detects the lowest result within each group.
  // Returns true if anything should change.

  const auto& dist = reduction.full2reducedDist;
  vector<bool> firstList(dist.size(), true);
  bool anyChangeFlag = false;

  for (auto& result: results)
  {
    const unsigned reducedDist = dist[result.getDist()];

    if (firstList[reducedDist] == true)
    {
      // Store the group's Result.
      reducedResultList[reducedDist] = result;
      firstList[reducedDist] = false;
    }
    else if (result != reducedResultList[reducedDist])
    {
      reducedResultList[reducedDist] *= result;
      changeList[reducedDist] = true;
      anyChangeFlag = true;
    }
  }

  return anyChangeFlag;
}


bool Strategy::reduceByResults(const Distribution& dist)
{
  // Forces constant results (tricks or whole results depending on
  // Result, i.e. control.runRankComparisons()) within each
  // reduction group.  Returns true if no change was made.

  const Result resLowest = Strategy::resultLowest();
  const unsigned char rankCritical = resLowest.getRank();

  // TODO The 0 test should no longer be needed?
  if (rankCritical == 0 || rankCritical == UCHAR_NOT_SET)
    return true;

  const auto& reduction = dist.getReduction(rankCritical);
  assert(reduction.full2reducedDist.size() == results.size());

  return Strategy::reduceByResults(reduction);
}


bool Strategy::reduceByResults(const Reduction& reduction)
{
  // Forces constant results (tricks or whole results depending on
  // Result, i.e. control.runRankComparisons()) within each
  // reduction group.  Returns true if no change was made.

  const auto& dist = reduction.full2reducedDist;
  const unsigned dlen = dist.size();
  assert(dlen == results.size());

  vector<Result> reducedResultList(dlen);
  vector<bool> changeList(dlen, false);

  if (! Strategy::detectChangedResults(reduction,
      reducedResultList, changeList))
    return true;

  // Change results in affected groups to the lowest one.
  for (auto& result: results)
  {
    const unsigned char d = result.getDist();
    const unsigned reducedDist = dist[d];
    if (changeList[reducedDist])
    {
      // As the copy also copies the distribution number, set it back.
      const Result& reducedResult = reducedResultList[reducedDist];
      weightInt += reducedResult.getTricks() - result.getTricks();
      result = reducedResult;
      result.setDist(d);
    }
  }

  return false;
}


void Strategy::expand(
  const Reduction& reduction,
  const char rankAdder,
  const bool rotateFlag)
{
  // Modify this strategy (from a minimal strategy) up to a
  // non-minimal one by renumbering and by shifting the winners.
  // The distribution map may not be monotonic.

  const auto& dist = reduction.full2reducedDist;
  const unsigned char dsize = static_cast<unsigned char>(dist.size()); 

  // The next regular, reduced distribution expected, i.e. 
  // the current one + 1.  The reduced ones may not be in order, but
  // the maximum should only increase one at at time.
  unsigned char dredNext = 0;

  // The immediately previous, reduced distribution that we used.
  unsigned char dredPrev = dsize;

  auto iter = results.begin();
  for (unsigned char dfull = 0; dfull < dsize; dfull++)
  {
    if (dist[dfull] == dredNext)
    {
      iter->expand(dfull, rankAdder);
      dredPrev = dredNext;
      dredNext++;
    }
    else if (dist[dfull] == dredPrev)
    {
      // Rank adder was already added once.
      iter = results.insert(iter, * prev(iter));
      iter->expand(dfull, 0);
      weightInt += iter->getTricks();
    }
    else
    {
      // Go back.
      unsigned char dprev = dfull-1;
      auto iterPrev = prev(iter);
      while (dist[dprev] != dist[dfull])
      {
        dprev--;
        iterPrev--;
      }

      iter = results.insert(iter, * iterPrev);
      iter->expand(dfull, 0);
      weightInt += iter->getTricks();
      dredPrev = dist[dprev];
    }

    iter++;
  }

  if (rotateFlag)
  {
    results.reverse();
    for (auto& result: results)
    {
      result.setDist(dsize - result.getDist() - 1);
      result.flip();
    }
  }

  studied.study(results);
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


bool Strategy::equalCompleteBasic(const Strategy& strat2) const
{
  // Just an alias.
  return (* this == strat2);
}


bool Strategy::equalPrimaryBasic(const Strategy& strat2) const
{
  const unsigned n = results.size();
  assert(strat2.results.size() == n);

  if (weightInt != strat2.weightInt)
    return false;

  list<Result>::const_iterator iter1 = results.cbegin();
  list<Result>::const_iterator iter2 = strat2.results.cbegin();

  while (iter1 != results.end())
  {
    if (iter1->comparePrimaryInDetail(* iter2) != WIN_EQUAL_OVERALL)
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
  // that it does not rely on studied or scrutinized Result's.

  unsigned cumul;

  if (control.runRankComparisons())
  {
    if (! Strategy::cumulate(strat2, true, cumul))
      return false;
  }
  else
  {
    return Strategy::cumulatePrimary(strat2, true, cumul);
  }
  
  if (cumul & WIN_SECOND_PRIMARY)
    return true;
  /*
  else if ((cumul & WIN_FIRST_SECONDARY) ||
      (cumul & WIN_DIFFERENT_SECONDARY))
  {
    // We still eliminate a strategy whose overall result is not better.
    return Strategy::lessEqualSecondaryOverallResult(strat2);
  }
  */
  /* */
  else if (cumul & WIN_FIRST_SECONDARY)
    return false;
  else if (cumul & WIN_DIFFERENT_SECONDARY)
    return false;
    /* */
  else
    return true;
}


Compare Strategy::compareCompleteBasic(const Strategy& strat2) const
{
  // This is mainly for diagnostics.
  assert(strat2.results.size() == results.size());

  unsigned cumul;
  Strategy::cumulate(strat2, false, cumul);

  return compressCompareDetail(cumul);
  
  /*
  Compare compressed;
  CompareDetail cleaned;
  processCompareDetail(cumul, compressed, cleaned);
  if (cleaned == WIN_DIFFERENT_SECONDARY)
  {
    // We still derive a comparison based on the overall result.
    return Strategy::compareSecondaryOverallResult(strat2);
  }
  else
    return compressed;
    */
}


Compare Strategy::compareSecondary(const Strategy& strat2) const
{
  // As we ignore tricks, this is in a sense basic.
  assert(strat2.results.size() == results.size());

  // Result will catch this as well, but no point in checking.
  if (! control.runRankComparisons())
    return WIN_EQUAL;

  unsigned cumul;
  Strategy::cumulateSecondary(strat2, cumul);

  return compressCore(cumul);

  /*
  Compare compressed;
  CompareDetail cleaned;
  processCore(cumul, compressed, cleaned);
  if (cleaned == WIN_DIFFERENT_SECONDARY)
  {
    // We still derive a comparison based on the overall result.
    return Strategy::compareSecondaryOverallResult(strat2);
  }
  else
    return compressed;
    */
}


bool Strategy::lessEqualSecondaryOverallResult(
  const Strategy& strat2) const
{
  const Result res1 = Strategy::resultLowest();
  const Result res2 = strat2.resultLowest();
  const CompareDetail c = res1.compareSecondaryInDetail(res2);

  if (c == WIN_SECOND_SECONDARY || c == WIN_EQUAL_OVERALL)
    return true;
  else
    return false;
}


Compare Strategy::compareSecondaryOverallResult(
  const Strategy& strat2) const
{
  const Result res1 = Strategy::resultLowest();
  const Result res2 = strat2.resultLowest();

  const Compare c = res1.compareComplete(res2);

  // Only use the truly different results.
  return (c == WIN_EQUAL ? WIN_DIFFERENT : c);

  // return res1.compareComplete(res2);
}


bool Strategy::equalCompleteStudied(const Strategy& strat2) const
{
  if (! studied.maybeLessEqualStudied(strat2.studied))
    return false;
  else if (! strat2.studied.maybeLessEqualStudied(studied))
    return false;
  else
    return (* this == strat2);
}


bool Strategy::equalPrimaryStudied(const Strategy& strat2) const
{
  if (! studied.maybeLessEqualStudied(strat2.studied))
    return false;
  else if (! strat2.studied.maybeLessEqualStudied(studied))
    return false;
  else
    return Strategy::equalPrimaryBasic(strat2);
}


bool Strategy::lessEqualCompleteStudied(const Strategy& strat2) const
{
  if (! studied.maybeLessEqualStudied(strat2.studied))
    return false;
  else
    return Strategy::lessEqualCompleteBasic(strat2);
}


bool Strategy::lessEqualPrimaryStudied(const Strategy& strat2) const
{
  // This uses studied results if possible, otherwise the basic method.
  if (! studied.maybeLessEqualStudied(strat2.studied))
    return false;
  else
  {
    unsigned cumul;
    return Strategy::cumulatePrimary(strat2, true, cumul);
  }
}


Compare Strategy::compareCompleteStudied(
  const Strategy& strat2) const
{
  // The first method only goes as far as the studied results allows.
  // Unlike in the other methods:
  //                  >=   ==   <=   !=
  // WIN_EQUAL        poss poss poss poss
  // WIN_FIRST        poss           poss
  // WIN_SECOND                 poss poss
  // WIN_DIFFERENT                   poss

  const Compare c = studied.comparePartialPrimaryStudied(strat2.studied);

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
  return studied.lessEqualScrutinized(strat2.studied);
}


Compare Strategy::comparePrimaryScrutinized(const Strategy& strat2) const
{
  return studied.comparePrimaryScrutinized(strat2.studied);
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
    if (iter1 == results.end() || iter1->getDist() > iter2->getDist())
    {
      results.insert(iter1, * iter2);
      weightInt += iter2->getTricks();
      iter2++;
    }
    else if (iter1->getDist() < iter2->getDist())
    {
      iter1++;
    }
    else 
    {
      if (iter1->getTricks() > iter2->getTricks())
        weightInt += static_cast<unsigned>
          (iter2->getTricks() - iter1->getTricks());
      * iter1 *= * iter2;

      iter1++;
      iter2++;
    }
  }

  studied.study(results);
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
          weightInt += it->getTricks();
      }
      break;
    }
    else if (iter2 == strat2.results.end())
    {
      results.insert(results.end(), iter1, strat1.results.end());
      for (auto it = iter1; it != strat1.results.end(); it++)
        weightInt += it->getTricks();
      break;
    }

    if (iter1->getDist() < iter2->getDist())
    {
      results.push_back(* iter1);
      weightInt += iter1->getTricks();
      iter1++;
    }
    else if (iter1->getDist() > iter2->getDist())
    {
      results.push_back(* iter2);
      weightInt += iter2->getTricks();
      iter2++;
    }
    else if (iter1->getTricks() < iter2->getTricks())
    {
      // Take the one with the lower number of tricks.
      results.push_back(* iter1);
      weightInt += iter1->getTricks();
      iter1++;
      iter2++;
    }
    else if (iter1->getTricks() > iter2->getTricks())
    {
      results.push_back(* iter2);
      weightInt += iter2->getTricks();
      iter1++;
      iter2++;
    }
    else
    {
      // Opponents can choose among the two winners.
      results.push_back(* iter1);
      results.back() *= * iter2;
      weightInt += iter1->getTricks();
      iter1++;
      iter2++;
    }
  }

  studied.study(results);
}


/************************************************************
 *                                                          *
 * Range methods                                            *
 *                                                          *
 ************************************************************/

void Strategy::initRanges(Ranges& ranges) const
{
  ranges.init(results, control.runRankComparisons());
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
  weightInt = result.getTricks();
}


void Strategy::updateSameLength(
  const Play& play,
  const SurvivorList& survivors)
{
  auto iter1 = results.begin();
  auto iter2 = survivors.begin();

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
  const SurvivorList& survivors)
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
  results.resize(survivors.sizeFull());
  auto iterSurvivors = survivors.begin();
  weightInt = 0;

  for (auto& res: results)
  {
    // Use the survivor's full distribution number and the 
    // corresponding result entry as the trick count.
    res = resultsOld[iterSurvivors->reducedNo];
    res.setDist(iterSurvivors->fullNo);

    weightInt += res.getTricks();
    iterSurvivors++;
  }
}


void Strategy::adaptResults(
  const Play& play,
  const SurvivorList& survivors)
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

    Strategy::updateSingle(play, survivors.front().fullNo);
  }
  else if (eastVoidFlag)
  {
    // Only keep the last result.
    if (len1 > 1)
      results.erase(results.begin(), prev(results.end()));

    Strategy::updateSingle(play, survivors.front().fullNo);
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
  const SurvivorList& survivors)
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

  studied.study(results);
}


void Strategy::setAndAdaptVoid(
  const Play& play,
  const Result& resultWestVoid,
  const Result& resultEastVoid,
  const bool westVoidFlag,
  const bool eastVoidFlag,
  const unsigned char fullDistNo)
{
  // The void flags are from the perspective of the current trick.
  assert(westVoidFlag != eastVoidFlag);

  Strategy::reset();

  // The result we will modify.
  const Result& resultBase = (westVoidFlag == play.rotateFlag ?
    resultEastVoid : resultWestVoid);

  // The distribution number we will use.
  // const unsigned char dno = (westVoidFlag ? 0 : dmax);

  results.emplace_back(resultBase);

  Result& result = results.front();

  if (play.rotateFlag)
    result.flip();

  result.update(play, fullDistNo);
  weightInt = result.getTricks();

  Strategy::study();
}


/************************************************************
 *                                                          *
 * Winners                                                  *
 *                                                          *
 ************************************************************/

unsigned Strategy::covers(
  const Covers& coversIn,
  ResExpl& resExpl) const
{
  resExpl.reset();
  CoverState state = coversIn.explain(results, resExpl);
  return (state == COVER_DONE ? 1 : 0);
}


void Strategy::studyOppsVoid(
  Result& resultWestVoid,
  Result& resultEastVoid) const
{
  // When West or East is void, only the first resp. last result matters.
  assert(! results.empty());
  
  resultWestVoid += results.front();
  resultEastVoid += results.back();
}

  
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

