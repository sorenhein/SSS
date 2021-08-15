#ifndef SSS_RESULT_H
#define SSS_RESULT_H

#include <string>

#include "Winners.h"

using namespace std;


class Result
{
  private:

    unsigned char distInt;

    unsigned char tricksInt;

    Winners winnersInt;

  public:

    void set(
      const unsigned char distIn,
      const unsigned char tricksIn,
      const Winners& winnersIn);

    void setEmpty(const unsigned char tricksIn);

    void update(
      const unsigned char distIn,
      const unsigned char trickNS);

    void update(const Play& play);

    void updateDist(const unsigned char distIn);

    void flip();

    bool differentTricks(const Result& res2) const;

    Compare compareByTricks(const Result& res2) const;

    void operator *= (const Result& result2);

    void operator += (const Result& result2);
  
    void multiply(Result& result) const;

    bool operator < (const Result& res2) const;

    bool operator == (const Result& res2) const;

    bool operator != (const Result& res2) const;

    Compare compareCompletely(const Result& res2) const;

    unsigned char dist() const;

    unsigned char tricks() const;

    const Winners& winners() const;

    string strHeaderEntry(
      const bool rankFlag,
      const string& title = "") const;

    string strEntry(const bool rankFlag) const;

    string strWinners() const;

    string strHeader(const string& title) const;

    string str(const bool rankFlag) const;
};

#endif
