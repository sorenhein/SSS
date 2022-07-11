#ifndef SSS_PARTIAL_H
#define SSS_PARTIAL_H

#include < string>

#include "../Tricks.h"

using namespace std;

class Cover;
class Complexity;


class Partial
{
  private:

    Cover const * coverPtr;

    Tricks additions;

    unsigned rawWeightAdder;

  
  public:

    void set(
      Cover const * coverPtrIn,
      const Tricks& additionsIn,
      const unsigned rawWeightAdderIn);

    bool operator < (const Partial& partial2) const;

    void addCoverToComplexity(Complexity& complexity) const;

    void addRowToComplexity(Complexity& complexity) const;

    string str() const;
};

#endif
