#ifndef SSS_RESULT_H
#define SSS_RESULT_H

#include <iostream>
#include <iomanip>
#include <sstream>

#include "Winners.h"

using namespace std;


struct Result
{
  unsigned char dist;
  unsigned char tricks;
  Winners winners;

  bool operator < (const Result& res2) const
  {
    return(tricks < res2.tricks);
  }

  bool operator != (const Result& res2) const
  {
    return(tricks != res2.tricks);
  }

  bool operator > (const Result& res2) const
  {
    return(tricks > res2.tricks);
  }

  void set(
    const unsigned char tricksIn,
    const WinningSide side,
    const Card& card)
  {
    tricks = tricksIn;
    winners.set(side, card);
  }

  void setEmpty(const unsigned char tricksIn)
  {
    tricks = tricksIn;
    winners.setEmpty();
  }

  string strEntry(const bool rankFlag) const
  {
    stringstream ss;
    ss << setw(4) << +tricks;
    if (rankFlag)
      ss << setw(8) << winners.strEntry();
    return ss.str();
  }
};

#endif
