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


void Winner::setFull(
  const WinningSide sideIn,
  const unsigned rankIn,
  const unsigned depthIn,
  const unsigned numberIn)
{
  side = sideIn;
  rank = rankIn;
  depth = depthIn;
  number = numberIn;
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


string Winner::strEntry() const
{
  stringstream ss;

  if (side == WIN_NORTH)
    ss << "N";
  else if (side == WIN_SOUTH)
    ss << "S";
  else if (side == WIN_EITHER)
    ss << "B";
  else
    ss << "-";
  
  ss << hex << uppercase << rank << dec;
  if (depth == 2)
    ss << "'";
  else if (depth == 3)
    ss << "\"";
  else if (depth > 3)
    ss << "+";
  
  return ss.str();
}

