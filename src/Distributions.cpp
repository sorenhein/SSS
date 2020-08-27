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


Distributions::~Distributions()
{
}


void Distributions::reset()
{
  maxCards = 0;
  distributions.clear();
  counts.clear();
}


void Distributions::resize(const unsigned maxCardsIn)
{
  maxCards = maxCardsIn;

  distributions.resize(maxCardsIn+1);

  // A distribution is a set of card splits for a given rank
  // vector covering both East and West.
  // There are 2 E-W distributions with 1 card: Either they have it
  // or they don't.
  unsigned numDistributions = 2;

  for (unsigned cards = 1; cards < distributions.size(); cards++)
  {
    distributions[cards].resize(numDistributions);
    numDistributions <<= 1;
  }

  // We count the total number of card splits for East and West.
  counts.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < counts.size(); cards++)
    counts[cards] = 0;
}


void Distributions::runUniques(const unsigned cards)
{
  assert(cards < distributions.size());
  assert(cards < counts.size());

  vector<Distribution>& dists = distributions[cards];

  for (unsigned holding = 0; holding < dists.size(); holding++)
    counts[cards] += dists[holding].set(cards, holding);
}


void Distributions::runUniqueThread(
  const unsigned cards,
  const unsigned thid)
{
  assert(cards < distributions.size());
  assert(thid < threadCounts.size());

  vector<Distribution>& dists = distributions[cards];

  unsigned holding;

  const unsigned counterMax = dists.size();

  while (true)
  {
    holding = counter++; // Atomic
    if (holding >= counterMax)
      break;

    threadCounts[thid] += dists[holding].set(cards, holding);
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

  for (unsigned thid = 0; thid < numThreads; thid++)
    threads[thid] = new thread(&Distributions::runUniqueThread, 
      this, cards, thid);

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    threads[thid]->join();
    delete threads[thid];
  }

  for (unsigned thid = 0; thid < numThreads; thid++)
    counts[cards] += threadCounts[thid];
}


string Distributions::str(const unsigned cards) const
{
  unsigned cmin, cmax;
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

  stringstream ss;
  ss <<
    setw(5) << "Cards" <<
    setw(9) << "Count" <<
    setw(9) << "Dists" <<
    setw(9) << "Avg." <<
    "\n";

  for (unsigned c = cmin; c <= cmax; c++)
  {
    assert(c < counts.size() && c < distributions.size());
    if (counts[c] == 0)
      continue;

    ss <<
      setw(5) << c <<
      setw(9) << distributions[c].size() <<
      setw(9) << counts[c] <<
      setw(9) << fixed << setprecision(2) <<
        static_cast<double>(counts[c]) / distributions[c].size() << 
      "\n";
  }

  return ss.str() + "\n";
}

