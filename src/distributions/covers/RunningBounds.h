/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#ifndef SSS_RUNNINGBOUNDS_H
#define SSS_RUNNINGBOUNDS_H

#include <string>

using namespace std;


class RunningBounds
{
  private:

    unsigned char minWest; // Sum of West's top minima
    unsigned char minEast; // Sum of East's top minima
    unsigned char maxDiff; // Largest difference max-min for a top
    unsigned char maxWest; // Largest West maximum
    unsigned char maxEast; // Largest East maximum

    unsigned char length;


  public:

   void reset(const unsigned char lengthIn);

    void step(
      const RunningBounds& stackBounds,
      const unsigned char topCountActual,
      const unsigned char topCountLow,
      const unsigned char topCountHigh);

    bool busted() const;

    bool unnecessaryLength(
      const unsigned char lengthWestLow,
      const unsigned char lengthWestHigh) const;

    unsigned char lengthWestLow() const;

    unsigned char lengthWestHigh() const;

    string str() const;
};

#endif
