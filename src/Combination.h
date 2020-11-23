#ifndef SSS_COMBINATION_H
#define SSS_COMBINATION_H

#include <string>
#include <list>

#include "struct.h"

using namespace std;

struct CombEntry;
class Ranks;
class Plays;


class Combination
{
  private:



  public:

    Combination();

    ~Combination();

    void reset();

    void strategize(
      const CombEntry& centry,
      Ranks& ranks,
      Plays& plays);

    string str() const;

};

#endif
