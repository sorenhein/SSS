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

// Number of distributions is:
//
//   1:       3
//   2:       8
//   3:      21
//   4:      55
//   5:     144
//   6:     377
//   7:     987
//   8:    2584
//   9:    6765
//  10:   17711
//  11:   46368
//  12:  121383
//  13:  317811
//  14:  832040
//  15: 2178409
//
//  These are Fibonacci numbers.  If f(0) = 1, f(1) = 1, f(2) = 2, ...
//  then dist(n) = f(2*n+1).
//  There are a lot of possible interpretations on
//  http://oeis.org/A001906 and http://oeis.org/A088305.


Distributions::Distributions()
{
  Distributions::reset();
}


void Distributions::reset()
{
  maxCards = 0;
  distributions.clear();
  counts.clear();
  uniques.clear();
}


void Distributions::resize(const unsigned maxCardsIn)
{
  maxCards = maxCardsIn;

  distributions.resize(maxCardsIn+1);

  // A distribution is a set of card splits for a given rank
  // vector covering both East and West.
  // There are 2 E-W distributions with 1 card: Either they have it
  // or they don't.
  unsigned numDistributions = 1;

  for (unsigned cards = 0; cards < distributions.size(); cards++)
  {
    distributions[cards].resize(numDistributions);
    numDistributions <<= 1;
  }

  // We count the total number of card splits for East and West.
  counts.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < counts.size(); cards++)
  {
    counts[cards] = 0;
    uniques[cards] = 0;
  }
}


void Distributions::runUniques(const unsigned cards)
{
  assert(cards < distributions.size());
  assert(cards < counts.size());
  assert(cards < uniques.size());

  vector<Distribution>& dists = distributions[cards];

  for (unsigned holding = 0; holding < dists.size(); holding++)
  {
    dists[holding].setRanks(cards, holding);

    DistID distID = dists[holding].getID();
    if (distID.cards == cards && distID.holding == holding)
    {
      uniques[cards]++;
      dists[holding].splitAlternative();
      dists[holding].setLookups();
    }
    else
      dists[holding].setPtr(&distributions[distID.cards][distID.holding]);

    counts[cards] += dists[holding].size();
  }
}


void Distributions::runUniqueThread(
  const unsigned cards,
  const unsigned thid)
{
  assert(cards < distributions.size());
  assert(thid < threadCounts.size());
  assert(thid < threadUniques.size());

  vector<Distribution>& dists = distributions[cards];

  unsigned holding;

  const unsigned counterMax = dists.size();

  while (true)
  {
    holding = counter++; // Atomic
    if (holding >= counterMax)
      break;

    dists[holding].setRanks(cards, holding);

    // dists[holding].splitAlternative();
    // threadCounts[thid] += dists[holding].size();

    DistID distID = dists[holding].getID();
    if (distID.cards == cards && distID.holding == holding)
    {
      threadUniques[thid]++;
      dists[holding].splitAlternative();
      dists[holding].setLookups();
    }
    else
      dists[holding].setPtr(&distributions[distID.cards][distID.holding]);

    threadCounts[thid] += dists[holding].size();
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


Distribution const * Distributions::ptrCanonical(
  const unsigned cards,
  const unsigned holding2) const
{
  const Distribution& dist = distributions[cards][holding2];

  Distribution const * p = dist.getPtr();
  if (p == nullptr)
{
cout << "distributions ptr(" << cards << ", " << holding2 << ") is canonical\n";
    // If there is a null pointer, it was a canonical distribution.
    return &dist;
}
  else
{
cout << "distributions ptr(" << cards << ", " << holding2 << ") isn't canonical\n";
   // Otherwise it wasn't.
   return p;
}
}


Distribution const * Distributions::ptrNoncanonical(
  const unsigned cards,
  const unsigned holding2) const
{
  return &distributions[cards][holding2];
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
    setw(9) << "Uniques" <<
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
      setw(9) << uniques[c] <<
      "\n";
  }

  return ss.str() + "\n";
}

