#ifndef SSS_RANGECOMPLETE_H
#define SSS_RANGECOMPLETE_H

#include "winners/Winners.h"

#include <string>

using namespace std;

struct Result;


class RangeComplete
{
  private:

    unsigned char distribution;
    unsigned char lower;
    unsigned char upper;
    unsigned char minimum;

    // Unlike Range, this class takes into account the Winner's when
    // tracking and comparing for a given distribution.
    // Let's say one Strategy has 2-3 tricks and the highest Winner
    // for 3 tricks is 4N.  Another strategy has 3 tricks and the
    // lowest winner is 6NS.  Then the defense would never choose the
    // second one, because even if the number of tricks could be the
    // same (3), the winner would be worse for the defense.
    // The general rule is to look at
    // 1. The highest winner at the highest number of tricks, vs.
    // 2. The lowest winner at the lowest number of tricks.

    Winners winnersHigh; // 1.
    Winners winnersLow;  // 2.

  public:

    void init(const Result& result);

    void extend(const Result& result);

    void operator *= (const RangeComplete& range2);

    bool operator < (const RangeComplete& range2) const;

    bool constant() const;
    const Winners& constantWinners() const;

    string strHeader() const;

    unsigned char dist() const;

    unsigned char min() const;

    string str() const;
};

#endif
