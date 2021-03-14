#ifndef SSS_SIDEWINNER_H
#define SSS_SIDEWINNER_H

#include <string>

#include "../const.h"

using namespace std;


// Sidewinner itself cannot be different, but this is also used
// to compare subwinners.

enum WinnerCompare
{
  WIN_FIRST = 0,
  WIN_SECOND = 1,
  WIN_EQUAL = 2,
  WIN_DIFFERENT = 3,
  WIN_UNSET = 4
};


class Sidewinner
{
  private:

    unsigned rank;
    unsigned depth;
    unsigned number;

  public:

    void reset();

    void set(
      const unsigned rankIn,
      const unsigned depthIn,
      const unsigned numberIn);

    bool operator > (const Sidewinner& sw2) const;
    bool operator >= (const Sidewinner& sw2) const;
    bool operator == (const Sidewinner& sw2) const;
    bool operator != (const Sidewinner& sw2) const;
    bool operator <= (const Sidewinner& sw2) const;
    bool operator < (const Sidewinner& sw2) const;

    WinnerCompare compare(const Sidewinner& sw2) const;

    bool rankSame(const Sidewinner& sw2) const;
    bool rankExceeds(const Sidewinner& sw2) const;

    void operator *= (const Sidewinner& sw2);

    unsigned no() const;

    string str(const string& name) const;

    string strDebug(const string& name) const;
};

#endif
