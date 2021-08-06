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

  // TODO Remove some of all these comparators again

  bool fewerTricks(const Result& res2) const
  {
    return (tricks < res2.tricks);
  }

  bool differentTricks(const Result& res2) const
  {
    if (tricks != res2.tricks)
      return true;
    else
      return (winners.compareForDeclarer(res2.winners) != WIN_EQUAL);
  }

  bool moreTricks(const Result& res2) const
  {
    if (tricks > res2.tricks)
      return true;
    else if (tricks < res2.tricks)
      return false;
    else
      return (winners.compareForDeclarer(res2.winners) == WIN_FIRST);
  }

  WinnerCompare compareByTricks(const Result& res2) const
  {
    if (tricks > res2.tricks)
      return WIN_FIRST;
    else if (tricks < res2.tricks)
      return WIN_SECOND;
    else
      return WIN_EQUAL;
  }

  bool operator < (const Result& res2) const
  {
    if (tricks < res2.tricks)
      return true;
    else if (tricks > res2.tricks)
      return false;
    else
      return (winners.compareForDeclarer(res2.winners) == WIN_SECOND);
  }

  bool operator != (const Result& res2) const
  {
    if (tricks != res2.tricks)
      return true;
    else
      return (winners.compareForDeclarer(res2.winners) != WIN_EQUAL);
  }

  bool operator > (const Result& res2) const
  {
    if (tricks > res2.tricks)
      return true;
    else if (tricks < res2.tricks)
      return false;
    else
      return (winners.compareForDeclarer(res2.winners) == WIN_FIRST);
  }

  WinnerCompare compareCompletely(const Result& res2) const
  {
    if (tricks > res2.tricks)
      return WIN_FIRST;
    else if (tricks < res2.tricks)
      return WIN_SECOND;
    else
      return winners.compareForDeclarer(res2.winners);
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
