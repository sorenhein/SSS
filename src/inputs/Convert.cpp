#include <iostream>
#include <iomanip>
#include <sstream>

#include "Convert.h"

#include "../const.h"


Convert::Convert()
{
  Convert::reset();
}


Convert::~Convert()
{
  cout << "Convert destructor" << endl;
}


void Convert::reset()
{
}


bool Convert::holding2cards(
  const int holding,
  const int cards,
  string& north,
  string& south)
{
  int h = holding;
  north = "";
  south = "";

  const int imin = (cards > 13 ? 0 : 13-cards);
  for (int i = imin; i < imin+cards; i++)
  {
    const int c = h % 3;
    if (c == CONVERT_NORTH)
      north = CARD_NAMES[i] + north;
    else if (c == CONVERT_SOUTH)
      south = CARD_NAMES[i] + south;
    
    h /= 3;
  }

  return (h == 0);
}


bool Convert::cards2holding(
  const string& north,
  const string& south,
  const int cards,
  int& holding)
{
  const int nlen = north.length();
  const int slen = south.length();
  holding = 0;

  int nindex = 0;
  int sindex = 0;
  int h;

  // Generally start at the ace, even if the suit is shorter.
  // So it's the deuce and not the ace missing from a 12-card suit.
  // For longer suits, start from the top.
  // So in a hypothetical 15-card suit the first card is the "C".
  const int jmax = (cards > 13 ? cards : 13);
  int count = 0;

  // First do the non-x cards from the top down.
  for (int j = jmax; j > jmax-cards; j--)
  {
    const char nextCard = CARD_NAMES[j-1];
    const char ncard = (nindex == nlen ? ' ' : north.at(nindex));
    const char scard = (sindex == slen ? ' ' : south.at(sindex));

    if (ncard == nextCard)
    {
      h = CONVERT_NORTH;
      nindex++;
    }
    else if (scard == nextCard)
    {
      h = CONVERT_SOUTH;
      sindex++;
    }
    else
      h = CONVERT_OPPS;
    
    holding = 3*holding + h;
    count++;

    if ((nindex == nlen || ncard == 'x') &&
        (sindex == slen || scard == 'x'))
      break;
  }

  const int num_x = nlen - nindex + slen - sindex;
  if (num_x == 0)
    return true;

  // If there are false characters in the input, this test may trigger.
  if (count + num_x > cards)
    return false;

  // Shift up the holding before filling in the x's.
  for (int i = 0; i < cards - count - num_x; i++)
    holding = 3*holding + CONVERT_OPPS;

  // North gets the first x's.
  for (int i = nindex; i < nlen; i++)
  {
    if (north.at(i) != 'x')
      return false;
    holding = 3*holding + CONVERT_NORTH;
  }

  for (int i = sindex; i < slen; i++)
  {
    if (south.at(i) != 'x')
      return false;
    holding = 3*holding + CONVERT_SOUTH;
  }
  
  return true;
}

