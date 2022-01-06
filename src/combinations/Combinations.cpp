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

#include "Combinations.h"
#include "CombFiles.h"
#include "CombEntry.h"
#include "CombTest.h"

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


void Combinations::setTimerNames()
{
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


void Combinations::reset()
{
  maxCards = 0;
  combEntries.clear();
  uniques.clear();
  countStats.clear();
  countNonreference.clear();

  timersStrat.clear();
  Combinations::setTimerNames();
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

    if (control.runRankComparisons())
    {
      // One void plus half of the rest, as North always has the
      // highest card among the North-South.

      const unsigned numRanked = 1 + ((numCombinations-1) >> 1);
      uniques[cards].resize(numRanked);
    }
    else
    {
      assert(cards < UNIQUE_COUNT.size());
      uniques[cards].resize(UNIQUE_COUNT[cards]);
    }

    numCombinations *= 3;
  }

  countStats.resize(maxCardsIn+1);
  for (unsigned cards = 0; cards < maxCardsIn+1; cards++)
  {
    countStats[cards].data.resize(COMB_SIZE);
    countStats[cards].data.resize(COMB_SIZE);
  }

  countNonreference.resize(maxCardsIn+1);
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

  const unsigned referenceHolding3 = centry.reference.holding3;
  if (holding != referenceHolding3)
  {
    cout << "Specific (cards, holding) = (" << cards << ", " <<
      holding << ") is not reference." << endl;
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

  const unsigned referenceHolding3 = centry.reference.holding3;
  if (holding != referenceHolding3)
  {
    cout << "Specific (cards, holding) = (" << cards << ", " <<
      holding << ") is not reference." << endl;
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

  // Result resultLowest;
  // strategies.getResultLowest(resultLowest);
  list<Result> resultList;
  strategies.getResultList(resultList);
  return ranks.getMinimals(resultList, minimals);
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

// cout << "STARTING ON HOLDING3 " << holding << "\n";
    timers.start(TIMER_RANKS);
    ranks.setRanks(holding, centry);
    timers.stop(TIMER_RANKS);

    const unsigned referenceHolding3 = centry.reference.holding3;
    if (holding == referenceHolding3)
    {
      assert(uniqueIndex < uniqs.size());
      centry.reference.index = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];
      uniqueIndex++;

      comb.setMaxRank(ranks.maxRank());

    // Plays is cleared and rewritten, so it is only an optimization
    // not to let Combination make its own plays.

      comb.strategize(centry, * this, distributions, ranks, plays);

      centry.minimalFlag =
        Combinations::getMinimals(comb.strategies(), ranks, 
          centry.minimals);


      if (! centry.minimalFlag)
      {
      // TODO Control by some flag.  Isn't needed anyway
        comb.reduce(
          * distributions.ptrNoncanonical(cards, centry.refHolding2));
          // * distributions.ptrNoncanonical(cards, centry.reference.holding2));
      }

      centry.type = Combinations::classify(
        centry.minimalFlag, comb.strategies(), ranks);

      Result res;
      comb.strategies().getResultLowest(res);
      centry.winRankLow = res.getRank();

      countStats[cards].data[centry.type].incr(
        plays.size(), comb.strategies().size());
    }
    else
    {
      centry.reference.index = centries[referenceHolding3].reference.index;
      countNonreference[cards]++;
    }
  }

  timersStrat[33].start();
  Combinations::fixMinimals(centries);
  timersStrat[33].stop();

  CombTest ctest;

  timersStrat[31].start();
  ctest.checkAllMinimals(centries);
  timersStrat[31].stop();

  // TODO Control by flags.
  timersStrat[30].start();
  ctest.checkAllReductions(cards, centries, uniqs, distributions);
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


void Combinations::runUniquesOld(
  const unsigned cards,
  const Distributions& distributions)
{
  // TODO Similar to runUniques, but later on this should become the
  // loop over unranked combinations.  So here it makes sense to look
  // at canonicals, also in Ranks (plays).

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

  // CombTest ctest;

  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    CombEntry& centry = centries[holding];

    timers.start(TIMER_RANKS);
    ranks.setRanks(holding, centry);
    timers.stop(TIMER_RANKS);

    const unsigned referenceHolding3 = centry.reference.holding3;
    if (holding == referenceHolding3)
    {
      // TODO just centry.referenceFlag?
      assert(uniqueIndex < uniqs.size());
      centry.reference.index = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];
      uniqueIndex++;

      comb.setMaxRank(ranks.maxRank());

      // Plays is cleared and rewritten, so it is only an optimization
      // not to let Combination make its own plays.

      comb.strategize(centry, * this, distributions, ranks, plays);

      centry.minimalFlag =
        Combinations::getMinimals(comb.strategies(), ranks, centry.minimals);
      if (! centry.minimalFlag)
      {
        // TODO Control by some flag
        comb.reduce(
          * distributions.ptrNoncanonical(cards, centry.refHolding2));
          // * distributions.ptrNoncanonical(cards, centry.reference.holding2));
      }

      centry.type = Combinations::classify(
        centry.minimalFlag, comb.strategies(), ranks);

      Result res;
      comb.strategies().getResultLowest(res);
      centry.winRankLow = res.getRank();

      countStats[cards].data[centry.type].incr(
        plays.size(), comb.strategies().size());
      
      /*
      if (! centry.minimalFlag)
      {
        // TODO Control by some flag
        ctest.checkReductions(cards, centry, comb.strategies(), 
          * distributions.ptrNoncanonical(cards, centry.canonical.holding2));
      }
      */
    }
    else
    {
      centry.reference.index = centries[referenceHolding3].reference.index;
      countNonreference[cards]++;
    }
  }

/*
  timersStrat[33].start();
  Combinations::fixMinimals(centries);
  timersStrat[33].stop();

  timersStrat[31].start();
  ctest.checkAllMinimals(centries);
  timersStrat[31].stop();

  // TODO Control by flags.
  timersStrat[30].start();
  ctest.checkAllReductions(cards, centries, uniqs, distributions);
  timersStrat[30].stop();
*/

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

    const unsigned referenceHolding3 = centry.reference.holding3;
    if (holding == referenceHolding3)
    {
      const unsigned uniqueIndex = counterUnique++; // Atomic
      assert(uniqueIndex < uniqs.size());
      centry.reference.index = uniqueIndex;
      Combination& comb = uniqs[uniqueIndex];

      comb.setMaxRank(ranks.maxRank());

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
      centry.reference.index = centries[referenceHolding3].reference.index;
      threadCountNonreference[thid]++;
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

  threadCountNonreference.clear();
  threadCountNonreference.resize(numThreads);

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
    threadCountNonreference[cards] += threadCountNonreference[thid];
  }
}


#include "../const.h"

Combination const * Combinations::getPtr(
  const unsigned cards,
  const unsigned holding3,
  const CombMinimumMode mode,
  bool& rotateFlag) const
{
  const auto& centry = combEntries[cards][holding3];

  const auto& cref = (centry.referenceFlag ? centry :
    combEntries[cards][centry.reference.holding3]);

  // TODO Delete parameters rotateFlag and mode
  UNUSED(mode);
  rotateFlag = false;

  const unsigned ui = cref.reference.index;
  return &uniques[cards][ui];
}


void Combinations::fixMinimals(vector<CombEntry>& centries)
{
  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    // Only look at non-minimal combinations.
    const CombEntry& centry = centries[holding];
    if (! centry.referenceFlag || centry.minimalFlag)
      continue;

    if (! centries[holding].fixMinimals(holding, centries))
    {
      cout << "WARN-NONMIN: holding " << holding << " uses non-minimals\n";
    }
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
    setw(9) << "Refs";

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
      setw(9) << combEntries[c].size() - countNonreference[c];

    for (unsigned n = 0; n < COMB_SIZE; n++)
      ss <<
        setw(8) << countStats[c].data[n].count <<
        setw(8) << countStats[c].data[n].strAveragePlays() <<
        setw(8) << countStats[c].data[n].strAverageStrats();

    ss << "\n";
  }

  return ss.str() + "\n";
}

