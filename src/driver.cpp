#include <iostream>
#include <iomanip>

#include "Combinations.h"

#include "inputs/Control.h"
#include "inputs/Convert.h"

#include "stats/Timers.h"

#include "const.h"


// Can also keep them local and pass them to runThread, but in
// Visual Studio, for some reason the compiler does not like it
// if the list of thread arguments becomes too long.

Control control;
Convert convert;

vector<Timers> timers;


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

  Timer timerCard;
  for (unsigned cards = 1; cards <= control.cards(); cards++)
  {
    timerCard.reset();
    timerCard.start();
    cout << "Cards " << setw(2) << right << cards << ": ";

    // combinations.runUniquesMT(cards, control.numThreads());
    combinations.runUniques(cards);

    timerCard.stop();
    cout << timerCard.str();
  }
  cout << endl;

  cout << combinations.strUniques();
}

