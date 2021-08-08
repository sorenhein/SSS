#include <iostream>
#include <iomanip>
#include <sstream>

#include "Strategies.h"
#include "Extensions.h"
#include "StratData.h"

#include "../plays/Play.h"

#include "../Survivor.h"

// TMP
#include "../utils/Timer.h"
extern vector<Timer> timersStrat;


Strategies::Strategies()
{
  Strategies::reset();
}


Strategies::~Strategies()
{
}


void Strategies::reset()
{
  strategies.clear();
  rangesNew.reset();
  scrutinizedFlag = false;
}


/************************************************************
 *                                                          *
 * Ways in which new Strategies arise: setTrivial and adapt *
 *                                                          *
 ************************************************************/

void Strategies::setTrivial(
  const Result& trivial,
  const unsigned char len)
{
  // Repeat the trivial result len times.

  Strategies::reset();
  strategies.emplace_back(Strategy());
  Strategy& strat = strategies.back();
  strat.logTrivial(trivial, len);
}


void Strategies::collapseOnVoid()
{
  assert(strategies.size() > 0);
  if (strategies.size() == 1)
  {
    assert(strategies.front().size() == 1);
    return;
  }

  auto iterBest = strategies.begin();
  assert(iterBest->size() == 1);

  // Find the best one for declarer.
  for (auto iter = next(strategies.begin()); 
      iter != strategies.end(); iter++)
  {
    assert(iter->size() == 1);
    if (* iter >= * iterBest)
      iterBest = iter;
  }

  // Copy it to the front and remove the others.
  strategies.front() = * iterBest;
  strategies.erase(next(strategies.begin()), strategies.end());
}


void Strategies::adapt(
  const Play& play,
  const Survivors& survivors)
{
timersStrat[0].start();

  // Adapt the Strategies of a following play to this trick by
  // rotating, mapping etc.  This is a somewhat expensive method.

  for (auto& strat: strategies)
    strat.adapt(play, survivors);

  if (play.lhoPtr->isVoid() || play.rhoPtr->isVoid())
    Strategies::collapseOnVoid();

  scrutinizedFlag = false;

timersStrat[0].stop();
}


/************************************************************
 *                                                          *
 * Cleaning up an existing strategy                         *
 *                                                          *
 ************************************************************/

void Strategies::consolidateTwo()
{
  // Check whether to swap the two and whether one is dominated.

  auto iter1 = strategies.begin();
  auto iter2 = next(iter1);
  if (iter1->weight() < iter2->weight())
  {
    if (iter2->greaterEqualByProfile(* iter1))
      strategies.pop_front();
    else
      iter_swap(iter1, iter2);
  }
  else if (iter1->greaterEqualByProfile(* iter2))
    strategies.pop_back();
}


void Strategies::consolidate()
{
  // Used when a strategy may have gone out of order.  
  // In Node::purgeRanges individual distributions may have been
  // removed from Strategies, so that the strategies are no longer
  // in order and may even have dominations among them.

  if (Strategies::empty())
    return;

  Strategies::restudy();

  if (strategies.size() == 1)
  {
    // Don't have to do anything.
    return;
  }
  else if (strategies.size() == 2)
  {
    // The general way also works in this case, and it is just
    // a small optimization.
    Strategies::consolidateTwo();
    return;
  }
  else
  {
timersStrat[1].start();

    auto oldStrats = move(strategies);
    strategies.clear();  // But leave ranges intact

    for (auto& strat: oldStrats)
      * this += strat;

timersStrat[1].stop();
  }
}


void Strategies::restudy()
{
  for (auto& strategy: strategies)
    strategy.restudy();
}


void Strategies::scrutinize(const Ranges& rangesIn)
{
  for (auto& strat: strategies)
    strat.scrutinize(rangesIn);

  scrutinizedFlag = true;
}


/************************************************************
 *                                                          *
 * operator == and two helper methods                       *
 *                                                          *
 ************************************************************/

bool Strategies::sameOrdered(const Strategies& strats2) const
{
  // This assumes the same ordering.

  auto iter2 = strats2.strategies.begin();

  for (auto iter1 = strategies.begin(); iter1 != strategies.end(); 
      iter1++, iter2++)
  {
    if (!(* iter1 == * iter2))
      return false;
  }

  return true;
}


bool Strategies::sameUnordered(const Strategies& strats2) const
{
  // This is dreadfully slow.  We could make use of the 
  // weight-ordered nature, and perhaps also the used-up matches,
  // if this ever became a performance issue.  But I think it's
  // mainly used for debugging.

  for (auto iter1 = strategies.begin(); iter1 != strategies.end(); iter1++)
  {
    bool sameFlag = false;
    for (auto iter2 = strats2.strategies.begin(); 
        iter2 != strats2.strategies.end(); iter2++)
    {
      if (* iter1 == * iter2)
      {
        sameFlag = true;
        break;
      }
    }

    if (! sameFlag)
      return false;
  }

  return true;
}


bool Strategies::operator == (const Strategies& strats2) const
{
  if (Strategies::size() != strats2.size())
    return false;
  else if (Strategies::sameOrdered(strats2))
    return true;
  else
    return Strategies::sameUnordered(strats2);
}


/************************************************************
 *                                                          *
 * operator += Strategy and a helper method                 *
 *                                                          *
 ************************************************************/

void Strategies::addStrategy(
  const Strategy& strat,
  ComparatorType comparator)
{
  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  
  if (strategies.empty())
  {
    // This happens in consolidate() with the first entry.
    strategies.push_back(strat);
    return;
  }

  auto iter = strategies.begin();
  while (iter != strategies.end() && iter->weight() >= strat.weight())
  {
    // Is the new strat dominated?
    if (((* iter).*comparator)(strat))
    {
/* */
if (iter->weight() == strat.weight())
{
  if (iter->consolidateByRank(strat))
  {
timersStrat[28].start();
timersStrat[28].stop();
  }
  else
  {
  cout << "HERE1 addStrategy\n";
  cout << iter->str("iter", true);
  cout << strat.str("str", true) << endl;
  assert(false);
  }
}
/* */
      return;
    }
    else
      iter++;
  }

  // The new vector must be inserted.  This consumes about a third
  // of the time of the overall method.
  iter = next(strategies.insert(iter, strat));

  // The new vector may dominate lighter vectors. This only consumes
  // 5-10% of the overall time.
  while (iter != strategies.end())
  {
if (iter->weight() == strat.weight())
{
  cout << "HERE2 addStrategy\n";
  cout << iter->str("iter");
  cout << strat.str("str") << endl;
  assert(false);
// timersStrat[29].start();
// timersStrat[29].stop();
}

    if ((strat.*comparator)(* iter))
      iter = strategies.erase(iter);
    else
      iter++;
  }
}


void Strategies::operator += (const Strategy& strat)
{
  // Gets called from consolidate and from += strats.

  ComparatorType comp;
  unsigned tno;
  if (scrutinizedFlag)
  {
    comp = &Strategy::greaterEqualByProfile;
    tno = 2;
  }
  else
  {
    comp = &Strategy::operator >=;
    tno = 3;
  }

timersStrat[tno].start();
  Strategies::addStrategy(strat, comp);
timersStrat[tno].stop();

}


/************************************************************
 *                                                          *
 * operator += Strategies and helper methods                *
 *                                                          *
 ************************************************************/

void Strategies::plusOneByOne(const Strategies& strats2)
{
  // Simple version when both Strategies are known to have size 1,
  // as happens very frequently.

  auto& str1 = strategies.front();
  auto& str2 = strats2.strategies.front();

  if (str1 >= str2)
    return;
  else if (str2 >= str1)
    * this = strats2;
  else
  {
    if (str1.weight() >= str2.weight())
      strategies.push_back(str2);
    else
      strategies.push_front(str2);
  }
}


void Strategies::markChanges(
  const Strategies& strats2,
  list<Addition>& additions,
  list<list<Strategy>::const_iterator>& deletions) const
{
  // The simple Strategies += Strategies adds an individual strategy
  // to the LHS if it is not dominated.  Then the following
  // strategies from the RHS are also compared to this, but we already
  // know that there is no point.  This is a more complicated way
  // to do the comparisons up front between all LHS and RHS pairs,
  // but only updating LHS at the end.

  vector<unsigned> ownDeletions(strategies.size(), 0);

  for (auto& strat: strats2.strategies)
  {
    auto iter = strategies.begin();
    unsigned stratNo = 0;
    bool doneFlag = false;
    while (iter != strategies.end() && iter->weight() >= strat.weight())
    {
      if (ownDeletions[stratNo])
      {
        iter++;
        stratNo++;
        continue;
      }

      if (iter->greaterEqualByProfile(strat))
      {
        doneFlag = true;
        break;
      }
      else
      {
        iter++;
        stratNo++;
      }
    }

    if (doneFlag)
      continue;

    // Note for insertion.
    additions.emplace_back(Addition());
    auto& addition = additions.back();
    addition.ptr = &strat;
    addition.iter = iter;

    // The new vector may dominate lighter vectors.
    while (iter != strategies.end())
    {
      if (strat.greaterEqualByProfile(* iter))
      {
        if (ownDeletions[stratNo] == 0)
        {
          deletions.push_back(iter);
          ownDeletions[stratNo] = 1;
        }
      }
      iter++;
      stratNo++;
    }
  }
}


void Strategies::operator += (Strategies& strats2)
{
  if (strategies.empty())
  {
    * this = strats2;
    return;
  }
  else if (strats2.empty())
    return;

  const unsigned sno1 = strategies.size();
  const unsigned sno2 = strats2.size();

  if (sno1 == 1 && sno2 == 1)
  {
    // Simplified case.
    Strategies::plusOneByOne(strats2);
  }
  else if (sno1 >= 20 && sno2 >= 20)
  {
    // Rare, but very slow per invocation when it happens.
    // Consumes perhaps 75% of the method time, so more optimized.

timersStrat[4].start();

    // We only need the minima here, but we use the existing method.
    Strategies::makeRanges();
    strats2.makeRanges();
    Strategies::propagateRanges(strats2);

    Strategies::scrutinize(rangesNew);
    strats2.scrutinize(rangesNew);

    list<Addition> additions;
    list<list<Strategy>::const_iterator> deletions;
    Strategies::markChanges(strats2, additions, deletions);

    for (auto& addition: additions)
      strategies.insert(addition.iter, *(addition.ptr));

    for (auto& deletion: deletions)
      strategies.erase(deletion);

timersStrat[4].stop();
  }
  else
  {
timersStrat[5].start();

    // General case.  Frequent and fast, perhaps 25% of the method time.

    // We may inherit a set scrutinizedFlag from the previous branch.
    // But it's generally not worth in this this case.
    scrutinizedFlag = false;

    for (auto& strat2: strats2.strategies)
      * this += strat2;

timersStrat[5].stop();

  }
}


/************************************************************
 *                                                          *
 * operator *= Strategy                                     *
 *                                                          *
 ************************************************************/

void Strategies::operator *= (const Strategy& strat)
{
  // This does not re-sort and consolidate strategies.  If that
  // needs to be done, the caller must do it.  It is currently only
  // called from Node::reactivate().

  if (strategies.empty())
  {
    strategies.push_back(strat);
  }
  else
  {
timersStrat[6].start();

    for (auto& strat1: strategies)
      strat1 *= strat;

timersStrat[6].stop();
  }
}


/************************************************************
 *                                                          *
 * operator *= Strategies and many helper methods           *
 *                                                          *
 ************************************************************/

void Strategies::multiplyAddStrategy(
  const Strategy& strat1,
  const Strategy& strat2,
  ComparatorType comparator)
{
  // This costs about two thirds of the overall method time.
  auto& product = strategies.back();
  product.multiply(strat1, strat2);
  if (! rangesNew.empty())
    product.scrutinize(rangesNew);
  auto piter = prev(strategies.end());

  if (strategies.size() == 1)
  {
    // Keep the product and make a new scratch-pad element.
    strategies.emplace_back(Strategy());
    return;
  }

  // The strategies list is in descending order of weights.
  // The new Strategy might dominate everything with a lower weight and
  // can only be dominated by a Strategy with at least its own weight.
  // This checking costs about one third of the overall method time.
  
  auto iter = strategies.begin();
  while (iter != piter && iter->weight() >= piter->weight())
  {
    if (((* iter).*comparator)(* piter))
    {
      // The new strat is dominated.
      return;
    }
    else
      iter++;
  }

  // Already in the right place at the end?
  if (iter == piter)
  {
    strategies.emplace_back(Strategy());
    return;
  }

  // The new vector must be inserted, i.e. spliced in.
  // This is super-fast.
  strategies.splice(iter, strategies, piter);
  piter = prev(iter);

  // The new vector may dominate lighter vectors.  This is also
  // quite efficient and doesn't happen so often.
  while (iter != strategies.end())
  {
    if (((* piter).*comparator)(* iter))
      iter = strategies.erase(iter);
    else
      iter++;
  }

  // Make a new scratch-pad element.
  strategies.emplace_back(Strategy());
}


void Strategies::operator *= (Strategies& strats2)
{
  // This method only gets called from Nodes::cross, which means
  // that *this is a parent node and strats2 is a child node.
  // If the method is used differently, unexpected behavior may
  // occur!

  const unsigned len2 = strats2.strategies.size();
  if (len2 == 0)
  {
    // Keep the current results.
    return;
  }

  const unsigned len1 = strategies.size();
  if (len1 == 0)
  {
    // Keep the new results, but don't change ranges.
    strategies = strats2.strategies;
    return;
  }

  if (len1 == 1 && len2 == 1)
  {
    strategies.front() *= strats2.strategies.front();
    return;
  }

  if (rangesNew.empty() || len1 < 10 || len2 < 10)
  {

    // This implementation of the general product reduces
    // memory overhead.  The temporary product is formed in the last
    // element of Strategies as a scratch pad.  If it turns out to be
    // viable, it is already in Strategies and subject to move semantics.

    ComparatorType comp;
    unsigned tno;
    if (rangesNew.empty())
    {
      comp = &Strategy::operator >=;
      tno = 7;
    }
    else
    {
      comp = &Strategy::greaterEqualByProfile;
      tno = 8;
    }

timersStrat[tno].start();

    // ComparatorType comp = (ranges.empty() ? &Strategy::operator >= :
      // &Strategy::greaterEqualByProfile);

    auto strategiesOwn = move(strategies);
    strategies.clear();
    strategies.emplace_back(Strategy());

    for (auto& strat1: strategiesOwn)
      for (auto& strat2: strats2.strategies)
        Strategies::multiplyAddStrategy(strat1, strat2, comp);

    strategies.pop_back();

timersStrat[tno].stop();
    return;
  }
  else
  {

    // This is the most complex version, and I may have gotten a bit
    // carried away...  The two Strategies have distributions that are 
    // overlapping as well as distributions that are unique to each of 
    // them.  We split these out, and we pre-compare within each 
    // Strategies.  This makes it faster to compare products from each 
    // Strategies.

timersStrat[9].start();

    Extensions extensions;
    extensions.split(* this, strats2.strategies.front(), 
      EXTENSION_SPLIT1);
    extensions.split(strats2, strategies.front(), EXTENSION_SPLIT2);

    extensions.multiply(rangesNew);

    strategies.clear();
    extensions.flatten(strategies);

timersStrat[9].stop();
  }
}


/************************************************************
 *                                                          *
 * Utilities                                                *
 *                                                          *
 ************************************************************/

const Strategy& Strategies::front() const
{
  return strategies.front();
}


unsigned Strategies::size() const
{
  return strategies.size();
}


bool Strategies::empty() const
{
  return (strategies.empty() || strategies.front().empty());
}


bool Strategies::ordered() const
{
  // Check that the strategies are ordered by descending weight.
  for (auto iter = strategies.begin(); iter != prev(strategies.end()); 
      iter++)
  {
    if (iter->weight() < next(iter)->weight())
      return false;
  }
  return true;
}


void Strategies::getLoopData(StratData& stratData)
{
  // This is used to loop over all strategies in synchrony, one
  // distribution at a time.  If the caller is going to change
  // anything inside Strategies with this, the caller must also
  // consider the effect on scrutinizedFlag.
  auto siter = stratData.data.begin();
  for (auto& strat: strategies)
  {
    siter->ptr = &strat;
    siter->iter = strat.begin();
    siter->end = strat.end();
    siter++;
  }
}


/************************************************************
 *                                                          *
 * Ranges methods                                           *
 *                                                          *
 ************************************************************/

void Strategies::makeRanges()
{
  if (strategies.empty())
    return;

  strategies.front().initRanges(rangesNew);

  if (strategies.size() == 1)
    return;

  for (auto iter = next(strategies.begin()); 
      iter != strategies.end(); iter++)
    iter->extendRanges(rangesNew);
}


void Strategies::propagateRanges(const Strategies& child)
{
  // This propagates the child's ranges to the current parent ranges.
  // The distribution number has to match.

  rangesNew *= child.rangesNew;
}


const Ranges& Strategies::getRanges() const
{
  return rangesNew;
}


/************************************************************
 *                                                          *
 * string methods                                           *
 *                                                          *
 ************************************************************/

string Strategies::strRanges(const string& title) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";

  ss << rangesNew.strHeader();
  // if (! rangesNew.empty())
    // ss << rangesNew.front().strHeader();
  for (auto& range: rangesNew)
    ss << range.str();

  return ss.str();
}


string Strategies::strHeader(
  const string& title,
  const bool rankFlag) const
{
  stringstream ss;
  if (title != "")
    ss << title << "\n";

  const unsigned incr = (rankFlag ? 12 : 4);

  ss << setw(4) << left << "Dist" << right;

  for (unsigned i = 0; i < strategies.size(); i++)
    ss << setw(static_cast<int>(incr)) << i;
  ss << "\n";

  ss << string(4 + incr * strategies.size(), '-') << "\n";

  return ss.str();
}


string Strategies::strWeights(const bool rankFlag) const
{
  stringstream ss;

  const unsigned incr = (rankFlag ? 12 : 4);
  ss << string(4 + incr * strategies.size(), '-') << "\n";
  ss << setw(4) << "Wgt";
  for (const auto& res: strategies)
    ss << setw(static_cast<int>(incr)) << res.weight();
  return ss.str() + "\n";
}


string Strategies::str(
  const string& title,
  const bool rankFlag) const
{
  if (strategies.size() == 0)
    return "";

  stringstream ss;
  ss << Strategies::strHeader(title, rankFlag);

  // Make a list of iterators -- one per Strategy.
  list<list<Result>::const_iterator> iters, itersEnd;
  for (auto& res: strategies)
  {
    iters.push_back(res.begin());
    itersEnd.push_back(res.end());
  }

  // Use the iterator for the first Strategy to get the distributions.
  while (iters.front() != itersEnd.front())
  {
    ss << setw(4) << left << +iters.front()->dist << right;
    for (auto& iter: iters)
    {
      ss << iter->strEntry(rankFlag);

      // This looks funny, but it's the content of iters that is modified.
      iter++;
    }
    ss << "\n";
  }

  ss << Strategies::strWeights(rankFlag);

  return ss.str();
}

