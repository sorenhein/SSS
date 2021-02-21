#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Winner.h"


Winner::Winner()
{
  Winner::reset();
}


Winner::~Winner()
{
}


void Winner::reset()
{
  north.reset();
  south.reset();
  mode = WIN_NOT_SET;
}


void Winner::set(
  const WinningSide sideIn,
  const unsigned rankIn,
  const unsigned depthIn,
  const unsigned numberIn)
{
  // This doesn't reset the winner, so the method can be used to build
  // up a winner with more than one component if called twice.
  // In that case, NS decide among the options.

  if (sideIn == WIN_NORTH)
  {
    north.set(rankIn, depthIn, numberIn);
    if (mode == WIN_NOT_SET)
      mode = WIN_NORTH_ONLY;
    else if (mode == WIN_SOUTH_ONLY)
      mode = WIN_NS_DECIDE;
    else
    {
cout << "About to fail" << endl;
      assert(false);
    }
  }
  else if (sideIn == WIN_SOUTH)
  {
    south.set(rankIn, depthIn, numberIn);
    if (mode == WIN_NOT_SET)
      mode = WIN_SOUTH_ONLY;
    else if (mode == WIN_NORTH_ONLY)
      mode = WIN_NS_DECIDE;
    else
      assert(false);
  }
  else if (sideIn == WIN_EITHER)
  {
    // Call the method for North and South separately.
    assert(false);
  }
  else if (sideIn == WIN_NONE)
  {
    assert(rankIn == 0);
    assert(depthIn == 0);
    assert(numberIn == 0);
    north.set(rankIn, depthIn, numberIn);
    south.set(rankIn, depthIn, numberIn);
    mode = WIN_EMPTY;
  }
  else
    assert(false);
}


void Winner::operator *=(const Winner& w2)
{
  // The opponents have the choice.
  if (mode == WIN_NORTH_ONLY)
  {
    if (w2.mode == WIN_NORTH_ONLY)
      north *= w2.north;
    else if (w2.mode == WIN_SOUTH_ONLY)
    {
      if (north > w2.south)
        * this = w2;
      else if (north.sameRank(w2.south))
      {
        south = w2.south;
        mode = WIN_EW_DECIDE;
      }
      // Do nothing if North has a lower rank.
    }
    else if (w2.mode == WIN_NS_DECIDE)
    {
      if (north == w2.north)
      {
        // Rather than letting NS decide, the opponents choose the
        // side they can enforce.  Here it means changing nothing.
      }
      else
      {
        // Let's find an example of this.
        // When would EW choose NS_DECIDE rather than the north only?
        assert(false);
      }
    }
    else if (w2.mode == WIN_EW_DECIDE)
    {
      // Let's find an example of this.
      // When would EW choose EW_DECIDE rather than the north only?
      assert(false);
    }
    else if (w2.mode == WIN_EMPTY)
    {
      // OK as is.
    }
    else
      assert(false);
  }
  else if (mode == WIN_SOUTH_ONLY)
  {
    if (w2.mode == WIN_NORTH_ONLY)
    {
      if (south > w2.north)
        * this = w2;
      else if (south.sameRank(w2.north))
      {
        north = w2.north;
        mode = WIN_EW_DECIDE;
      }
      // Do nothing if South has a lower rank.
    }
    else if (w2.mode == WIN_SOUTH_ONLY)
      south *= w2.south;
    else if (w2.mode == WIN_NS_DECIDE)
    {
      if (south == w2.south)
      {
        // Rather than letting NS decide, the opponents choose the
        // side they can enforce.  Here it means changing nothing.
      }
      else
      {
        // Let's find an example of this.
        // When would EW choose NS_DECIDE rather than the north only?
        assert(false);
      }
    }
    else if (w2.mode == WIN_EW_DECIDE)
    {
      // Let's find an example of this.
      // When would EW choose EW_DECIDE rather than the north only?
        assert(false);
    }
    else if (w2.mode == WIN_EMPTY)
    {
      // OK as is.
    }
    else
      assert(false);
  }
  else if (mode == WIN_NS_DECIDE)
  {
    if (w2.mode == WIN_NORTH_ONLY)
    {
      if (north == w2.north)
      {
        // Rather than letting NS decide, the opponents choose the
        // side they can enforce.
        south.reset();
        mode = WIN_NORTH_ONLY;
      }
      else
      {
        // Example?
        assert(false);
      }
    }
    else if (w2.mode == WIN_SOUTH_ONLY)
    {
      if (south == w2.south)
      {
        // Rather than letting NS decide, the opponents choose the
        // side they can enforce.
        north.reset();
        mode = WIN_SOUTH_ONLY;
      }
      else
      {
        // Example?
        assert(false);
      }
    }
    else if (w2.mode == WIN_NS_DECIDE)
    {
      // Example?
      assert(false);
    }
    else if (w2.mode == WIN_EW_DECIDE)
    {
      // Example?
      assert(false);
    }
    else if (w2.mode == WIN_EMPTY)
    {
      // OK as is.
    }
    else
      assert(false);
  }
  else if (mode == WIN_EW_DECIDE)
  {
    if (w2.mode == WIN_NORTH_ONLY)
    {
      // Example?
      assert(false);
    }
    else if (w2.mode == WIN_SOUTH_ONLY)
    {
      // Example?
      assert(false);
    }
    else if (w2.mode == WIN_NS_DECIDE)
    {
      // Example?
      assert(false);
    }
    else if (w2.mode == WIN_EW_DECIDE)
    {
      // Example?
      assert(false);
    }
    else if (w2.mode == WIN_EMPTY)
    {
      // OK as is.
    }
    else
      assert(false);
  }
  else if (mode == WIN_EMPTY)
  {
    * this = w2;
  }
  else
    assert(false);
}


void Winner::flip()
{
  // Flips North and South.
  SideWinner tmp = north;
  north = south;
  south = north;

  if (mode == WIN_NORTH_ONLY)
    mode = WIN_SOUTH_ONLY;
  else if (mode == WIN_SOUTH_ONLY)
    mode = WIN_NORTH_ONLY;
}


void Winner::update(
  const vector<Winner>& northOrder,
  const vector<Winner>& southOrder,
  const Winner& currBest,
  const unsigned trickNS)
{
  if (mode == WIN_NORTH_ONLY)
  {
    // This may also change the winning side.
    assert(north.number < northOrder.size());
    * this = northOrder[north.number];
  }
  else if (mode == WIN_SOUTH_ONLY)
  {
    // This may also change the winning side.
    assert(south.number < southOrder.size());
    * this = southOrder[south.number];
  }
  else if (mode == WIN_NS_DECIDE || mode == WIN_EW_DECIDE)
  {
    assert(north.number < northOrder.size());
    assert(south.number < southOrder.size());
    north = northOrder[north.number].north;
    south = southOrder[south.number].south;

    // As a result of the mapping to parent ranks, North and South
    // may actually be different ranks now.  As North-South choose,
    // they only keep the higher one.
    if (north.rank > south.rank)
    {
      // Only North survives.
      south.reset();
      mode = WIN_NORTH_ONLY;
    }
    else if (south.rank > north.rank)
    {
      north.reset();
      mode = WIN_SOUTH_ONLY;
    }

    // * this = northOrder[north.number];
    // * this = southOrder[south.number];
  }
  else if (mode == WIN_EMPTY)
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
  else // if (mode != WIN_EMPTY)
    // Should not happen.
    assert(false);
}


string Winner::str() const
{
  stringstream ss;

  ss << "Winner ";
  if (mode == WIN_NORTH_ONLY)
    ss << setw(8) << "North";
  else if (mode == WIN_SOUTH_ONLY)
    ss << setw(8) << "South";
  else if (mode == WIN_NS_DECIDE)
    ss << setw(8) << "NSdec";
  else if (mode == WIN_EW_DECIDE)
    ss << setw(8) << "EWdec";
  else if (mode == WIN_EMPTY)
    ss << setw(8) << "None";

  return ss.str() + "\n";
}


string Winner::strSingleSided(
  const string& name,
  const SideWinner& winner) const
{
  stringstream ss;
  ss << name << hex << uppercase << winner.rank << dec;
  if (winner.depth == 2)
    ss << "'";
  else if (winner.depth == 3)
    ss << "\"";
  else if (winner.depth > 3)
    ss << "+";
  return ss.str();
}


string Winner::strEntry() const
{
  stringstream ss;

  if (mode == WIN_NORTH_ONLY)
    return Winner::strSingleSided("N", north);
  else if (mode == WIN_SOUTH_ONLY)
    return Winner::strSingleSided("S", south);

  if (mode == WIN_NS_DECIDE)
  {
    ss << "O" << hex << uppercase << north.rank << dec; // Offense
  }
  else if (mode == WIN_EW_DECIDE)
  {
    ss << "O" << hex << uppercase << north.rank << dec; // Defense
  }
  else
  {
    ss << "-";
  }
  
  return ss.str();
}

