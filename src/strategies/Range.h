#ifndef SSS_RANGE_H
#define SSS_RANGE_H

#include <string>

using namespace std;

struct Result;


class Range
{
  private:

    unsigned char distribution;
    unsigned char lower;
    unsigned char upper;
    unsigned char minimum;

  public:

    void init(const Result& result);

    void extend(const Result& result);

    void operator *= (const Range& range2);

    bool operator < (const Range& range2) const;

    bool constant() const;

    string strHeader() const;

    unsigned char dist() const;

    unsigned char min() const;

    string str() const;
};

#endif
