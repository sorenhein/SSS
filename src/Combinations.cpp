#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <cassert>

#include "Combinations.h"
#include "CombEntry.h"
#include "Distributions.h"

#include "plays/Plays.h"

#include "ranks/Ranks.h"

#include "utils/Timers.h"

extern Timers timers;

// TMP
vector<Timer> timersStrat;


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
  combEntries.clear();
  uniques.clear();
  combCounts.clear();
  playCounts.clear();
  stratCounts.clear();

  timersStrat.clear();
}


void Combinations::resize(const unsigned maxCardsIn)
{
  maxCards = maxCardsIn;

  combEntries.resize(maxCardsIn+1);
  uniques.resize(maxCardsIn+1);

  // There are three combinations with 1 card: It may be with
  // North, South or the opponents.
  unsigned numCombinations = 1;

  // for (unsigned cards = 1; cards <= maxCards; cards++)
  for (unsigned cards = 0; cards < combEntries.size(); cards++)
  {
    combEntries[cards].resize(numCombinations);
    numCombinations *= 3;

    assert(cards < UNIQUE_COUNT.size());
    uniques[cards].resize(UNIQUE_COUNT[cards]);
  }

  combCounts.resize(maxCardsIn+1);
  playCounts.resize(maxCardsIn+1);
  stratCounts.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < combCounts.size(); cards++)
  {
    combCounts[cards].reset();
    playCounts[cards].reset();
    stratCounts[cards].reset();
  }

  timersStrat.resize(40);
  timersStrat[ 0].name("Strats::adapt");
  timersStrat[ 1].name("Strats::reactivate");
  timersStrat[ 2].name("Strats::scrutinize");
  timersStrat[ 3].name("Strats::+= strats (gen.)");
  timersStrat[ 4].name("Strats::*= strats (1*1)");
  timersStrat[ 5].name("Strats::*= strats (<10)");
  timersStrat[ 6].name("Strats::*= strats (>=10)");
  timersStrat[ 7].name("Strats::makeRanges");
  timersStrat[ 8].name("Strats::propagateRanges");
  timersStrat[ 9].name("Strats::purgeRanges");
}


void Combinations::runSpecific(
  const unsigned cards,
  const unsigned holding,
  const Distributions& distributions)
{
  assert(cards < combEntries.size());

  Ranks ranks;
  ranks.resize(cards);
  
  Plays plays;
  plays.resize(cards);

  CombEntry& centry = combEntries[cards][holding];
  ranks.set(holding, centry);

  const unsigned canonicalHolding3 = centry.canonicalHolding3;
  if (holding != canonicalHolding3)
  {
    cout << "Specific (cards, holding) = (" << cards << ", " <<
      holding << ") is not canonical." << endl;
    return;
  }

  cout << ranks.strTable();

  Combination comb;
  comb.strategize(centry, * this, distributions, ranks, plays, true);
}


void Combinations::runSpecificVoid(
  const unsigned cards,
  const unsigned holding,
  const Distributions& distributions)
{
  assert(cards < combEntries.size());

  Ranks ranks;
  ranks.resize(cards);
  
  Plays plays;
  plays.resize(cards);

  CombEntry& centry = combEntries[cards][holding];
  ranks.set(holding, centry);

  const unsigned canonicalHolding3 = centry.canonicalHolding3;
  if (holding != canonicalHolding3)
  {
    cout << "Specific (cards, holding) = (" << cards << ", " <<
      holding << ") is not canonical." << endl;
    return;
  }

  cout << ranks.strTable();
  wcout << ranks.wstrDiagram();

  Combination comb;
  comb.strategizeVoid(centry, * this, distributions, ranks, plays, true);
}


void Combinations::runUniques(
  const unsigned cards,
  const Distributions& distributions)
{
  assert(cards < combEntries.size());
  assert(cards < uniques.size());
  assert(cards < combCounts.size());
  assert(cards < playCounts.size());
  assert(cards < stratCounts.size());

  vector<CombEntry>& centries = combEntries[cards];
  vector<Combination>& uniqs = uniques[cards];

  Ranks ranks;
  ranks.resize(cards);
  unsigned uniqueIndex = 0;

  Plays plays;
  plays.resize(cards);

  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    CombEntry& centry = centries[holding];

    timers.start(TIMER_RANKS);
    ranks.set(holding, centry);
    timers.stop(TIMER_RANKS);

    combCounts[cards].total++;
    const unsigned canonicalHolding3 = centry.canonicalHolding3;
    if (holding == canonicalHolding3)
    {
      combCounts[cards].unique++;

      assert(uniqueIndex < uniqs.size());
      centry.canonicalIndex = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];
      uniqueIndex++;

      // Plays is cleared and rewritten, so it is only an optimization
      // not to let Combination make its own plays.

      comb.strategize(centry, * this, distributions, ranks, plays);

      playCounts[cards].unique++;
      playCounts[cards].total += plays.size();

      stratCounts[cards].unique++;
      stratCounts[cards].total += comb.strategies().size();
    }
    else
      centry.canonicalIndex = centries[canonicalHolding3].canonicalIndex;
  }

  // TMP Print timers
  cout << "Individual timers\n";
  Timer sum;
  for (unsigned i = 0; i < timersStrat.size(); i++)
  {
    string s = timersStrat[i].str(2);
    if (s == "")
      continue;
    s = s.substr(0, s.size()-1);
    sum += timersStrat[i];
    cout << setw(4) << i << s << "\n";
  }
  cout << string(70, '-') << "\n";
  cout << setw(4) << "" << sum.str(2) << endl;

}


void Combinations::runUniqueThread(
  const unsigned cards,
  Distributions const * distributions,
  const unsigned thid)
{
  assert(cards < combEntries.size());
  assert(cards < uniques.size());
  assert(thid < threadCombCounts.size());
  assert(thid < threadPlayCounts.size());

  vector<CombEntry>& centries = combEntries[cards];
  vector<Combination>& uniqs = uniques[cards];

  Ranks ranks;
  ranks.resize(cards);
  unsigned holding;

  Plays plays;
  plays.resize(cards);

  const unsigned counterMax = centries.size();

  while (true)
  {
    holding = counterHolding++; // Atomic
    if (holding >= counterMax)
      break;

    CombEntry& centry = centries[holding];
    ranks.set(holding, centry);

    threadCombCounts[thid].total++;
    const unsigned canonicalHolding3 = centry.canonicalHolding3;
    if (holding == canonicalHolding3)
    {
      threadCombCounts[thid].unique++;

      const unsigned uniqueIndex = counterUnique++; // Atomic
      assert(uniqueIndex < uniqs.size());
      centry.canonicalIndex = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];

      comb.strategize(centry, * this, * distributions, ranks, plays);

      threadPlayCounts[thid].unique++;
      threadPlayCounts[thid].total += plays.size();
    }
    else
    {
      centry.canonicalIndex = centries[canonicalHolding3].canonicalIndex;
    }
  }
}


void Combinations::runUniquesMT(
  const unsigned cards,
  const Distributions& distributions,
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

  threadStratCounts.clear();
  threadStratCounts.resize(numThreads);

  for (unsigned thid = 0; thid < numThreads; thid++)
    threads[thid] = new thread(&Combinations::runUniqueThread, 
      this, cards, &distributions, thid);

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    threads[thid]->join();
    delete threads[thid];
  }

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    combCounts[cards] += threadCombCounts[thid];
    playCounts[cards] += threadPlayCounts[thid];
    stratCounts[cards] += threadStratCounts[thid];
  }
}


Combination const * Combinations::getPtr(
  const unsigned cards,
  const unsigned holding3) const
{
  const unsigned ui = combEntries[cards][holding3].canonicalIndex;
  return &uniques[cards][ui];
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
    setw(9) << "Strats" <<
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
      setw(9) << fixed << setprecision(2) <<
        stratCounts[c].total / static_cast<double>(stratCounts[c].unique) <<
        "\n";
  }

  return ss.str() + "\n";
}

