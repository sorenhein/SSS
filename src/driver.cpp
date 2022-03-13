#include <iostream>
#include <iomanip>

#include "combinations/Combinations.h"

#include "distributions/Distributions.h"
#include "distributions/covers/ExplStats.h"

#include "inputs/Control.h"
#include "inputs/Convert.h"

#include "strategies/result/ResConvert.h"

#include "utils/Timers.h"

Timers timers;

// Can also keep them local and pass them to runThread, but in
// Visual Studio, for some reason the compiler does not like it
// if the list of thread arguments becomes too long.

Control control;
Convert convert;
ResConvert resConvert;
Combinations combinations;
Distributions distributions;


int main(int argc, char * argv[])
{
  if (! control.parseCommandLine(argc, argv))
  {
    cout << "Bad command line" << endl;
    exit(0);
  }

  if (control.debugArgs())
    cout << control.str();

  setlocale(LC_ALL, "en_US.UTF-8");

  // Set up distributions.

  distributions.resize(control.cards());



  if (control.loop())
  {
    timers.start(TIMER_DISTRIBUTIONS);

    for (unsigned char cards = 0; cards <= control.cards(); cards++)
    {
      // distributions.runUniquesMT(cards, control.numThreads());
      distributions.runUniques(cards);
    }

    timers.stop(TIMER_DISTRIBUTIONS);

    cout << distributions.str();

    // Set up combinations.

    combinations.resize(control.cards());

    ExplStats explStats;
    distributions.resizeStats(explStats);

    for (unsigned char cards = 0; cards <= 11; cards++)
    {
      cout << "Cards " << setw(2) << right << +cards << endl;

      // combinations.runUniquesMT(cards, control.numThreads());
      combinations.runUniques(cards, distributions, explStats);
    }

    cout << "\n";
    cout << distributions.strCovers();

    cout << "\n";
    cout << explStats.str();

    cout << "\n";
    cout << combinations.str();


  }
  else
  {
    combinations.resize(control.cards(), false);

    vector<set<unsigned>> dependenciesTrinary;
    vector<set<unsigned>> dependenciesBinaryCan,
      dependenciesBinaryNoncan;

    combinations.getDependencies(
      control.holdingLength(),
      control.holding(),
      dependenciesTrinary, 
      dependenciesBinaryCan,
      dependenciesBinaryNoncan);

    timers.start(TIMER_DISTRIBUTIONS);

    distributions.runSingle(
      dependenciesBinaryCan, 
      dependenciesBinaryNoncan);

    cout << distributions.str();

    timers.stop(TIMER_DISTRIBUTIONS);

    combinations.runSingle(
      control.holdingLength(),
      control.holding(),
      distributions,
      dependenciesTrinary);
    
    combinations.covers(
      control.holdingLength(),
      control.holding(),
      distributions);
  }

  // combinations.tmp(control.cards(), control.holding());
  cout << "\n" << timers.str();
  
  // combinations.runSpecific(11, 132902, distributions);
  // combinations.runSpecific(12, 132889, distributions);

  // combinations.runSpecificVoid(6, 546, distributions);

  // KJ97 missing AQT86.
  // combinations.runSpecificVoid(9, 14762, distributions);

  // combinations.runSpecificVoid(12, 398582, distributions);

}

