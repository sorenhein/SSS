#ifndef SSS_RESULT_H
#define SSS_RESULT_H

#include <iostream>
#include <iomanip>
#include <sstream>

#include "winners/Winners.h"

using namespace std;


class Result
{
  private:

  unsigned char distInternal;
  unsigned char tricksInternal;
  Winners winnersInternal;

  public:

  unsigned char dist() const
  {
    return distInternal;
  }

  unsigned char tricks() const
  {
    return tricksInternal;
  }

  const Winners& winners() const
  {
    return winnersInternal;
  }

  // TODO Remove some of all these comparators again
  // Should it be differentTricks or differentCompletely?

  bool differentTricks(const Result& res2) const
  {
    if (tricksInternal != res2.tricksInternal)
      return true;
    else
      return (winnersInternal.compareForDeclarer(res2.winnersInternal) != WIN_EQUAL);
  }

  Compare compareByTricks(const Result& res2) const
  {
    if (tricksInternal > res2.tricksInternal)
      return WIN_FIRST;
    else if (tricksInternal < res2.tricksInternal)
      return WIN_SECOND;
    else
      return WIN_EQUAL;
  }

  bool operator < (const Result& res2) const
  {
    if (tricksInternal < res2.tricksInternal)
      return true;
    else if (tricksInternal > res2.tricksInternal)
      return false;
    else
      return (winnersInternal.compareForDeclarer(res2.winnersInternal) == WIN_SECOND);
  }

  /*
  bool fewerTricks(const Result& res2) const
  {
    return (tricks < res2.tricks);
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
  */

  Compare compareCompletely(const Result& res2) const
  {
    if (tricksInternal > res2.tricksInternal)
      return WIN_FIRST;
    else if (tricksInternal < res2.tricksInternal)
      return WIN_SECOND;
    else
      return winnersInternal.compareForDeclarer(res2.winnersInternal);
  }

  void set(
    const unsigned char distIn,
    const unsigned char tricksIn,
    const Winners& winnersIn)
  {
    distInternal = distIn;
    tricksInternal = tricksIn;
    winnersInternal = winnersIn;
  }

  void setEmpty(const unsigned char tricksIn)
  {
    tricksInternal = tricksIn;
    winnersInternal.setEmpty();
  }

  // TMP Should be a more comprehensive update method
  void updateDist(const unsigned char distIn)
  {
    distInternal = distIn;
  }

  void flip()
  {
    winnersInternal.flip();
  }

  void update(
    const unsigned char distIn,
    const unsigned char trickNS)
  {
    distInternal = distIn;
    tricksInternal += trickNS;
  }

  void update(const Play& play)
  {
    winnersInternal.update(play);
  }

  void operator *= (const Result& result2)
  {
    // Keep the "lower" one.
    if (tricksInternal > result2.tricksInternal)
      * this = result2;
    else if (tricksInternal == result2.tricksInternal)
      winnersInternal *= result2.winnersInternal;
  }

  void operator += (const Result& result2)
  {
    // Keep the "upper" one.
    if (tricksInternal < result2.tricksInternal)
      * this = result2;
    else if (tricksInternal == result2.tricksInternal)
      winnersInternal += result2.winnersInternal;
  }
  
  void multiply(Winners& winners) const
  {
    winners *= winnersInternal;
  }

  string strEntry(const bool rankFlag) const
  {
    stringstream ss;
    ss << setw(4) << +tricksInternal;

    if (rankFlag)
      ss << setw(8) << winnersInternal.strEntry();

    return ss.str();
  }

  string strHeader(const string& title) const
  {
    stringstream ss;
    if (title != "")
      ss << title << "\n";

    ss <<
      setw(4) << left << "Dist" <<
      setw(6) << "Tricks" << "\n";

    return ss.str();
  }

  string str(const bool rankFlag) const
  {
    stringstream ss;
    ss << 
      setw(4) << +distInternal << 
      setw(6) << Result::strEntry(rankFlag) << "\n";

    return ss.str();
  }
};

#endif
