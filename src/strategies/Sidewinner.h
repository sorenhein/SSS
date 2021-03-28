#ifndef SSS_SIDEWINNER_H
#define SSS_SIDEWINNER_H

#include <string>

#include "../const.h"

using namespace std;


// Sidewinner itself cannot be different, but this is also used
// to compare subwinners.


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

    unsigned getRank() const;
    bool rankSame(const Sidewinner& sw2) const;
    bool rankExceeds(const Sidewinner& sw2) const;

    void operator *= (const Sidewinner& sw2);

    unsigned no() const;

    string str(
      const string& name,
      const bool rankFlag = true) const;

    string strDebug(const string& name) const;
};

#endif
