#include <iostream>
#include <iomanip>
#include <sstream>

#include "Extensions.h"

// TMP
#include "../stats/Timer.h"
extern vector<Timer> timersStrat;


Extensions::Extensions()
{
  Extensions::reset();
}


Extensions::~Extensions()
{
}


void Extensions::reset()
{
  extensions.clear();
  splits1.reset();
  splits2.reset();
}


void Extensions::split(
  Strategies& strategies,
  const Strategy& counterpart,
  const ExtensionSplit split)
{
  if (split == EXTENSION_SPLIT1)
    splits1.split(strategies, counterpart);
  else
    splits2.split(strategies, counterpart);
}
