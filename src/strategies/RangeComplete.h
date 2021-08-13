#ifndef SSS_RANGECOMPLETE_H
#define SSS_RANGECOMPLETE_H

#include "Result.h"

#include <string>

using namespace std;


class RangeComplete
{
  private:

    Result minimum;
    Result resultHigh;
    Result resultLow;

  public:

    void init(const Result& result);

    void extend(const Result& result);

    void operator *= (const RangeComplete& range2);

    bool operator < (const RangeComplete& range2) const;

    bool constant() const;
    const Result& constantResult() const;

    string strHeader(const bool rankFlag) const;

    unsigned char dist() const;

    unsigned char min() const;

    string str(const bool rankFlag) const;
};

#endif
