#ifndef SSS_STRATDATA_H
#define SSS_STRATDATA_H

#include "Result.h"
#include "Range.h"

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
    ptr->erase(iter);
  };
};

struct StratData
{
  list<StratDatum> data;
  Ranges::const_iterator riter;

  StratStatus advance(const unsigned dist)
  {
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
    const char tricks)
  {
    cresult.dist = data.front().iter->dist;
    cresult.tricks = tricks;
    
    cresult.winners.reset();

    for (auto& sd: data)
    {
      cresult.winners *= sd.iter->winners;
      sd.erase();
    }
  };

  void eraseDominatedDist()
  {
    for (auto& sd: data)
      sd.erase();
  };
};

#endif
