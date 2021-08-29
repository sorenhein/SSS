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
  countStats.clear();

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

  countStats.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < maxCardsIn+1; cards++)
    countStats[cards].reset();

  timersStrat.resize(40);
  timersStrat[ 0].name("Strats::adapt");
  timersStrat[ 1].name("Strats::reactivate");
  timersStrat[ 2].name("Strats::scrutinize");
  timersStrat[ 3].name("Strats::+= strats (gen.)");
  timersStrat[ 4].name("Strats::*= strats (1*1)");
  timersStrat[ 5].name("Strats::*= strats (study)");
  timersStrat[ 6].name("Strats::*= strats (<10)");
  timersStrat[ 7].name("Strats::*= strats (>=10)");
  timersStrat[ 8].name("Strats::makeRanges");
  timersStrat[ 9].name("Strats::propagateRanges");
  timersStrat[10].name("Strats::purgeRanges");
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

  const unsigned canonicalHolding3 = centry.canonical.holding3;
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

  const unsigned canonicalHolding3 = centry.canonical.holding3;
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



#include "const.h"
bool Combinations::getMinimals(
  const Strategies& strategies,
  const Ranks& ranks,
  list<CombReference>& minimals) const
{
  Result resultLowest;
  strategies.getResultLowest(resultLowest);

  // TODO
  // Size of minimals is size of winners
  // For each winner in winners
  //   iter to winner, iter to minimals
  //   ranks.minimalEntry(winner, CombReference& to minimals entry)
  // Sort them?

  UNUSED(ranks);
  UNUSED(minimals);
  return false;
}


void Combinations::runUniques(
  const unsigned cards,
  const Distributions& distributions)
{
  assert(cards < combEntries.size());
  assert(cards < uniques.size());
  assert(cards < countStats.size());

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

    const unsigned canonicalHolding3 = centry.canonical.holding3;
    if (holding == canonicalHolding3)
    {
      assert(uniqueIndex < uniqs.size());
      centry.canonical.index = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];
      uniqueIndex++;

      // Plays is cleared and rewritten, so it is only an optimization
      // not to let Combination make its own plays.

      comb.strategize(centry, * this, distributions, ranks, plays);

      countStats[cards].plays.unique += plays.size();
      countStats[cards].strats.unique += comb.strategies().size();

      centry.minimalFlag =
        Combinations::getMinimals(comb.strategies(), ranks, centry.minimals);

      if (centry.minimalFlag)
      {
        countStats[cards].plays.minimal += plays.size();
        countStats[cards].strats.minimal += comb.strategies().size();
      }
    }
    else
      centry.canonical.index = centries[canonicalHolding3].canonical.index;
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
  assert(thid < threadCountStats.size());

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

    const unsigned canonicalHolding3 = centry.canonical.holding3;
    if (holding == canonicalHolding3)
    {
      const unsigned uniqueIndex = counterUnique++; // Atomic
      assert(uniqueIndex < uniqs.size());
      centry.canonical.index = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];

      comb.strategize(centry, * this, * distributions, ranks, plays);

      threadCountStats[thid].plays.unique += plays.size();
      threadCountStats[thid].strats.unique += comb.strategies().size();

      centry.minimalFlag =
        Combinations::getMinimals(comb.strategies(), ranks, centry.minimals);

      if (centry.minimalFlag)
      {
        threadCountStats[thid].plays.minimal += plays.size();
        threadCountStats[thid].strats.minimal += comb.strategies().size();
      }
    }
    else
      centry.canonical.index = centries[canonicalHolding3].canonical.index;
  }
}


void Combinations::runUniquesMT(
  const unsigned cards,
  const Distributions& distributions,
  const unsigned numThreads)
{
  // TODO Untested since changes to statistics gathering.

  counterHolding = 0;
  counterUnique = 0;

  vector<thread *> threads;
  threads.resize(numThreads);

  threadCountStats.clear();
  threadCountStats.resize(numThreads);

  for (unsigned thid = 0; thid < numThreads; thid++)
    threads[thid] = new thread(&Combinations::runUniqueThread, 
      this, cards, &distributions, thid);

  for (unsigned thid = 0; thid < numThreads; thid++)
  {
    threads[thid]->join();
    delete threads[thid];
  }

  for (unsigned thid = 0; thid < numThreads; thid++)
    countStats[cards] += threadCountStats[thid];
}


Combination const * Combinations::getPtr(
  const unsigned cards,
  const unsigned holding3) const
{
  const unsigned ui = combEntries[cards][holding3].canonical.index;
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
    setw(8) << "Unique" <<
    setw(8) << "Plays" <<
    setw(8) << "Strats" <<
    setw(8) << "Minimal" <<
    setw(8) << "Plays" <<
    setw(8) << "Strats" <<
    "\n";

  for (unsigned c = cmin; c <= cmax; c++)
  {
    assert(c < maxCards+1);
    if (countStats[c].plays.unique.count == 0)
      continue;

    ss <<
      setw(5) << c <<
      setw(9) << combEntries[c].size() <<
      setw(8) << countStats[c].plays.unique.count <<
      setw(8) << countStats[c].plays.unique.strAverage() <<
      setw(8) << countStats[c].strats.unique.strAverage();

    if (countStats[c].plays.minimal.count > 0)
      ss <<
      setw(8) << countStats[c].plays.minimal.count <<
      setw(8) << countStats[c].plays.minimal.strAverage() <<
      setw(8) << countStats[c].strats.minimal.strAverage();

    ss << "\n";
  }

  return ss.str() + "\n";
}

