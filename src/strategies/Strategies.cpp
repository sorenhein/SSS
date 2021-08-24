/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Strategies.h"
#include "optim/Extensions.h"

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
  slist.clear();
  ranges.reset();
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
timersStrat[0].start();

  // Repeat the trivial result len times.

  ranges.reset();
  scrutinizedFlag = false;

  slist.setTrivial(trivial, len);

timersStrat[0].stop();
}


void Strategies::collapseOnVoid()
{
timersStrat[1].start();

  slist.collapseOnVoid();

timersStrat[1].stop();
}


void Strategies::adapt(
  const Play& play,
  const Survivors& survivors)
{
timersStrat[2].start();

  slist.adapt(play, survivors);
  scrutinizedFlag = false;

timersStrat[2].stop();
}


/************************************************************
 *                                                          *
 * Cleaning up an existing strategy                         *
 *                                                          *
 ************************************************************/

void Strategies::consolidate()
{
  // Used when a strategy may have gone out of order.  
  // In Node::purgeRanges individual distributions may have been
  // removed from Strategies, so that the strategies are no longer
  // in order and may even have dominations among them.

  if (Strategies::empty())
    return;

  Strategies::restudy();

  if (slist.size() == 1)
  {
    // Don't have to do anything.
    return;
  }
  else if (slist.size() == 2)
  {
timersStrat[3].start();

    // The general way also works in this case, and it is just
    // a small optimization.
    ComparatorType lessEqualMethod = (scrutinizedFlag ? 
      &Strategy::lessEqualPrimaryScrutinized : 
      &Strategy::lessEqualPrimaryStudied);

    slist.consolidateTwo(lessEqualMethod);

timersStrat[3].stop();
    return;
  }
  else
  {
timersStrat[4].start();

    // But leave ranges intact.
    // The method was not explicit until now -- is it right?
    slist.consolidate(&Strategy::lessEqualPrimaryStudied);

timersStrat[4].stop();
  }
}


void Strategies::restudy()
{
timersStrat[5].start();

  for (auto& strategy: slist)
    strategy.restudy();

timersStrat[5].stop();
}


void Strategies::scrutinize(const Ranges& rangesIn)
{
timersStrat[6].start();

  for (auto& strategy: slist)
    strategy.scrutinize(rangesIn);

  scrutinizedFlag = true;

timersStrat[6].stop();
}


/************************************************************
 *                                                          *
 * operator == and two helper methods                       *
 *                                                          *
 ************************************************************/

bool Strategies::operator == (const Strategies& strats2) const
{
  return (slist == strats2.slist);
}


/************************************************************
 *                                                          *
 * operator += Strategies and helper methods                *
 *                                                          *
 ************************************************************/

void Strategies::operator += (Strategies& strats2)
{
  if (slist.empty())
  {
    * this = strats2;
    return;
  }
  else if (strats2.empty())
    return;

  const unsigned sno1 = slist.size();
  const unsigned sno2 = strats2.slist.size();

  if (sno1 == 1 && sno2 == 1)
  {
    // Simplified case.
timersStrat[7].start();
    slist.plusOneByOne(strats2.slist);
timersStrat[7].stop();
  }
  else if (sno1 >= 20 && sno2 >= 20)
  {
    // Rare, but very slow per invocation when it happens.
    // Consumes perhaps 75% of the method time, so more optimized.

timersStrat[8].start();

    // We only need the minima here, but we use the existing method.

    Strategies::makeRanges();
    strats2.makeRanges();
    Strategies::propagateRanges(strats2);

    Strategies::scrutinize(ranges);
    strats2.scrutinize(ranges);

    list<Addition> additions;
    list<list<Strategy>::const_iterator> deletions;
    slist.markChanges(strats2.slist, additions, deletions);

    slist.processChanges(additions, deletions);

timersStrat[8].stop();
  }
  else
  {
timersStrat[9].start();

    // General case.  Frequent and fast, perhaps 25% of the method time.

    // We may inherit a set scrutinizedFlag from the previous branch.
    // But it's generally not worth it in this case.
    scrutinizedFlag = false;

    slist.add(strats2.slist, &Strategy::lessEqualPrimaryStudied);

timersStrat[9].stop();
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

  if (slist.empty())
  {
    slist.push_back(strat);
  }
  else
  {
timersStrat[10].start();

    slist *= strat;

timersStrat[10].stop();
  }
  // Addition: Correct?
  scrutinizedFlag = false;
}


/************************************************************
 *                                                          *
 * operator *= Strategies and many helper methods           *
 *                                                          *
 ************************************************************/

void Strategies::operator *= (Strategies& strats2)
{
  // This method only gets called from Nodes::cross, which means
  // that *this is a parent node and strats2 is a child node.
  // If the method is used differently, unexpected behavior may
  // occur!

  const unsigned len2 = strats2.slist.size();
  if (len2 == 0)
  {
    // Keep the current results.
    return;
  }

  const unsigned len1 = slist.size();
  if (len1 == 0)
  {
    // Keep the new results, but don't change ranges.
    slist = strats2.slist;
    // Addition
    // TODO Just copy the whole thing, * this = strats2 ?
    scrutinizedFlag = strats2.scrutinizedFlag;
    return;
  }

  if (len1 == 1 && len2 == 1)
  {
timersStrat[11].start();

    slist.multiplyOneByOne(strats2.slist);
    scrutinizedFlag = false;

timersStrat[11].stop();
    return;
  }

  if (ranges.empty() || len1 < 10 || len2 < 10)
  {
timersStrat[12].start();

    // This implementation of the general product reduces
    // memory overhead.  The temporary product is formed in the last
    // element of Strategies as a scratch pad.  If it turns out to be
    // viable, it is already in Strategies and subject to move semantics.

    ComparatorType lessEqualMethod =
      &Strategy::lessEqualPrimaryScrutinized;

    // TODO Just to make it work.  Slow?
    Strategies::makeRanges();
    strats2.makeRanges();
    Strategies::propagateRanges(strats2);

    Strategies::scrutinize(ranges);
    strats2.scrutinize(ranges);

    slist.multiply(strats2.slist, ranges, lessEqualMethod);

    scrutinizedFlag = false;

timersStrat[12].stop();
    return;
  }
  else
  {
timersStrat[13].start();

    // This is the most complex version, and I may have gotten a bit
    // carried away...  The two Strategies have distributions that are 
    // overlapping as well as distributions that are unique to each of 
    // them.  We split these out, and we pre-compare within each 
    // Strategies.  This makes it faster to compare products from each 
    // Strategies.
    //
    // Even though Extensions splits Strategy's into own and shared
    // by distribution, we can still share the central ranges.


    Strategies::makeRanges();
    strats2.makeRanges();
    Strategies::propagateRanges(strats2);

    Extensions extensions;
    extensions.split(* this, strats2.slist.front(), 
      ranges, EXTENSION_SPLIT1);
    extensions.split(strats2, slist.front(), 
      ranges, EXTENSION_SPLIT2);

    extensions.multiply(ranges);

    slist.clear();
    extensions.flatten(slist);

    scrutinizedFlag = false;

timersStrat[13].stop();
  }
}


/************************************************************
 *                                                          *
 * Utilities                                                *
 *                                                          *
 ************************************************************/

const Strategy& Strategies::front() const
{
  return slist.front();
}


unsigned Strategies::size() const
{
  return slist.size();
}


bool Strategies::empty() const
{
  return slist.empty();
}


bool Strategies::ordered() const
{
  return slist.ordered();
}


bool Strategies::minimal() const
{
  return slist.minimal();
}


void Strategies::splitDistributions(
  const Strategy& counterpart,
  Strategies& own,
  Strategies& shared)
{
  slist.splitDistributions(counterpart, own.slist, shared.slist);
}


void Strategies::reactivate(
  Strategy& simpleStrat, // TODO const?
  const Strategy& constants)
{
  simpleStrat *= constants;
  * this *= simpleStrat;

  // As simpleStrat is completely complementary to strats,
  // we often do not need to re-sort and consolidate.
  // But it can happen that the defenders can hold declarer to a
  // constant 2 tricks (10/16907, lead 7, then rising with the king),
  // or the defender can give declarer several choices in the range
  // of 1-3 tricks.  Since the defenders move after declarer
  // publishes his strategy, he will never get 3 tricks.  When
  // the simple strategy is multiplied back here, weights can change
  // and therefore the ordering can also change.

  if (! simpleStrat.empty())
  {
    // TODO Maybe only when real simpleStrats non-empty?
    // Constants don't change anything?
    Strategies::consolidate();
  }
}


/************************************************************
 *                                                          *
 * Ranges methods                                           *
 *                                                          *
 ************************************************************/

void Strategies::makeRanges()
{
timersStrat[14].start();

  slist.makeRanges(ranges);

timersStrat[14].stop();
}


void Strategies::propagateRanges(const Strategies& child)
{
timersStrat[15].start();

  // This propagates the child's ranges to the current parent ranges.
  // The distribution number has to match.

  ranges*= child.ranges;

timersStrat[15].stop();
}


bool Strategies::purgeRanges(
  Strategy& constants,
  const Ranges& rangesParent,
  const bool debugFlag)
{
  if (slist.empty())
    return false;

timersStrat[16].start();

  const bool eraseFlag = slist.purgeRanges(constants,
    ranges, rangesParent, debugFlag);

  if (eraseFlag)
  {
    // Some strategies may be dominated that weren't before.
    Strategies::consolidate();

    // It could happen that a strategy has become dominated after
    // the erasures.  To take advantage of this we'd have to redo
    // the loop (only for dominance, not for constants), so we'd
    // regenerate stratData first.  But this is just an optimization
    // anyway, so we'll stop here.

    if (debugFlag)
    {
      cout << constants.str("\nNew constants", true) << "\n";
      cout << Strategies::str("Ranges after purging", true);
    }
  }

timersStrat[16].stop();

  return eraseFlag;
}


const Ranges& Strategies::getRanges() const
{
  return ranges;
}


/************************************************************
 *                                                          *
 * Winners methods                                          *
 *                                                          *
 ************************************************************/

const Result Strategies::resultLowest() const
{
timersStrat[17].start();

  auto& res = slist.resultLowest();

timersStrat[17].start();
  return res;
  // TODO Is this broken?
}


/************************************************************
 *                                                          *
 * string methods                                           *
 *                                                          *
 ************************************************************/

string Strategies::strRanges(const string& title) const
{
  return ranges.str(title);
}


string Strategies::str(
  const string& title,
  const bool rankFlag) const
{
  return slist.str(title,rankFlag);
}

