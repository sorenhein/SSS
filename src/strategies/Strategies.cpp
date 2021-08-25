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


/************************************************************
 *                                                          *
 * Simple methods                                           *
 *                                                          *
 ************************************************************/

void Strategies::reset()
{
  slist.clear();
  ranges.reset();
  scrutinizedFlag = false;
}


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

  ranges.reset();
  scrutinizedFlag = false;

  slist.setTrivial(trivial, len);
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
 * Consolidating, studying and scrutinizing (internal)      *
 *                                                          *
 ************************************************************/

void Strategies::consolidate()
{
  // Used when a strategy may have gone out of order.  
  // In Node::purgeRanges individual distributions may have been
  // removed from Strategies, so that the strategies are no longer
  // in order and may even have dominations among them.
  // In reactivate, the changes may also have had such an effect.

  Strategies::study();

  if (slist.size() <= 1 || Strategies::empty())
  {
    // Don't have to do anything.
    return;
  }
  else if (slist.size() == 2)
  {
    // The general way also works in this case, and it is just
    // a small, but meaningful optimization.

    slist.consolidateTwo(&Strategy::lessEqualPrimaryStudied);
    return;
  }
  else
  {
timersStrat[3].start();

    // It actually causes a slowdown to scrutinize here,
    // even for quite large strategies.
    slist.consolidate(&Strategy::lessEqualPrimaryStudied);

timersStrat[3].stop();
  }
}


void Strategies::reactivate(
  const Strategy& simpleStrat,
  const Strategy& constants)
{
timersStrat[4].start();

  scrutinizedFlag = false;

  if (simpleStrat.empty())
  {
    // Constants don't throw off the consolidation.
    if (! constants.empty())
      slist *= constants;
    return;
  }
  else
  {
    Strategy st = simpleStrat;
    if (! constants.empty())
      st *= constants;

    slist *= st;

timersStrat[4].stop();

    // Timing is already counted once above.
    Strategies::consolidate();
  }

  // TODO Put the consolidate in *= strat in slist.
  // Maybe not even have a *= strat in Stragies.
}


void Strategies::study()
{
timersStrat[5].start();

  for (auto& strategy: slist)
    strategy.study();

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
 * operator ==                                              *
 *                                                          *
 ************************************************************/

bool Strategies::operator == (const Strategies& strats2) const
{
  return (slist == strats2.slist);
}


/************************************************************
 *                                                          *
 * operator += Strategies                                   *
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
    slist.plusOneByOne(strats2.slist);
  }
  else
  {
timersStrat[9].start();

    // Scrutinize doesn't help here, even for large strategies.
    slist.addStrategies(strats2.slist, 
      &Strategy::lessEqualPrimaryStudied);

timersStrat[9].stop();
  }
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

    // TODO Could make an Extensions method multiply with these
    // arguments.
    // Uses lessEqualPrimary and then compareSecondary.
    Extensions extensions;
    extensions.split(slist, strats2.slist.front(), 
      ranges, EXTENSION_SPLIT1);
    extensions.split(strats2.slist, slist.front(), 
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
 * Utilities                                                *
 *                                                          *
 ************************************************************/

bool Strategies::ordered() const
{
  return slist.ordered();
}


bool Strategies::minimal() const
{
  return slist.minimal();
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

