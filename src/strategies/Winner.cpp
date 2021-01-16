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
  rank = 0;
  side = WIN_NONE;
}


void Winner::set(
  const unsigned rankIn,
  const WinningSide sideIn)
{
  rank = rankIn;
  side = sideIn;
}


string Winner::str() const
{
  stringstream ss;

  ss << "Winner ";
  if (side == WIN_NORTH)
    ss << setw(8) << "North";
  else if (side == WIN_SOUTH)
    ss << setw(8) << "South";
  else if (side == WIN_EITHER)
    ss << setw(8) << "Either";
  else
    ss << setw(8) << "None";

  return ss.str() + "\n";
}

