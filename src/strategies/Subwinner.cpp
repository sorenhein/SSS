#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Subwinner.h"


Subwinner::Subwinner()
{
  Subwinner::reset();
}


Subwinner::~Subwinner()
{
}


void Subwinner::reset()
{
  north.reset();
  south.reset();
  mode = SUBWIN_NOT_SET;
}


void Subwinner::set(
  const WinningSide sideIn,
  const unsigned rankIn,
  const unsigned depthIn,
  const unsigned numberIn)
{
  // This doesn't reset the winner, so the method can be used to build
  // up a winner with more than one component if called twice.
  // In that case, NS decide among the options.

  assert(mode == SUBWIN_NOT_SET);
  if (sideIn == WIN_NORTH)
  {
    north.set(rankIn, depthIn, numberIn);
    mode = SUBWIN_NORTH_ONLY;
  }
  else if (sideIn == WIN_SOUTH)
  {
    south.set(rankIn, depthIn, numberIn);
    mode = SUBWIN_SOUTH_ONLY;
  }
  else
    assert(false);
}


bool Subwinner::operator == (const Subwinner& w2) const
{
  if (mode != w2.mode)
    return false;
  if (mode == SUBWIN_NOT_SET)
    return true;
  if (mode != SUBWIN_SOUTH_ONLY && north != w2.north)
    return false;
  if (mode != SUBWIN_NORTH_ONLY && south != w2.south)
    return false;

  return true;
}


bool Subwinner::operator != (const Subwinner& w2) const
{
  return ! (* this == w2);
}


void Subwinner::operator *= (const Subwinner& w2)
{
  // The opponents have the choice.

  if (w2.mode == SUBWIN_NOT_SET)
  {
    // OK as is.
    return;
  }
  else if (mode == SUBWIN_NOT_SET)
  {
    * this = w2;
    return;
  }

  if (mode == SUBWIN_NORTH_ONLY)
  {
    if (w2.mode == SUBWIN_NORTH_ONLY)
      north *= w2.north;
    else if (w2.mode == SUBWIN_SOUTH_ONLY)
    {
      if (north.rankExceeds(w2.south))
        * this = w2;
      else if (north.rankSame(w2.south))
      {
        south = w2.south;
        mode = SUBWIN_BOTH;
      }
      else
      {
        // Do nothing if North has a lower rank.
      }
    }
    else if (w2.mode == SUBWIN_BOTH)
    {
      if (w2.north >= north)
      {
        // The current winner is no higher, and it has no option
        // of a South play at NS's discretion.  So the current winner 
        // dominates from EW's perspective, and we change nothing.
      }
      else
      {
        // Let's find an example of this.
        // When would EW choose NS_DECIDE rather than the north only?
        cout << "Position 1" <<  endl;
        cout << Subwinner::strDebug();
        cout << w2.strDebug() << flush;
        assert(false);
      }
    }
    else
    {
      cout << "Position 3" <<  endl;
      cout << Subwinner::strDebug();
      cout << w2.strDebug();
      assert(false);
    }
  }
  else if (mode == SUBWIN_SOUTH_ONLY)
  {
    if (w2.mode == SUBWIN_NORTH_ONLY)
    {
      if (south.rankExceeds(w2.north))
        * this = w2;
      else if (south.rankSame(w2.north))
      {
        north = w2.north;
        mode = SUBWIN_BOTH;
      }
      else
      {
        // Do nothing if South has a lower rank.
      }
    }
    else if (w2.mode == SUBWIN_SOUTH_ONLY)
      south *= w2.south;
    else if (w2.mode == SUBWIN_BOTH)
    {
      if (w2.south >= south)
      {
        // The current winner is no higher, and it has no option
        // of a North play at NS's discretion.  So the current winner 
        // dominates from EW's perspective, and we change nothing.
      }
      else
      {
        // Let's find an example of this.
        // When would EW choose NS_DECIDE rather than the north only?
        cout << "Position 4" <<  endl;
        cout << Subwinner::strDebug();
        cout << w2.strDebug();
        assert(false);
      }
    }
    else
    {
      cout << "Position 6" <<  endl;
      cout << Subwinner::strDebug();
      cout << w2.strDebug();
      assert(false);
    }
  }
  else if (mode == SUBWIN_BOTH)
  {
    if (w2.mode == SUBWIN_NORTH_ONLY)
    {
      if (north >= w2.north)
      {
        // The w2 winner is no higher, and it has no option
        // of a South play at NS's discretion.  So the w2 winner 
        // dominates from EW's perspective.
        * this = w2;
      }
      else
      {
        // Example?
        cout << "Position 7" <<  endl;
        cout << Subwinner::strDebug();
        cout << w2.strDebug();
        assert(false);
      }
    }
    else if (w2.mode == SUBWIN_SOUTH_ONLY)
    {
      if (south >= w2.south)
      {
        // The w2 winner is no higher, and it has no option
        // of a South play at NS's discretion.  So the w2 winner 
        // dominates from EW's perspective.
        * this = w2;
      }
      else
      {
        // Example?
        cout << "Position 8" <<  endl;
        cout << Subwinner::strDebug();
        cout << w2.strDebug();
        assert(false);
      }
    }
    else if (w2.mode == SUBWIN_BOTH)
    {
      // TODO Some kind of domination?
      const WinnerCompare cmpNorth = north.compare(w2.north);
      const WinnerCompare cmpSouth = south.compare(w2.north);

      if ((cmpNorth == WIN_SECOND || cmpNorth == WIN_EQUAL) &&
          (cmpSouth == WIN_SECOND || cmpSouth == WIN_EQUAL))
      {
        // OK as is.
      }
      else if ((cmpNorth == WIN_FIRST || cmpNorth == WIN_EQUAL) &&
          (cmpSouth == WIN_FIRST || cmpSouth == WIN_EQUAL))
      {
        * this = w2;
      }
      else
      {
        // Example?
        cout << "Position 9" <<  endl;
        cout << Subwinner::strDebug();
        cout << w2.strDebug() << endl;
        assert(false);
      }
    }
    else
    {
      cout << "Position 11" <<  endl;
      cout << Subwinner::strDebug();
      cout << w2.strDebug();
      assert(false);
    }
  }
  else
  {
    cout << "Position 17" <<  endl;
    cout << Subwinner::strDebug();
    cout << w2.strDebug();
    assert(false);
  }
}


void Subwinner::flip()
{
  if (mode == SUBWIN_NOT_SET)
    return;

  // Flips North and South.
  Sidewinner tmp = north;
  north = south;
  south = north;

  if (mode == SUBWIN_NORTH_ONLY)
    mode = SUBWIN_SOUTH_ONLY;
  else if (mode == SUBWIN_SOUTH_ONLY)
    mode = SUBWIN_NORTH_ONLY;
}


void Subwinner::update(
  const vector<Subwinner>& northOrder,
  const vector<Subwinner>& southOrder,
  const Subwinner& currBest,
  const unsigned trickNS)
{
  if (mode == SUBWIN_NORTH_ONLY)
  {
    // This may also change the winning side.
    assert(north.no() < northOrder.size());
    * this = northOrder[north.no()];
  }
  else if (mode == SUBWIN_SOUTH_ONLY)
  {
    // This may also change the winning side.
    assert(south.no() < southOrder.size());
    * this = southOrder[south.no()];
  }
  else if (mode == SUBWIN_BOTH)
  {
    assert(north.no() < northOrder.size());
    assert(south.no() < southOrder.size());
    north = northOrder[north.no()].north;
    south = southOrder[south.no()].south;

    // As a result of the mapping to parent ranks, North and South
    // may actually be different ranks now.  As North-South choose,
    // they only keep the higher one.
    if (north.rankExceeds(south))
    {
      // Only North survives.
      south.reset();
      mode = SUBWIN_NORTH_ONLY;
    }
    else if (south.rankExceeds(north))
    {
      north.reset();
      mode = SUBWIN_SOUTH_ONLY;
    }
  }
  else if (mode == SUBWIN_NOT_SET)
  {
    // We weren't winning anything by ranks.
    if (trickNS)
    {
      // The current winner is the overall winner.
      * this = currBest;
    }
    else
    {
      // Stick with the empty winner.
    }
  }
  else
    // Should not happen.
    assert(false);
}


string Subwinner::str() const
{
  if (mode == SUBWIN_NORTH_ONLY)
    return north.str("N");
  else if (mode == SUBWIN_SOUTH_ONLY)
    return south.str("S");
  else if (mode == SUBWIN_NOT_SET)
    return "-";
  else
    // Not ideal
    return north.str("B");
}


string Subwinner::strDebug() const
{
  stringstream ss;
  if (mode == SUBWIN_NORTH_ONLY || mode == SUBWIN_BOTH)
    ss << north.strDebug("N");
  if (mode == SUBWIN_SOUTH_ONLY || mode == SUBWIN_BOTH)
    ss << south.strDebug("S");
  return ss.str();
}

