#include <iostream>
#include <iomanip>
#include <sstream>

#include "Extension.h"
#include "StratData.h"

// TMP
#include "../stats/Timer.h"
extern vector<Timer> timersStrat;


Extension::Extension()
{
  Extension::reset();
}


Extension::~Extension()
{
}


void Extension::reset()
{
  overlap.reset();
  weightInt = 0;
}


unsigned Extension::weight() const
{
  return weightInt;
}

