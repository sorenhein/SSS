/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <cassert>

#include "Distributions.h"


Distributions::Distributions()
{
  Distributions::reset();
}


void Distributions::reset()
{
  maxCards = 0;
  distMemory.reset();
  splitCounts.clear();
}


void Distributions::resize(const unsigned char maxCardsIn)
{
  maxCards = maxCardsIn;

  // If there are 13 (or 14) cards, then there are at most 
  // (maxCards+1) / 2 = 7 opposing ranks.
  productMemory.resize((maxCards+1)/2 + 1);

  distMemory.resize(maxCardsIn+1);

  // A distribution is a set of card splits for a given rank
  // vector covering both East and West.
  // There are 2 E-W distributions with 1 card: Either they have it
  // or they don't.

  // We count the total number of card splits for East and West.
  splitCounts.resize(maxCardsIn+1, 0);
}


/*
void Distributions::resizeStats(ExplStats& explStats) const
{
  distMemory.resizeStats(explStats);
}
*/


void Distributions::runSingle(
  const vector<set<unsigned>>& dependenciesCan,
  const vector<set<unsigned>>& dependenciesNoncan)
{
  distMemory.resizeSingle(dependenciesCan);

  for (unsigned char c = 0; c < dependenciesCan.size(); c++)
    for (auto& dep: dependenciesCan[c])
      distMemory.addCanonicalMT(productMemory, c, dep);

  for (unsigned char c = 0; c < dependenciesCan.size(); c++)
    for (auto& dep: dependenciesNoncan[c])
      distMemory.addNoncanonicalMT(c, dep);
}


void Distributions::runUniques(const unsigned char cards)
{
  for (unsigned holding = 0; holding < distMemory.size(cards); holding++)
  {
    const Distribution& dist = distMemory.addFullMT(
      productMemory, cards, holding);
    splitCounts[cards] += dist.size();
  }
}


void Distributions::runUniqueThread(
  const unsigned char cards,
  const unsigned thid)
{
  unsigned holding;
  const size_t counterMax = distMemory.size(cards);

  while (true)
  {
    holding = counter++; // Atomic
    if (holding >= counterMax)
      break;

    // Pass in thid?  Then less blocking.
    const Distribution& dist = distMemory.addFullMT(
      productMemory, cards, holding);
    threadSplitCounts[thid] += dist.size();
  }
}


void Distributions::runUniquesMT(
  const unsigned char cards,
  const unsigned numThreads)
{
  counter = 0;

  vector<thread *> threads;
  threads.resize(numThreads);

  threadSplitCounts.clear();
  threadSplitCounts.resize(numThreads);

  for (unsigned thid = 0; thid < numThreads; thid++)
    threads[thid] = new thread(&Distributions::runUniqueThread, 
      this, cards, thid);

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    threads[thid]->join();
    delete threads[thid];
  }

  for (unsigned thid = 0; thid < numThreads; thid++)
    splitCounts[cards] += threadSplitCounts[thid];
}


Distribution& Distributions::get(
  const unsigned char cards,
  const unsigned holding2)
{
  return distMemory.get(cards, holding2);
}


const Distribution& Distributions::get(
  const unsigned char cards,
  const unsigned holding2) const
{
  return distMemory.get(cards, holding2);
}


string Distributions::strDynamic() const
{
  return distMemory.strDynamic();
}


void Distributions::strLimits(
  const unsigned char cards,
  unsigned char& cmin,
  unsigned char& cmax) const
{
  if (cards == 0)
  {
    cmin = 1;
    cmax = maxCards;
  }
  else
  {
    cmin = cards;
    cmax = cards;
  }
}


string Distributions::str(const unsigned char cards) const
{
  unsigned char cmin, cmax;
  Distributions::strLimits(cards, cmin, cmax);

  stringstream ss;
  ss <<
    setw(5) << "Cards" <<
    setw(9) << "Count" <<
    setw(9) << "Dists" <<
    setw(9) << "Avg." <<
    setw(9) << "Uniques" <<
    "\n";

  for (unsigned char c = cmin; c <= cmax; c++)
  {
    assert(c < splitCounts.size() && c < distMemory.size(c));
    if (splitCounts[c] == 0)
      continue;

    ss <<
      setw(5) << +c <<
      setw(9) << distMemory.size(c) <<
      setw(9) << splitCounts[c] <<
      setw(9) << fixed << setprecision(2) <<
        static_cast<double>(splitCounts[c]) / 
        static_cast<double>(distMemory.size(c)) <<
      setw(9) << distMemory.used(c) <<
      "\n";
  }

  return ss.str() + "\n";
}


string Distributions::strCovers(
  const RanksNames& ranksNames,
  const unsigned char cards) const
{
  unsigned char cmin, cmax;
  Distributions::strLimits(cards, cmin, cmax);

  stringstream ss;
  for (unsigned char c = cmin; c <= cmax; c++)
    ss << distMemory.strCovers(c, ranksNames);

  return ss.str();
}


string Distributions::strCoverCounts(const unsigned char cards) const
{
  unsigned char cmin, cmax;
  Distributions::strLimits(cards, cmin, cmax);

  stringstream ss;
  ss << distMemory.strCoverCountsHeader();
  ss << distMemory.strCoverCounts(cmin, cmax);
  return ss.str() + "\n";
}


string Distributions::strProductEnterStats() const
{
  return productMemory.strEnterStats();
}

