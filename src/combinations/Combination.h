#ifndef SSS_COMBINATION_H
#define SSS_COMBINATION_H

#include <string>

#include "../strategies/Strategies.h"

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

    Strategies strats;


  public:

    Combination();

    void reset();

    const Strategies& strategize(
      const CombEntry& centry,
      const Combinations& combinations,
      const Distributions& distributions,
      Ranks& ranks,
      Plays& plays,
      bool debugFlag = false);

    const Strategies& strategizeVoid(
      const CombEntry& centry,
      const Combinations& combinations,
      const Distributions& distributions,
      Ranks& ranks,
      Plays& plays,
      bool debugFlag = false);

    void reduce(const Distribution& distribution);

    const Strategies& strategies() const;

    string str() const;

};

#endif
