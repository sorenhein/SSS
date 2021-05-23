#ifndef SSS_RANGE_H
#define SSS_RANGE_H

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


struct Range
{
  unsigned dist;
  unsigned lower;
  unsigned upper;
  unsigned minimum;

  void operator *= (const Range& range2)
  {
    if (range2.minimum < minimum)
      minimum = range2.minimum;

    if (range2.upper < upper ||
        (range2.upper == upper && range2.lower < lower))
    {
      lower = range2.lower;
      upper = range2.upper;
    }
  };

  bool operator < (const Range& range2) const
  {
    return (upper <= range2.lower &&
        (range2.lower < range2.upper || lower < upper));
  };

  bool constant() const
  {
    return (lower == minimum && upper == minimum);
  };

  string strHeader() const
  {
    stringstream ss;
    ss << 
      setw(4) << right << "dist" <<
      setw(4) << "lo" <<
      setw(4) << "hi" << 
      setw(4) << "min" << 
      endl;
    return ss.str();
  };

  string str() const
  {
    stringstream ss;
    ss << 
      setw(4) << dist <<
      setw(4) << lower <<
      setw(4) << upper << 
      setw(4) << minimum << 
      endl;
    return ss.str();
  };
};

#endif
