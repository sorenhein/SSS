#ifndef SSS_COMBINATION_H
#define SSS_COMBINATION_H

#include <string>
#include <list>

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



  public:

    Combination();

    ~Combination();

    void reset();

    void strategize(
      const CombEntry& centry,
      const Combinations& combinations,
      const Distributions& distributions,
      Ranks& ranks,
      Plays& plays);

    string str() const;

};

#endif
