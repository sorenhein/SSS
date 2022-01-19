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
  timersStrat[30].name("Ranks::getMinimals");
  timersStrat[31].name("CombEntry::fixMinimals");
  timersStrat[32].name("CombTest::checkReductions");
}


void Combinations::reset()
{
  combMemory.reset();

  countStats.clear();
  countNonreference.clear();

  timersStrat.clear();
  Combinations::setTimerNames();
}


void Combinations::resize(
  const unsigned char maxCardsIn,
  const bool fullFlag)
{
  combMemory.resize(maxCardsIn, fullFlag);

  countStats.resize(maxCardsIn+1);
  for (unsigned char cards = 0; cards < maxCardsIn+1; cards++)
  {
    countStats[cards].data.resize(COMB_SIZE);
    countStats[cards].data.resize(COMB_SIZE);
  }

  countNonreference.resize(maxCardsIn+1);
}


void Combinations::dumpVS(
  const string& title,
  const unsigned char cards,
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


void Combinations::getDependencies(
  const unsigned char cards,
  const unsigned holding,
  vector<set<unsigned>>& dependenciesTrinary,
  vector<set<unsigned>>& dependenciesBinaryCan,
  vector<set<unsigned>>& dependenciesBinaryNoncan)
{
  Ranks ranks;
  Plays plays;
  plays.resize(cards);
  DistMap distMap;

  vector<set<unsigned>> scratch1, scratch2;
  dependenciesTrinary.resize(cards+1);
  dependenciesBinaryCan.resize(cards+1);
  dependenciesBinaryNoncan.resize(cards+1);
  scratch1.resize(cards+1);
  scratch2.resize(cards+1);

  // We add later plays from scratch1 entries to scratch2.
  scratch1[cards].insert(holding);

  while (true)
  {
    bool doneFlag = true;
    for (unsigned char c = 0; c <= cards; c++)
    {
      for (auto& dep: scratch1[c])
      {
        CombEntry& centry = combMemory.getEntry(c, dep);
        if (centry.isReference() || dependenciesTrinary[c].count(dep))
        {
          // Already solved
          continue;
        }

        ranks.resize(c);
        ranks.setRanks(dep, centry);

        plays.clear();
        Result trivialEntry;
        ranks.setPlays(plays, trivialEntry);

        plays.addHoldings(scratch2);
        dependenciesTrinary[c].insert(dep);

        // The binary holding may not be canonical, in which case we
        // also need to generate the canonical one.  It is convenient 
        // to keep these separate.

        const unsigned h2 = centry.getHolding2();
        distMap.reset();
        distMap.setRanks(c, h2);
        const DistID distID = distMap.getID();
        if (distID.cards == c && distID.holding == h2)
        {
          dependenciesBinaryCan[distID.cards].insert(distID.holding);
        }
        else
        {
          dependenciesBinaryCan[distID.cards].insert(distID.holding);
          dependenciesBinaryNoncan[c].insert(h2);
        }
          
        doneFlag = false;
      }
    }

    if (doneFlag)
      break;

    for (unsigned c = 0; c <= cards; c++)
      scratch1[c].clear();

    swap(scratch1, scratch2);
  }
}


void Combinations::runSingle(
  const unsigned char cards,
  const unsigned holding,
  const Distributions& distributions,
  const vector<set<unsigned>>& dependenciesTrinary)
{
  Ranks ranks;
  Plays plays;
  plays.resize(cards);

  for (unsigned char c = 0; c <= cards; c++)
  {
    for (auto& dep: dependenciesTrinary[c])
    {
      Combination& comb = combMemory.add(c, dep);
      comb.setMaxRank(ranks.maxRank());

      CombEntry& centry = combMemory.getEntry(c, dep);

      ranks.resize(c);
      ranks.setRanks(dep, centry);

      comb.strategize(centry, * this, distributions,
        ranks, plays, (c == cards && holding == dep));
    }
  }

  cout << distributions.strDynamic();
  cout << combMemory.strDynamic();
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
  const unsigned char cards,
  const Distributions& distributions)
{
  assert(cards < countStats.size());

  Ranks ranks;
  ranks.resize(cards);

  Plays plays;
  plays.resize(cards);

  vector<unsigned> histoPlay;
  histoPlay.resize(1000);

  // Minimals are guaranteed to have higher values than their referents,
  // so when when looping backwards we already have the minimals done
  // when we need them.

  // TODO Back into Combinations?
  CombTest ctest;

  for (unsigned holding = combMemory.size(cards); holding-- > 0; )
  {
    CombEntry& centry = combMemory.getEntry(cards, holding);

    timers.start(TIMER_RANKS);
    ranks.setRanks(holding, centry);
    timers.stop(TIMER_RANKS);

    const unsigned referenceHolding3 = centry.getHolding3();
    if (holding == referenceHolding3)
    {
      Combination& comb = combMemory.add(cards, holding);

      comb.setMaxRank(ranks.maxRank());

      // Plays is cleared and rewritten, so it is only an optimization
      // not to let Combination make its own plays.

      CombinationType ctype =
        comb.strategize(centry, * this, distributions, ranks, plays);

      if (ctype == COMB_TRIVIAL)
      {
        // Don't look for minimals.
        centry.setType(COMB_TRIVIAL);
      }
      else
      {
histoPlay[plays.size()]++;

        timersStrat[30].start();
        if (Combinations::getMinimals(comb.strategies(), ranks, centry))
          centry.setMinimal();
        timersStrat[30].stop();

        centry.setType(Combinations::classify(
          centry.isMinimal(), comb.strategies(), ranks));

        timersStrat[31].start();
        if (! centry.fixMinimals(combMemory, cards))
        {
          cout << "WARNING: Uses non-minimals\n\n";
        }
        timersStrat[31].stop();

        if (centry.isReference())
        {
          timersStrat[32].start();
          ctest.checkReductions(cards, combMemory, centry,
            comb.strategies(), comb.getMaxRank(),
            distributions.get(cards, centry.getHolding2()));
          timersStrat[32].stop();
        }

        if (! centry.isMinimal())
        {
          if (cards >= 11 || (cards == 10 && ! ranks.partnerVoid()))
            comb.forgetDetails();
        }
      }

      countStats[cards].data[centry.getType()].incr(
        plays.size(), comb.strategies().size());
    }
    else
    {
      centry.setIndex(
        combMemory.getEntry(cards, referenceHolding3).getIndex());

      centry.setReference(false);
      countNonreference[cards]++;
    }
  }

unsigned sump = 0, count = 0;
for (unsigned i = 0; i < histoPlay.size(); i++)
{
  if (histoPlay[i])
  {
    count += histoPlay[i];
    sump += i * histoPlay[i];
  }
}
if (count > 0)
{
  double d =
 (static_cast<float>(sump) / static_cast<float>(count));
cout << "Play average " << fixed << setprecision(2) << d << "\n\n";

}

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


/*
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
*/


void Combinations::runUniqueThread(
  const unsigned char cards,
  Distributions const * distributions,
  const unsigned thid)
{
  // TODO Haven't tested this since introducing CombMemory.
  assert(thid < threadCountStats.size());

  Ranks ranks;
  ranks.resize(cards);
  unsigned holding;

  Plays plays;
  plays.resize(cards);

  const unsigned counterMax = combMemory.size(cards);

  while (true)
  {
    holding = counterHolding++; // Atomic
    if (holding >= counterMax)
      break;

    CombEntry& centry = combMemory.getEntry(cards, holding);
    ranks.setRanks(holding, centry);

    const unsigned referenceHolding3 = centry.getHolding3();
    if (holding == referenceHolding3)
    {
      Combination& comb = combMemory.add(cards, holding);

      comb.setMaxRank(ranks.maxRank());

      comb.strategize(centry, * this, * distributions, ranks, plays);

      if (Combinations::getMinimals(comb.strategies(), ranks, centry))
        centry.setMinimal();

      centry.setType(Combinations::classify(
        centry.isMinimal(), comb.strategies(), ranks));

      threadCountStats[thid].data[centry.getType()].incr(
        plays.size(), comb.strategies().size());
    }
    else
    {
      centry.setIndex(
        combMemory.getEntry(cards, referenceHolding3).getIndex());

      threadCountNonreference[thid]++;
    }
  }
}


void Combinations::runUniquesMT(
  const unsigned char cards,
  const Distributions& distributions,
  const unsigned numThreads)
{
  // TODO Untested since changes to statistics gathering.

  counterHolding = 0;

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


void Combinations::covers(
  const unsigned char cards,
  const unsigned holding,
  Distributions& distributions)
{
  CombEntry& centry = combMemory.getEntry(cards, holding);
  Distribution& dist = distributions.get(cards, centry.getHolding2());
  const Combination& comb = combMemory.getComb(cards, holding);

  const unsigned numCovers = comb.covers(dist);
}


Combination const * Combinations::getPtr(
  const unsigned char cards,
  const unsigned holding3) const
{
  return &combMemory.getComb(cards, holding3);
}


string Combinations::strHeader() const
{
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

  return ss.str() + "\n";
}


string Combinations::strLine(const unsigned char cards) const
{
  if (combMemory.size(cards) == 0)
    return "";

  stringstream ss;

  ss <<
    setw(5) << +cards <<
    setw(9) << combMemory.size(cards) <<
    setw(9) << combMemory.size(cards) - countNonreference[cards];

  for (unsigned n = 0; n < COMB_SIZE; n++)
    ss <<
      setw(8) << countStats[cards].data[n].count <<
      setw(8) << countStats[cards].data[n].strAveragePlays() <<
      setw(8) << countStats[cards].data[n].strAverageStrats();

  return ss.str() + "\n";
}


string Combinations::str(const unsigned char cards) const
{
  unsigned char cmin, cmax;
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

  string s = Combinations::strHeader();
  
  for (unsigned char c = cmin; c <= cmax; c++)
    s += Combinations::strLine(c);

  return s + "\n";
}

