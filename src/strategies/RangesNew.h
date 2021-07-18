#ifndef SSS_RANGES_H
#define SSS_RANGES_H

#include <list>
#include <string>

#include "Range.h"

using namespace std;

struct Result;


class RangesNew
{
  private:

    list<Range> * rangesPtr;

    bool winnersFlag;


  public:

    RangesNew(const bool winnersFlagIn);

    ~RangesNew();

    list<Range>::iterator begin()
      { return rangesPtr->begin(); };
    list<Range>::iterator end()
      { return rangesPtr->end(); }
    list<Range>::const_iterator begin() const
      { return rangesPtr->begin(); };
    list<Range>::const_iterator end() const
      { return rangesPtr->end(); }

    void reset();

    bool empty() const;

    void init(const list<Result>& results);

    void extend(const list<Result>& results);

    void operator *= (const RangesNew& r2);

    string str() const;
};

#endif
