#include <iostream>
#include <cassert>

#include "Convert.h"

#include "../const.h"
#include "../utils/table.h"


Convert::Convert()
{
  Convert::reset();
}


Convert::~Convert()
{
}


void Convert::reset()
{
}


bool Convert::holding2cards(
  const unsigned holding,
  const unsigned cards,
  string& north,
  string& south)
{
  unsigned h = holding;
  north = "";
  south = "";

  const unsigned imin = (cards > 13 ? 0 : 13-cards);
  for (unsigned i = imin; i < imin+cards; i++)
  {
    assert(i < MAX_CARDS);
    const unsigned c = h % 3;
    if (c == SIDE_NORTH)
      north = CARD_NAMES[i] + north;
    else if (c == SIDE_SOUTH)
      south = CARD_NAMES[i] + south;
    
    h /= 3;
  }

  return (h == 0);
}


bool Convert::cards2holding(
  const string& north,
  const string& south,
  const unsigned cards,
  unsigned& holding)
{
  const unsigned nlen = north.length();
  const unsigned slen = south.length();
  holding = 0;

  unsigned nindex = 0;
  unsigned sindex = 0;
  unsigned h;

  // Generally start at the ace, even if the suit is shorter.
  // So it's the deuce and not the ace missing from a 12-card suit.
  // For longer suits, start from the top.
  // So in a hypothetical 15-card suit the first card is the "C".
  const unsigned jmax = (cards > 13 ? cards : 13);
  unsigned count = 0;

  // First do the non-x cards from the top down.
  for (unsigned j = jmax; j > jmax-cards; j--)
  {
    assert(j <= MAX_CARDS);
    const char nextCard = CARD_NAMES[j-1];
    const char ncard = (nindex >= nlen ? ' ' : north.at(nindex));
    const char scard = (sindex >= slen ? ' ' : south.at(sindex));

    if (ncard == nextCard)
    {
      h = SIDE_NORTH;
      nindex++;
    }
    else if (scard == nextCard)
    {
      h = SIDE_SOUTH;
      sindex++;
    }
    else
      h = SIDE_OPPS;
    
    holding = 3*holding + h;
    count++;

    if ((nindex == nlen || ncard == 'x') &&
        (sindex == slen || scard == 'x'))
      break;
  }

  const unsigned num_x = nlen - nindex + slen - sindex;
  if (num_x == 0)
    return true;

  // If there are false characters in the input, this test may trigger.
  if (count + num_x > cards)
    return false;

  // Shift up the holding before filling in the x's.
  for (unsigned i = 0; i < cards - count - num_x; i++)
    holding = 3*holding + SIDE_OPPS;

  // North gets the first x's.
  for (unsigned i = nindex; i < nlen; i++)
  {
    if (north.at(i) != 'x')
      return false;
    holding = 3*holding + SIDE_NORTH;
  }

  for (unsigned i = sindex; i < slen; i++)
  {
    if (south.at(i) != 'x')
      return false;
    holding = 3*holding + SIDE_SOUTH;
  }
  
  return true;
}

