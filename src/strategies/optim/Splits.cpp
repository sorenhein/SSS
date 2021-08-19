#include <iostream>
#include <iomanip>
#include <sstream>

#include "Splits.h"
#include "../StratData.h"

// TMP
#include "../../utils/Timer.h"
extern vector<Timer> timersStrat;


Splits::Splits()
{
  Splits::reset();
}


Splits::~Splits()
{
}


void Splits::reset()
{
  own.reset();
  shared.reset();
  ownPtrs.clear();
  matrix.clear();
  count = 0;
}


void Splits::pushDistribution(
  Strategies& strats,
  const StratData& stratData)
{
  auto siter = strats.strategies.begin();
  for (auto& sd: stratData.data)
  {
    siter->push_back(* sd.iter);
    siter++;
  }
}


void Splits::splitDistributions(
  Strategies& strategies,
  const Strategy& counterpart)
{
  // Split our strategies by distribution into one group (own) with
  // those distributions that are unique to us, and another (shared)
  // with distributions that overlap.  This is relative to counterpart.

  // List of iterators to a Result of each Strategy in stratsToSplit.
  // All are in sync to point to a given distribution.
  // They move in sync down across the Strategy's.
  StratData stratData;
  stratData.data.resize(count);
  strategies.getLoopData(stratData);

  auto riter = counterpart.begin();
  while (true)
  {
    if (riter == counterpart.end())
    {
      // A unique distribution.
      Splits::pushDistribution(own, stratData);

      if (stratData.advance() == STRATSTATUS_END)
        break;
      else
        continue;
    }

    if (riter->dist() < stratData.dist())
    {
      // Distribution that is only in strat2.
      riter++;
      continue;
    }

    bool endFlag = false;
    while (stratData.dist() < riter->dist())
    {
      // A unique distribution.
      Splits::pushDistribution(own, stratData);

      if (stratData.advance() == STRATSTATUS_END)
      {
        endFlag = true;
        break;
      }
    }

    if (endFlag)
      break;

    if (riter->dist() == stratData.dist())
    {
      // A shared distribution.
      Splits::pushDistribution(shared, stratData);

      if (stratData.advance() == STRATSTATUS_END)
        break;
    }

    riter++;
  }
}


void Splits::setPointers()
{
  // Pointers to each of the own, unique partial Strategy's,
  // for later use.
  ownPtrs.resize(count);
  auto ownIter = own.strategies.begin();
  for (unsigned i = 0; i < count; i++, ownIter++)
    ownPtrs[i] = &* ownIter;
}


void Splits::setMatrix()
{
  own.makeRanges();
  own.scrutinize(own.ranges);

  matrix.resize(count);
  for (unsigned i = 0; i < count; i++)
    matrix[i].resize(count);

  if (own.empty())
  {
    for (unsigned i = 0; i < count; i++)
      for (unsigned j = 0; j < count; j++)
        matrix[i][j] = WIN_EQUAL_OVERALL;
  }
  else
  {
    for (unsigned i = 0; i < count; i++)
    {
      matrix[i][i] = WIN_EQUAL_OVERALL;
      for (unsigned j = 0; j < i; j++)
      {
        // Compare c = ownPtrs[i]->compareByProfile(* ownPtrs[j]);

        // Seems wasteful
        bool b12 = (ownPtrs[i]->greaterEqualByProfile)(* ownPtrs[j]);
        bool b21 = (ownPtrs[j]->greaterEqualByProfile)(* ownPtrs[i]);
        CompareDetail c;
        if (b12)
        {
          // If number and profile of tricks are the same, go deeper.
          if (b21)
            c = ownPtrs[i]->compareDetail(* ownPtrs[j]);
          else
            c = WIN_FIRST_PRIMARY;
        }
        else if (b21)
          c = WIN_SECOND_PRIMARY;
        else
          c = ownPtrs[i]->compareDetail(* ownPtrs[j]);

        matrix[i][j] = c;

        // Flip for the anti-symmetric position.
        if (c == WIN_SECOND_PRIMARY)
          matrix[j][i] = WIN_FIRST_PRIMARY;
        else if (c == WIN_SECOND_SECONDARY)
          matrix[j][i] = WIN_FIRST_SECONDARY;
        else if (c == WIN_FIRST_PRIMARY)
          matrix[j][i] = WIN_SECOND_PRIMARY;
        else if (c == WIN_FIRST_SECONDARY)
          matrix[j][i] = WIN_SECOND_SECONDARY;
        else
          matrix[j][i] = c;
      }
    }
  }
}


void Splits::split(
  Strategies& strategies,
  const Strategy& counterpart)
{
  count = strategies.size();

  own.strategies.resize(count);
  shared.strategies.resize(count);

  Splits::splitDistributions(strategies, counterpart);

  Splits::setPointers();

  Splits::setMatrix();
}


const list<Strategy>& Splits::sharedStrategies() const
{
  return shared.strategies;
}


const Strategy& Splits::ownStrategy(const unsigned index) const
{
 return * ownPtrs[index];
}


Compare Splits::compare(
  const unsigned index1,
  const unsigned index2) const
{
  switch(matrix[index1][index2])
  {
    case WIN_NEUTRAL_OVERALL: return WIN_UNSET;
    case WIN_EQUAL_OVERALL: return WIN_EQUAL;
    case WIN_FIRST_PRIMARY: return WIN_FIRST;
    case WIN_SECOND_PRIMARY: return WIN_SECOND;
    case WIN_FIRST_SECONDARY: return WIN_FIRST;
    case WIN_SECOND_SECONDARY: return WIN_SECOND;
    case WIN_DIFFERENT_PRIMARY: return WIN_DIFFERENT;
    case WIN_DIFFERENT_SECONDARY: return WIN_DIFFERENT;
  }
  return WIN_UNSET;
}


CompareDetail Splits::compareDetail(
  const unsigned index1,
  const unsigned index2) const
{
  return matrix[index1][index2];
}


unsigned Splits::weight(const unsigned index) const
{
  return ownPtrs[index]->weight();
}

