#ifndef SSS_STRATDATA_H
#define SSS_STRATDATA_H

#include "Result.h"
#include "RangeComplete.h"

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
  list<Result>::iterator iter;
  list<Result>::iterator end;

  void erase()
  {
    iter = ptr->erase(iter);
  };
};

struct StratData
{
  list<StratDatum> data;
  list<RangeComplete>::const_iterator riter;

  unsigned char dist()
  {
    assert(! data.empty());
    return data.front().iter->dist;
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
        data.front().iter->dist < dist)
    {
      riter++;
      for (auto& sd: data)
        sd.iter++;
    }

    if (data.front().iter == data.front().end)
      return STRATSTATUS_END;
    else if (data.front().iter->dist > dist)
      return STRATSTATUS_FURTHER_DIST;
    else
      return STRATSTATUS_SAME_DIST;
  };


  void eraseConstantDist(
    Result& cresult,
    const unsigned char tricks,
    const Winners& cwinners)
  {
    cresult.dist = data.front().iter->dist;
    cresult.tricks = tricks;
    
    // cresult.winners.reset();
    cresult.winners = cwinners;

    StratData::eraseDominatedDist();
    /*
    riter++;
    for (auto& sd: data)
    {
      cresult.winners *= sd.iter->winners;
      sd.erase();
    }
    */
  };

  void eraseDominatedDist()
  {
    riter++;
    for (auto& sd: data)
      sd.erase();
  };
};

#endif
