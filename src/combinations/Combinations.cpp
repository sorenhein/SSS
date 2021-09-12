#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <cassert>

#include "Combinations.h"
#include "CombFiles.h"
#include "CombEntry.h"

#include "../distributions/Distributions.h"

#include "../plays/Plays.h"

#include "../ranks/Ranks.h"

#include "../inputs/Control.h"
#include "../utils/Timers.h"

extern Control control;
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


void Combinations::reset()
{
  maxCards = 0;
  combEntries.clear();
  uniques.clear();
  countStats.clear();
  countNoncanonical.clear();

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
  {
    countStats[cards].data.resize(COMB_SIZE);
    countStats[cards].data.resize(COMB_SIZE);
  }

  countNoncanonical.resize(maxCardsIn+1);

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
  ranks.setRanks(holding, centry);

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
  ranks.setRanks(holding, centry);

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


bool Combinations::getMinimals(
  const Strategies& strategies,
  const Ranks& ranks,
  list<CombReference>& minimals) const
{
  // Returns true when the combination is already minimal,
  // in which case the set remains empty.

  Result resultLowest;
  strategies.getResultLowest(resultLowest);
  return ranks.getMinimals(resultLowest, minimals);
}


CombinationType Combinations::classify(
  const bool minimalFlag,
  const Strategies& strategies,
  const Ranks& ranks) const
{
  if (minimalFlag)
  {
    if (strategies.constantTricks())
    {
      // Could have multiple strategy's differing by rank.
      return COMB_CONSTANT;
    }
    else if (strategies.size() == 1 || strategies.sameTricks())
      return COMB_SINGLE_STRAT;
    else if (ranks.partnerVoid())
    {
      // TODO Recognize symmetrical
      return COMB_MULT_VOID_ASYMM;
    }
    else
      return COMB_MULT_NON_VOID;
  }
  else
    return COMB_NON_MINIMAL;
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
    ranks.setRanks(holding, centry);
    timers.stop(TIMER_RANKS);

    const unsigned canonicalHolding3 = centry.canonical.holding3;
    if (holding == canonicalHolding3)
    {
      // TODO Does canonicalFlag actually get set? Should it be?
      // Could we even be solving the rotations as well?
      assert(uniqueIndex < uniqs.size());
      centry.canonical.index = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];
      uniqueIndex++;

      // Plays is cleared and rewritten, so it is only an optimization
      // not to let Combination make its own plays.

      comb.strategize(centry, * this, distributions, ranks, plays);

      centry.minimalFlag =
        Combinations::getMinimals(comb.strategies(), ranks, centry.minimals);

      centry.type = Combinations::classify(
        centry.minimalFlag, comb.strategies(), ranks);

      countStats[cards].data[centry.type].incr(
        plays.size(), comb.strategies().size());
      
      if (! centry.minimalFlag)
      {
        // TODO Control by some flag
        Combinations::checkReductions(cards, centry, comb.strategies(), 
          * distributions.ptrNoncanonical(cards, centry.canonical.holding2));
      }
    }
    else
    {
      centry.canonical.index = centries[canonicalHolding3].canonical.index;
      countNoncanonical[cards]++;
    }
  }

  timersStrat[30].start();
  Combinations::checkAllMinimals(cards);
  timersStrat[30].stop();

  // This is how to write files:
  // CombFiles combFiles;
  // combFiles.writeFiles(cards, centries);

  // This is how to read files:
  // vector<CombEntry> copy;
  // copy.resize(centries.size());
  // combFiles.readFiles(cards, copy);

  // This is how to compare read and write.
  // for (unsigned holding = 0; holding < centries.size(); holding++)
    // assert(centries[holding] == copy[holding]);

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
    ranks.setRanks(holding, centry);

    const unsigned canonicalHolding3 = centry.canonical.holding3;
    if (holding == canonicalHolding3)
    {
      const unsigned uniqueIndex = counterUnique++; // Atomic
      assert(uniqueIndex < uniqs.size());
      centry.canonical.index = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];

      comb.strategize(centry, * this, * distributions, ranks, plays);

      centry.minimalFlag =
        Combinations::getMinimals(comb.strategies(), ranks, centry.minimals);

      centry.type = Combinations::classify(
        centry.minimalFlag, comb.strategies(), ranks);

      threadCountStats[thid].data[centry.type].incr(
        plays.size(), comb.strategies().size());
    }
    else
    {
      centry.canonical.index = centries[canonicalHolding3].canonical.index;
      threadCountNoncanonical[thid]++;
    }
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

  threadCountNoncanonical.clear();
  threadCountNoncanonical.resize(numThreads);

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
    countStats[cards] += threadCountStats[thid];
    threadCountNoncanonical[cards] += threadCountNoncanonical[thid];
  }
}


Combination const * Combinations::getPtr(
  const unsigned cards,
  const unsigned holding3) const
{
  const unsigned ui = combEntries[cards][holding3].canonical.index;
  return &uniques[cards][ui];
}


bool Combinations::checkMinimals(
  const vector<CombEntry>& centries,
  const list<CombReference>& minimals) const
{
  for (auto& min: minimals)
    if (! centries[min.holding3].minimalFlag)
      return false;

  return true;
}


void Combinations::checkReductions(
  const unsigned cards,
  const CombEntry& centry,
  const Strategies& strategies,
  const Distribution& distribution) const
{
  // Get the reduction that underlies the whole method.
  // TODO Could store the rank in CombEntry?
  Result resultLowest;
  strategies.getResultLowest(resultLowest);
  const unsigned char rankCritical = resultLowest.rank();
  const auto& reduction = distribution.getReduction(rankCritical);

  // Delete Strategy's where the number of tricks is not constant
  // within each reduction group.  The number of distributions is
  // unchanged.
  Strategies strategiesReduced = strategies;
  strategiesReduced.reduceByTricks(reduction);

  list<Strategies> strategiesExpanded;

  for (auto& min: centry.minimals)
  {
    const auto& ceMin = combEntries[cards][min.holding3];

    if (! ceMin.minimalFlag)
    {
      // This should not happen long-term, and short-term it is
      // addressed in checkMinimals().
      continue;
    }

    strategiesExpanded.emplace_back(Strategies());
    Strategies& strategiesMin = strategiesExpanded.back();

    // Get the strategies for the minimal reference.
    // TODO Assert can be erased later
    assert(ceMin.canonical.index < uniques[cards].size());
    strategiesMin = uniques[cards][ceMin.canonical.index].strategies();

    // Expand the strategies up using the reduction.
    strategiesMin.expand(reduction, min.rotateFlag);
  }

  // TODO Checks:
  // * The sum of the minimal winners should be the overall winner.
  //   In other words, the minimal winner should be what the top says.
  // * Each minimal strategy should agree in tricks with the top one.
  // * The minimal ones should merge (strategy by strategy) into
  //   the top one, including ranks in some sense.
}


bool Combinations::checkAndFixMinimals(
  const vector<CombEntry>& centries,
  list<CombReference>& minimals) const
{
  // Check that each non-minimal holding refers to minimal ones.
  // We actually follow through and change the minimals.
  // Once ranks are good, this method should no longer be needed.

  bool changeFlag = false;
  auto iter = minimals.begin();

  while (iter != minimals.end())
  {
    const CombEntry& centry = centries[iter->holding3];

    if (centry.minimalFlag)
      iter++;
    else
    {
      // Erase the non-minimal one and add the ones it points to.
      // Take into account the rotation flag -- we want the product
      // of all rotations to the really minimal holding.
      for (auto& min: centry.minimals)
      {
        minimals.push_back(min);
        CombReference& cr = minimals.back();
        cr.rotateFlag ^= iter->rotateFlag;
      }

      iter = minimals.erase(iter);
      changeFlag = true;
    }
  }

  if (changeFlag)
  {
    minimals.sort();
    minimals.unique();
  }

  return ! changeFlag;
}


void Combinations::checkAllMinimals(const unsigned cards)
{
  // Could also use checkMinimals here.
  vector<CombEntry>& centries = combEntries[cards];

  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    if (! Combinations::checkAndFixMinimals(
      centries,
      centries[holding].minimals))
      cout << "ERROR: holding " << holding << " uses non-minimals\n";
  }
}


string Combinations::strUniques(const unsigned cards) const
{
  unsigned cmin, cmax;
  if (cards == 0)
  {
    cmin = 1;
    cmax = control.cards();
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
    setw(9) << "Canon";

  for (auto& title: CombinationNames)
    ss <<
      setw(8) << title <<
      setw(8) << "Plays" <<
      setw(8) << "Strats";

  ss << "\n";
  
  for (unsigned c = cmin; c <= cmax; c++)
  {
    assert(c < maxCards+1);
    if (combEntries[c].size() == 0)
      continue;

    ss <<
      setw(5) << c <<
      setw(9) << combEntries[c].size() <<
      setw(9) << combEntries[c].size() - countNoncanonical[c];

    for (unsigned n = 0; n < COMB_SIZE; n++)
      ss <<
        setw(8) << countStats[c].data[n].count <<
        setw(8) << countStats[c].data[n].strAveragePlays() <<
        setw(8) << countStats[c].data[n].strAverageStrats();

    ss << "\n";
  }

  return ss.str() + "\n";
}

