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

  slist.setTrivial(trivial, len);
  ranges.reset();
}


void Strategies::adapt(
  const Play& play,
  const Survivors& survivors)
{
timersStrat[0].start();

  slist.adapt(play, survivors);

timersStrat[0].stop();
}


/************************************************************
 *                                                          *
 * Reactivating and scrutinizing (internal)                 *
 *                                                          *
 ************************************************************/

void Strategies::reactivate(
  const Strategy& simpleStrat,
  const Strategy& constants)
{
timersStrat[1].start();

  if (simpleStrat.empty())
  {
    if (! constants.empty())
    {
      // Constants don't throw off the consolidation.
      // The method argument is unused as we don't consolidate.
      slist.multiply(constants, &Strategy::lessEqualPrimaryStudied, false);
    }
  }
  else
  {
    Strategy st = simpleStrat;
    if (! constants.empty())
      st *= constants;

    slist.multiply(st, &Strategy::lessEqualPrimaryStudied);
  }

timersStrat[1].stop();
}


void Strategies::scrutinize(const Ranges& rangesIn)
{
timersStrat[2].start();

  for (auto& strategy: slist)
    strategy.scrutinize(rangesIn);

timersStrat[2].stop();
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
timersStrat[3].start();

    // Scrutinize doesn't help here, even for large strategies.
    slist.addStrategies(strats2.slist, 
      &Strategy::lessEqualPrimaryStudied);

timersStrat[3].stop();
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
  // It is essential that if this->ranges is non-empty, it is a 
  // valid range for both this->slist and for strats2->slist.  
  // This method does not change any of these ranges.

  const unsigned len1 = slist.size();
  const unsigned len2 = strats2.slist.size();

  if (len2 == 0)
    return;
  else if (len1 == 0)
  {
    // As * this is a parent node, preserve its ranges.
    slist = strats2.slist;
    return;
  }
  else if (len1 == 1 && len2 == 1)
  {
timersStrat[4].start();

    slist.multiplyOneByOne(strats2.slist);

timersStrat[4].stop();
    return;
  }

  if (ranges.empty() || len1 < 10 || len2 < 10)
  {
timersStrat[5].start();

    // This implementation of the general product reduces
    // memory overhead.  The temporary product is formed in the last
    // element of Strategies as a scratch pad.  If it turns out to be
    // viable, it is already in Strategies and subject to move semantics.

    if (ranges.empty())
    {
      for (auto& strategies: slist)
        strategies.study();

      for (auto& strategies: strats2.slist)
        strategies.study();

      slist.multiply(strats2.slist, ranges, 
        &Strategy::lessEqualPrimaryStudied);
    }
    else
    {
      // Use the existing ranges, presumed correct.
      Strategies::scrutinize(ranges);
      strats2.scrutinize(ranges);

      slist.multiply(strats2.slist, ranges, 
        &Strategy::lessEqualPrimaryScrutinized);
    }


/*
    ComparatorType lessEqualMethod =
      &Strategy::lessEqualPrimaryScrutinized;

// cout << ranges.str("pre-existing parent (easy branch)") << endl;
// cout << strats2.ranges.str("pre-existing child (easy branch)") << endl;

    // TODO Just to make it work.  Slow?
Ranges rtmp;
if (ranges.empty())
{
Strategies stmp1 = * this;
Strategies stmp2 = strats2;
stmp1.makeRanges();
stmp2.makeRanges();
stmp1.propagateRanges(stmp2);
rtmp = stmp1.ranges;

cout << "Made a temp ranges";
cout << rtmp.str("tmp");

cout << stmp1.str("stmp1");
cout << stmp2.str("stmp2");

    // Strategies::makeRanges();
    // strats2.makeRanges();
    // Strategies::propagateRanges(strats2);
}
else
{
  rtmp = ranges;
// cout << "Kept the input ranges\n";
// cout << rtmp.str("input");
}

    Strategies::scrutinize(rtmp);
    strats2.scrutinize(rtmp);

    slist.multiply(strats2.slist, rtmp, lessEqualMethod);

    **
    Strategies::scrutinize(ranges);
    strats2.scrutinize(ranges);

    slist.multiply(strats2.slist, ranges, lessEqualMethod);
    */

    /* */

    /*
    for (auto& strategies: slist)
      strategies.study();

    for (auto& strategies: strats2.slist)
      strategies.study();

    slist.multiply(strats2.slist, ranges, 
      &Strategy::lessEqualPrimaryStudied);
      */

timersStrat[5].stop();
    return;
  }
  else
  {
timersStrat[6].start();

    // The two Strategies have distributions that are overlapping 
    // as well as distributions that are unique to each of them.  
    // We split these out, and we pre-compare within each Strategies.  
    //
    // Even though Extensions splits Strategy's into own and shared
    // by distribution, we can still share the central ranges.
    //
    // TODO Is it possible that the ranges were already set
    // during purgeRanges?  Can we tell by whether ranges are
    // non-zero?

// cout << ranges.str("pre-existing parent (hard branch)") << endl;
// cout << strats2.ranges.str("pre-existing child (hard branch)") << endl;

    /* */
    // Strategies::makeRanges();
// cout << ranges.str("new parent");
    // strats2.makeRanges();
// cout << strats2.ranges.str("new child");
    // Strategies::propagateRanges(strats2);
// cout << ranges.str("propagated parent parent");

/* */

    Extensions extensions;
    extensions.split(slist, strats2.slist, ranges);
    extensions.multiply(ranges);

    extensions.flatten(slist);

timersStrat[6].stop();
  }
}


/************************************************************
 *                                                          *
 * Ranges methods                                           *
 *                                                          *
 ************************************************************/

void Strategies::makeRanges()
{
timersStrat[7].start();

  slist.makeRanges(ranges);

timersStrat[7].stop();
}


void Strategies::propagateRanges(const Strategies& child)
{
timersStrat[8].start();

  // This propagates the child's ranges to the current parent ranges.
  // The distribution number has to match.

  ranges*= child.ranges;

timersStrat[8].stop();
}


bool Strategies::purgeRanges(
  Strategy& constants,
  const Ranges& rangesParent,
  const bool debugFlag)
{
  if (slist.empty())
    return false;

timersStrat[9].start();

  const bool eraseFlag = slist.purgeRanges(constants,
    ranges, rangesParent, debugFlag);

  if (eraseFlag && debugFlag)
  {
    cout << constants.str("\nNew constants", true) << "\n";
    cout << Strategies::str("Ranges after purging", true);
  }

timersStrat[9].stop();

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
  return slist.resultLowest();
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

