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


#include <set>

void Combinations::dumpVS(
  const string& title,
  const unsigned cards,
  const vector<set<unsigned>>& vs) const
{
  cout << title << "\n";
  for (unsigned c = 0; c <= cards; c++)
  {
    for (auto& solve: vs[c])
    {
      cout << setw(2) << c << setw(8) << solve << "\n";
    }
  }
  cout << "\n";
}


void Combinations::runSingle(
  const unsigned cards,
  const unsigned holding,
  const Distributions& distributions)
{
  // This is like a method getDependencies3().
  // Then there is one to run the dependencies.
  // In principle, the same for distributions?

  assert(cards < combEntries.size());

  Ranks ranks;
  Plays plays;
  plays.resize(cards);

  vector<set<unsigned>> finished, scratch1, scratch2;
  finished.resize(cards+1);
  scratch1.resize(cards+1);
  scratch2.resize(cards+1);

  // We add later plays from scratch1 entries to scratch2.
  scratch1[cards].insert(holding);

  unsigned uniqueIndex = 0;

  while (true)
  {
    bool doneFlag = true;
    for (unsigned c = 0; c <= cards; c++)
    {
      for (auto& solve: scratch1[c])
      {
        CombEntry& centry = combEntries[c][solve];
        if (centry.isReference() || finished[c].count(solve))
        {
          // Already solved
          continue;
        }

        ranks.resize(c);
        ranks.setRanks(solve, centry);

        plays.clear();
        Result trivialEntry;
        const CombinationType ctype = ranks.setPlays(plays, trivialEntry);

        if (ctype == COMB_CONSTANT)
        {
          // If it's a trivial solution, solve it straight away.
          vector<Combination>& uniqs = uniques[c];

          assert(uniqueIndex < uniqs.size());
          centry.setIndex(uniqueIndex);
          Combination& comb = uniqs[uniqueIndex];
          uniqueIndex++;

          comb.setMaxRank(ranks.maxRank());

          Distribution const * distPtr = distributions.ptrNoncanonical(
            ranks.size(), centry.getHolding2());

          comb.setTrivial(trivialEntry, 
            static_cast<unsigned char>(distPtr->size()));
          
          centry.setReference();
        }
        else
        {
          plays.addHoldings(scratch2);
          finished[c].insert(solve);
          doneFlag = false;
        }
      }

    }

    if (doneFlag)
      break;

    for (unsigned c = 0; c <= cards; c++)
      scratch1[c].clear();

    swap(scratch1, scratch2);
  }

  Combinations::dumpVS("finished", cards, finished);
}


void Combinations::runSpecific(
  const unsigned cards,
  const unsigned holding,
  const Distributions& distributions)
{
  // This and the next one are not used.
  assert(cards < combEntries.size());

  Ranks ranks;
  ranks.resize(cards);
  
  Plays plays;
  plays.resize(cards);

  CombEntry& centry = combEntries[cards][holding];
  ranks.setRanks(holding, centry);

  const unsigned referenceHolding3 = centry.getHolding3();
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

  const unsigned referenceHolding3 = centry.getHolding3();
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
  CombEntry& centry) const
{
  // Returns true when the combination is already minimal,
  // in which case the set remains empty.

  list<Result> resultList;
  strategies.getResultList(resultList);
  return ranks.getMinimals(resultList, centry);
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

  vector<unsigned> histoPlay;
  histoPlay.resize(1000);

  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    CombEntry& centry = centries[holding];

// cout << "STARTING ON HOLDING3 " << holding << "\n";
    timers.start(TIMER_RANKS);
    ranks.setRanks(holding, centry);
    timers.stop(TIMER_RANKS);

    const unsigned referenceHolding3 = centry.getHolding3();
    if (holding == referenceHolding3)
    {
      assert(uniqueIndex < uniqs.size());
      centry.setIndex(uniqueIndex);
      Combination& comb = uniqs[uniqueIndex];
      uniqueIndex++;

      comb.setMaxRank(ranks.maxRank());

      // Plays is cleared and rewritten, so it is only an optimization
      // not to let Combination make its own plays.

      comb.strategize(centry, * this, distributions, ranks, plays);

histoPlay[plays.size()]++;

      // centry.minimalFlag =
      if (Combinations::getMinimals(comb.strategies(), ranks, centry))
        centry.setMinimal();

      centry.setType(Combinations::classify(
        centry.isMinimal(), comb.strategies(), ranks));

      countStats[cards].data[centry.getType()].incr(
        plays.size(), comb.strategies().size());
    }
    else
    {
      centry.setIndex(centries[referenceHolding3].getIndex());
      centry.setReference(false);
      countNonreference[cards]++;
    }
  }

// cout << "Play histogram\n";
unsigned sump = 0, count = 0;
for (unsigned i = 0; i < histoPlay.size(); i++)
{
  if (histoPlay[i])
  {
    // cout << setw(4) << i << setw(8) << histoPlay[i] << "\n";
    count += histoPlay[i];
    sump += i * histoPlay[i];
  }
}
// cout << "\n";
if (count > 0)
{
  double d =
 (static_cast<float>(sump) / static_cast<float>(count));
cout << "Play average " << fixed << setprecision(2) << d << "\n\n";

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

    const unsigned referenceHolding3 = centry.getHolding3();
    if (holding == referenceHolding3)
    {
      // TODO just centry.referenceFlag?
      assert(uniqueIndex < uniqs.size());
      centry.setIndex(uniqueIndex);
      Combination& comb = uniqs[uniqueIndex];
      uniqueIndex++;

      comb.setMaxRank(ranks.maxRank());

      // Plays is cleared and rewritten, so it is only an optimization
      // not to let Combination make its own plays.

      comb.strategize(centry, * this, distributions, ranks, plays);

      // centry.minimalFlag =
      if (Combinations::getMinimals(comb.strategies(), ranks, centry))
        centry.setMinimal();

      centry.setType(Combinations::classify(
        centry.isMinimal(), comb.strategies(), ranks));

      countStats[cards].data[centry.getType()].incr(
        plays.size(), comb.strategies().size());
    }
    else
    {
      centry.setIndex(centries[referenceHolding3].getIndex());
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

    const unsigned referenceHolding3 = centry.getHolding3();
    if (holding == referenceHolding3)
    {
      const unsigned uniqueIndex = counterUnique++; // Atomic
      assert(uniqueIndex < uniqs.size());
      centry.setIndex(uniqueIndex);
      Combination& comb = uniqs[uniqueIndex];

      comb.setMaxRank(ranks.maxRank());

      comb.strategize(centry, * this, * distributions, ranks, plays);

      // centry.minimalFlag =
      if (Combinations::getMinimals(comb.strategies(), ranks, centry))
        centry.setMinimal();

      centry.setType(Combinations::classify(
        centry.isMinimal(), comb.strategies(), ranks));

      threadCountStats[thid].data[centry.getType()].incr(
        plays.size(), comb.strategies().size());
    }
    else
    {
      centry.setIndex(centries[referenceHolding3].getIndex());
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

  const auto& cref = (centry.isReference() ? centry :
    combEntries[cards][centry.getHolding3()]);

  // TODO Delete parameters rotateFlag and mode
  UNUSED(mode);
  rotateFlag = false;

  const unsigned ui = cref.getIndex();
  return &uniques[cards][ui];
}


void Combinations::fixMinimals(vector<CombEntry>& centries)
{
  for (unsigned holding = 0; holding < centries.size(); holding++)
  {
    if (! centries[holding].fixMinimals(centries))
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

