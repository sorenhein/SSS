#ifndef SSS_COMBINATION_H
#define SSS_COMBINATION_H

#include <string>
#include <list>

#include "strategies/Tvectors.h"
#include "struct.h"

using namespace std;

struct CombEntry;
class Combinations;
class Distributions;
class Distribution;
class Ranks;
class Plays;


class Combination
{
  private:

    Distribution const * distPtr;

    Tvectors strats;


  public:

    Combination();

    ~Combination();

    void reset();

    const Tvectors& strategize(
      const CombEntry& centry,
      const Combinations& combinations,
      const Distributions& distributions,
      Ranks& ranks,
      Plays& plays);

    const Tvectors& strategies() const;

    string str() const;

};

#endif
