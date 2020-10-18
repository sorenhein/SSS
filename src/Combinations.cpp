#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <cassert>

#include "Combinations.h"
#include "Plays.h"
#include "Ranks.h"

// http://oeis.org/A051450
const vector<unsigned> UNIQUE_COUNT = 
{
  1,      //  0
  2,      //  1
  5,      //  2
  12,     //  3
  30,     //  4
  76,     //  5
  195,    //  6
  504,    //  7
  1309,   //  8
  3410,   //  9
  8900,   // 10
  23256,  // 11
  60813,  // 12
  159094, // 13
  416325, // 14
  1089648 // 15
};


Combinations::Combinations()
{
  Combinations::reset();
}


Combinations::~Combinations()
{
}


void Combinations::reset()
{
  maxCards = 0;
  combinations.clear();
  uniques.clear();
  combCounts.clear();
  playCounts.clear();
}


void Combinations::resize(const unsigned maxCardsIn)
{
  maxCards = maxCardsIn;

  combinations.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);

  // There are three combinations with 1 card: It may be with
  // North, South or the opponents.
  unsigned numCombinations = 3;

  // for (unsigned cards = 1; cards <= maxCards; cards++)
  for (unsigned cards = 1; cards < combinations.size(); cards++)
  {
    combinations[cards].resize(numCombinations);
    numCombinations *= 3;

    assert(cards < UNIQUE_COUNT.size());
    uniques[cards].resize(UNIQUE_COUNT[cards]);
  }

  combCounts.resize(maxCardsIn+1);
  playCounts.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < combCounts.size(); cards++)
  {
    combCounts[cards].reset();
    playCounts[cards].reset();
  }
}


void Combinations::runUniques(const unsigned cards)
{
  assert(cards < combinations.size());
  assert(cards < uniques.size());
  assert(cards < combCounts.size());
  assert(cards < playCounts.size());

  vector<CombEntry>& combs = combinations[cards];
  vector<unsigned>& uniqs = uniques[cards];

  Ranks ranks;
  ranks.resize(cards);
  unsigned uniqueIndex = 0;

  Plays plays;
  plays.resize(cards);

  for (unsigned holding = 0; holding < combs.size(); holding++)
  {
// cout << "combs holding " << holding << endl;
    ranks.set(holding, combs[holding]);

    combCounts[cards].total++;
    if (holding == combs[holding].canonicalHolding3)
    {
      assert(uniqueIndex < uniqs.size());
      combs[holding].canonicalIndex = uniqueIndex;
      uniqs[uniqueIndex] = holding;
      uniqueIndex++;

      combCounts[cards].unique++;

      unsigned term;
      ranks.setPlays(plays, term);
      playCounts[cards].unique++;
      playCounts[cards].total += plays.size();
      plays.reset();
    }
  }
}


void Combinations::runUniqueThread(
  const unsigned cards,
  const unsigned thid)
{
  assert(cards < combinations.size());
  assert(cards < uniques.size());
  assert(thid < threadCombCounts.size());
  assert(thid < threadPlayCounts.size());

  vector<CombEntry>& combs = combinations[cards];
  vector<unsigned>& uniqs = uniques[cards];

  Ranks ranks;
  ranks.resize(cards);
  unsigned holding;

  Plays plays;
  plays.resize(cards);

  const unsigned counterMax = combs.size();

  while (true)
  {
    holding = counterHolding++; // Atomic
    if (holding >= counterMax)
      break;

    ranks.set(holding, combs[holding]);

    threadCombCounts[thid].total++;
    if (holding == combs[holding].canonicalHolding3)
    {
      const unsigned uniqueIndex = counterUnique++; // Atomic
      assert(uniqueIndex < uniqs.size());

      combs[holding].canonicalIndex = uniqueIndex;
      uniqs[uniqueIndex] = holding;

      threadCombCounts[thid].unique++;

      unsigned term;
      ranks.setPlays(plays, term);
      threadPlayCounts[thid].unique++;
      threadPlayCounts[thid].total += plays.size();
      plays.reset();
    }
  }
}


void Combinations::runUniquesMT(
  const unsigned cards,
  const unsigned numThreads)
{
  counterHolding = 0;
  counterUnique = 0;

  vector<thread *> threads;
  threads.resize(numThreads);

  threadCombCounts.clear();
  threadCombCounts.resize(numThreads);

  threadPlayCounts.clear();
  threadPlayCounts.resize(numThreads);

  for (unsigned thid = 0; thid < numThreads; thid++)
    threads[thid] = new thread(&Combinations::runUniqueThread, 
      this, cards, thid);

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    threads[thid]->join();
    delete threads[thid];
  }

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    combCounts[cards] += threadCombCounts[thid];
    playCounts[cards] += threadPlayCounts[thid];
  }
}


string Combinations::strUniques(const unsigned cards) const
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
    setw(9) << "Combos" <<
    setw(9) << "Unique" <<
    setw(9) << "%" <<
    setw(9) << "Plays" <<
    "\n";

  for (unsigned c = cmin; c <= cmax; c++)
  {
    assert(c < combCounts.size());
    if (combCounts[c].total == 0)
      continue;

    ss <<
      setw(5) << c <<
      setw(9) << combCounts[c].total <<
      setw(9) << combCounts[c].unique <<
      setw(8) << fixed << setprecision(2) <<
        (100. * combCounts[c].unique) / combCounts[c].total << "%" << 
      setw(9) << fixed << setprecision(2) <<
        playCounts[c].total / static_cast<double>(playCounts[c].unique) <<
        "\n";
  }

  return ss.str() + "\n";
}

