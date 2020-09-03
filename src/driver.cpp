#include <iostream>
#include <iomanip>

#include "Combinations.h"
#include "Distributions.h"

#include "inputs/Control.h"
#include "inputs/Convert.h"

#include "stats/Timers.h"

Timers timers;


// Can also keep them local and pass them to runThread, but in
// Visual Studio, for some reason the compiler does not like it
// if the list of thread arguments becomes too long.

Control control;
Convert convert;
Combinations combinations;
Distributions distributions;


#include "Ranks.h"
#include "Ranks2.h"
int main(int argc, char * argv[])
{
  if (! control.parseCommandLine(argc, argv))
  {
    cout << "Bad command line" << endl;
    exit(0);
  }

  if (control.debugArgs())
    cout << control.str();

  /* 
  Ranks2 ranks;
  CombEntry ce;
  ranks.resize(13);
  ranks.set(464920, ce);
  cout << ranks.str();
  */

  /* */
  Ranks2 ranks;
  CombEntry ce;
  ranks.resize(13);
  ranks.set(464920, ce);
  cout << ranks.str();
  cout << "canonical holding " << ce.canonicalHolding << endl;
  // exit(0);
  /* */

  /* */
  vector<PlayEntry> plays;
  unsigned term, playNo;
  ranks.setPlays(plays, playNo, term);
  cout << "size " << playNo << endl << endl;
  cout << plays[0].strHeader();
  for (unsigned i = 0; i < playNo; i++)
    cout << plays[i].str();
  exit(0);
  /* */

  // Set up combinations.

  combinations.resize(control.cards());

  timers.start(TIMER_COMBINATIONS);

  for (unsigned cards = 1; cards <= control.cards(); cards++)
  {
    cout << "Cards " << setw(2) << right << cards << endl;

    // combinations.runUniquesMT(cards, control.numThreads());
    combinations.runUniques(cards);
  }

  timers.stop(TIMER_COMBINATIONS);

  cout << combinations.strUniques();


  // combinations.tmp(control.cards(), control.holding());
  // Distribution dist;
  // dist.set(5, 27);
  // cout << dist.str();
  // exit(0);

  // Set up distributions.

  distributions.resize(control.cards());

  timers.start(TIMER_DISTRIBUTIONS);

  for (unsigned cards = 1; cards <= control.cards(); cards++)
  {
    cout << "Cards " << setw(2) << right << cards << endl;

    // distributions.runUniquesMT(cards, control.numThreads());
    distributions.runUniques(cards);
  }

  timers.stop(TIMER_DISTRIBUTIONS);
  cout << "\n" << timers.str();

  cout << distributions.str();
}

