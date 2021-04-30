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

  timers.start(TIMER_DISTRIBUTIONS);

  for (unsigned cards = 1; cards <= control.cards(); cards++)
  {
    // distributions.runUniquesMT(cards, control.numThreads());
    distributions.runUniques(cards);
  }

  timers.stop(TIMER_DISTRIBUTIONS);

  cout << distributions.str();


  // Set up combinations.

  combinations.resize(control.cards());

  timers.start(TIMER_COMBINATIONS);

  // for (unsigned cards = 0; cards <= control.cards(); cards++)
  for (unsigned cards = 0; cards <= 9; cards++)
  {
    cout << "Cards " << setw(2) << right << cards << endl;

    // combinations.runUniquesMT(cards, control.numThreads());
    combinations.runUniques(cards, distributions);
  }

  timers.stop(TIMER_COMBINATIONS);

  cout << "\n";
  cout << combinations.strUniques();


  // combinations.tmp(control.cards(), control.holding());
  cout << "\n" << timers.str();
  
  // combinations.runSpecific(11, 132902, distributions);
  // combinations.runSpecific(12, 132889, distributions);

  // combinations.runSpecificVoid(6, 546, distributions);

  combinations.runSpecificVoid(9, 14762, distributions);

  // combinations.runSpecificVoid(12, 398582, distributions);

}

