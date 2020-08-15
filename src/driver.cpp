#include <iostream>
#include <thread>

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


void runThread(unsigned thid);


int main(int argc, char * argv[])
{
  if (! control.parseCommandLine(argc, argv))
  {
    cout << "Bad command line" << endl;
    exit(0);
  }

  cout << control.str();

  timers.resize(control.numThreads());
  timers[0].start(TIMER_ALL_THREADS);

  /*
  vector<thread *> threads;
  threads.resize(control.numThreads());
  for (int thid = 0; thid < control.numThreads(); thid++)
    threads[thid] = new thread(&runThread, thid);

  for (int thid = 0; thid < control.numThreads(); thid++)
  {
    threads[thid]->join();
    delete threads[thid];
  }
  */

  timers[0].stop(TIMER_ALL_THREADS);

  // Consolidate the thread timers.
  /*
  for (int i = 1; i < control.numThreads(); i++)
    timers[0] += timers[i];

  cout << timers[0].str(2) << endl;
  */
}


void runThread(unsigned thid)
{
  UNUSED(thid);
}

