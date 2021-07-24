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


void Extension::multiply(
  const Strategy& strat1,
  const Strategy& strat2,
  const RangesNew& ranges)
{
  overlap.multiply(strat1, strat2);
  overlap.scrutinize(ranges);
}


void Extension::finish(
  const unsigned index1,
  const unsigned index2,
  const unsigned weightUniques)
{
  index1Int = index1;
  index2Int = index2;

  weightInt = overlap.weight() + weightUniques;
}


void Extension::flatten(
  list<Strategy>& strategies,
  const Strategy& strat1,
  const Strategy& strat2)
{
  // TODO Make a two-product optimized method?
  overlap *= strat1;
  overlap *= strat2;
  strategies.push_back(move(overlap));
}


bool Extension::operator >= (const Extension& ext2) const
{
  return overlap.greaterEqualByProfile(ext2.overlap);
}


unsigned Extension::index1() const
{
  return index1Int;
}


unsigned Extension::index2() const
{
  return index2Int;
}


unsigned Extension::weight() const
{
  return weightInt;
}

