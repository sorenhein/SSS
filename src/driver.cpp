#include <iostream>
#include <iomanip>

#include "Combinations.h"

#include "inputs/Control.h"
#include "inputs/Convert.h"

#include "stats/Timers.h"

Timers timers;


// Can also keep them local and pass them to runThread, but in
// Visual Studio, for some reason the compiler does not like it
// if the list of thread arguments becomes too long.

Control control;
Convert convert;


int main(int argc, char * argv[])
{
  if (! control.parseCommandLine(argc, argv))
  {
    cout << "Bad command line" << endl;
    exit(0);
  }

  if (control.debugArgs())
    cout << control.str();

  Combinations combinations;
  combinations.resize(control.cards());

  timers.start(TIMER_UNIQUE);

  for (unsigned cards = 1; cards <= control.cards(); cards++)
  {
    cout << "Cards " << setw(2) << right << cards << endl;

    combinations.runUniquesMT(cards, control.numThreads());
    // combinations.runUniques(cards);
  }

  timers.stop(TIMER_UNIQUE);
  cout << timers.str();

  cout << combinations.strUniques();
}

