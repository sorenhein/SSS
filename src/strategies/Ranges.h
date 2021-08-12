#ifndef SSS_RANGES_H
#define SSS_RANGES_H

#include <list>
#include <string>

#include "RangeComplete.h"

using namespace std;

class Result;


class Ranges
{
  private:

    list<RangeComplete> ranges;

    bool winnersFlag;


  public:

    Ranges();

    ~Ranges();

    list<RangeComplete>::iterator begin() { return ranges.begin(); };
    list<RangeComplete>::iterator end() { return ranges.end(); }
    list<RangeComplete>::const_iterator begin() const { return ranges.begin(); };
    list<RangeComplete>::const_iterator end() const { return ranges.end(); }

    void reset();

    bool empty() const;

    unsigned size() const;

    void init(
      const list<Result>& results,
      const bool winnersFlagIn);

    void extend(const list<Result>& results);

    void operator *= (const Ranges& r2);

    string strHeader() const;

    string str() const;
};

#endif
