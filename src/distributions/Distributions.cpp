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
  counts.clear();
  uniques.clear();
}


void Distributions::resize(const unsigned maxCardsIn)
{
  maxCards = maxCardsIn;
  distMemory.resize(maxCardsIn+1);

  // A distribution is a set of card splits for a given rank
  // vector covering both East and West.
  // There are 2 E-W distributions with 1 card: Either they have it
  // or they don't.

  // We count the total number of card splits for East and West.
  counts.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < counts.size(); cards++)
  {
    counts[cards] = 0;
    uniques[cards] = 0;
  }
}


void Distributions::add(
  const unsigned cards,
  const unsigned holding)
{
  distMemory.add(cards, holding);
}


void Distributions::runUniques(const unsigned cards)
{
  for (unsigned holding = 0; holding < distMemory.size(cards); holding++)
    distMemory.add(cards, holding);
}


void Distributions::runUniqueThread(
  const unsigned cards,
  [[maybe_unused]] const unsigned thid)
{
  unsigned holding;
  const unsigned counterMax = distMemory.size(cards);

  while (true)
  {
    holding = counter++; // Atomic
    if (holding >= counterMax)
      break;

    // Pass in thid?  Then less blocking.
    distMemory.add(cards, holding);
  }
}


void Distributions::runUniquesMT(
  const unsigned cards,
  const unsigned numThreads)
{
  counter = 0;

  vector<thread *> threads;
  threads.resize(numThreads);

  threadCounts.clear();
  threadCounts.resize(numThreads);

  threadUniques.clear();
  threadUniques.resize(numThreads);

  for (unsigned thid = 0; thid < numThreads; thid++)
    threads[thid] = new thread(&Distributions::runUniqueThread, 
      this, cards, thid);

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    threads[thid]->join();
    delete threads[thid];
  }

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    counts[cards] += threadCounts[thid];
    uniques[cards] += threadUniques[thid];
  }
}


const Distribution& Distributions::get(
  const unsigned cards,
  const unsigned holding2) const
{
  return distMemory.get(cards, holding2);
}


string Distributions::str(const unsigned cards) const
{
  return distMemory.str(cards);
}

