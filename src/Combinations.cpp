#include <iostream>
#include <iomanip>
#include <sstream>

#include "Combinations.h"
#include "Ranks.h"

// http://oeis.org/A051450
const vector<int> UNIQUE_COUNT = 
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
  cout << "Combinations destructor" << endl;
}


void Combinations::reset()
{
  maxCards = 0;
  combinations.clear();
  uniques.clear();
  counts.clear();
}


void Combinations::resize(const int maxCardsIn)
{
  maxCards = maxCardsIn;

  combinations.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);

  // There are three combinations with 1 card: It may be with
  // North, South or the opponents.
  int numCombinations = 3;

  for (int cards = 1; cards <= maxCards; cards++)
  {
    combinations[cards].resize(numCombinations);
    numCombinations *= 3;

    uniques[cards].resize(UNIQUE_COUNT[cards]);
  }

  counts.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < counts.size(); cards++)
  {
    counts[cards].total = 0;
    counts[cards].unique = 0;
  }
}


void Combinations::runUniques(const int cards)
{
  vector<CombEntry>& combs = combinations[cards];
  vector<int>& uniqs = uniques[cards];
  Ranks ranks;

  ranks.resize(cards);
  int uniqueIndex = -1;

  for (unsigned holding = 0; holding < combs.size(); holding++)
  {
    ranks.set(holding, cards, combs[holding]);
// cout << ranks.str();

    counts[cards].total++;
    if (holding == combs[holding].canonicalHolding)
    {
      uniqueIndex++;
      combs[holding].canonicalIndex = uniqueIndex;
      uniqs[uniqueIndex] = holding;

      counts[cards].unique++;
    }
  }
}


void Combinations::runUniqueThread(
  const int cards,
  const int thid)
{
  vector<CombEntry>& combs = combinations[cards];
  vector<int>& uniqs = uniques[cards];

  Ranks ranks;
  ranks.resize(cards);
  unsigned holding;

  const unsigned counterMax = combs.size();

  while (true)
  {
    holding = counterHolding++; // Atomic
    if (holding >= counterMax)
      break;

    ranks.set(holding, cards, combs[holding]);

    threadCounts[thid].total++;
    if (holding == combs[holding].canonicalHolding)
    {
      const int uniqueIndex = counterUnique++; // Atomic

      combs[holding].canonicalIndex = uniqueIndex;
      uniqs[uniqueIndex] = holding;

      threadCounts[thid].unique++;
    }
  }
}


void Combinations::runUniquesMT(
  const int cards,
  const int numThreads)
{
  counterHolding = 0;
  counterUnique = 0;

  vector<thread *> threads;
  threads.resize(numThreads);

  threadCounts.clear();
  threadCounts.resize(numThreads);

  for (int thid = 0; thid < numThreads; thid++)
    threads[thid] = new thread(&Combinations::runUniqueThread, 
      this, cards, thid);

  for (int thid = 0; thid < numThreads; thid++)
  {
    threads[thid]->join();
    delete threads[thid];
  }

  for (int thid = 0; thid < numThreads; thid++)
  {
    counts[cards].total += threadCounts[thid].total;
    counts[cards].unique += threadCounts[thid].unique;
  }
}


string Combinations::strUniques(const int cards) const
{
  int cmin, cmax;
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
    setw(9) << "Total" <<
    setw(9) << "Unique" <<
    setw(9) << "%" <<
    "\n";

  for (int c = cmin; c <= cmax; c++)
  {
    if (counts[c].total == 0)
      continue;

    ss <<
      setw(5) << c <<
      setw(9) << counts[c].total <<
      setw(9) << counts[c].unique <<
      setw(8) << fixed << setprecision(2) <<
        (100. * counts[c].unique) / counts[c].total <<
        "%\n";
  }

  return ss.str() + "\n";
}

