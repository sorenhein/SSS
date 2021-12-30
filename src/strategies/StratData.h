/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2021 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_STRATDATA_H
#define SSS_STRATDATA_H

#include <cassert>

#include "result/Result.h"
#include "result/Range.h"

using namespace std;


// This is used to iterate over an ensemble of Strategy's in synchrony,
// i.e. one distribution at a time.


enum StratStatus
{
  STRATSTATUS_SAME_DIST = 0,
  STRATSTATUS_FURTHER_DIST = 1,
  STRATSTATUS_END = 2
};


struct StratDatum
{
  Strategy * ptr;
  list<Result>::const_iterator iter;
  list<Result>::const_iterator end;

  void erase()
  {
    iter = ptr->erase(iter);
  };
};


struct StratData
{
  list<StratDatum> data;
  list<Range>::const_iterator riter;

  void fill(list<Strategy>& strategies)
  {
    data.resize(strategies.size());
    auto siter = strategies.begin();
    for (auto& sd: data)
    {
      auto& s = * siter;
      sd.ptr = &s;
      sd.iter = s.begin();
      sd.end = s.end();
      siter++;
    }
  };

  void fill(const list<Strategy>& strategies)
  {
    data.resize(strategies.size());
    auto siter = strategies.begin();
    for (auto& sd: data)
    {
      auto& s = * siter;
      sd.iter = s.begin();
      sd.end = s.end();
      siter++;
    }
  };

  void pushDistributionOnto(list<Strategy>& strategies)
  {
    // Pushes a distribution (a row) onto strategies.
    auto siter = strategies.begin();
    for (auto& sd: data)
    {
      siter->push_back(* sd.iter);
      siter++;
    }
  };

  unsigned char dist()
  {
    assert(! data.empty());
    return data.front().iter->getDist();
  };

  bool done() const
  {
    assert(! data.empty());
    return (data.front().iter == data.front().end);
  };

  StratStatus advance()
  {
    // Advance one row.
    for (auto& sd: data)
      sd.iter++;

    if (data.front().iter == data.front().end)
      return STRATSTATUS_END;
    else
      return STRATSTATUS_FURTHER_DIST;
  };

  StratStatus advance(const unsigned dist)
  {
    // Advance to dist.
    while (data.front().iter != data.front().end &&
        data.front().iter->getDist() < dist)
    {
      riter++;
      for (auto& sd: data)
        sd.iter++;
    }

    if (data.front().iter == data.front().end)
      return STRATSTATUS_END;
    else if (data.front().iter->getDist() > dist)
      return STRATSTATUS_FURTHER_DIST;
    else
      return STRATSTATUS_SAME_DIST;
  };


  void eraseDominatedDist()
  {
    riter++;
    for (auto& sd: data)
      sd.erase();
  };

  bool sameTricks() const
  {
    unsigned t = numeric_limits<unsigned>::max();
    bool firstFlag = true;
    for (auto& sd: data)
    {
      if (firstFlag)
      {
        t = sd.iter->getTricks();
        firstFlag = false;
      }
      else if (t != sd.iter->getTricks())
        return false;
    }
    return true;
  };
};

#endif
