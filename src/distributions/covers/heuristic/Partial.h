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

    Partial();

    void set(
      Cover const * coverPtrIn,
      const Tricks& additionsIn,
      const unsigned rawWeightAdderIn);

    bool operator < (const Partial& partial2) const;

    bool empty() const;

    void addCoverToComplexity(Complexity& complexity) const;

    void addRowToComplexity(Complexity& complexity) const;

    const Cover& cover() const;

    Tricks& tricks();
    const Tricks& tricks() const;

    unsigned weight() const;

    string str() const;
};

#endif
