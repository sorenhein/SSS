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
}


void Convert::reset()
{
  const string cards = "23456789TJQKABC";

  index2card.clear();
  index2card.resize(MAX_CARDS+1);

  card2index.clear();
  for (int j = 1; j <= MAX_CARDS; j++)
  {
    const string c = cards.substr(j-1, 1);

    index2card[j] = c;
    card2index[c] = j;
  }

  number2card.clear();
  number2card.resize(MAX_CARDS+1);
  for (int i = 1; i <= MAX_CARDS; i++)
  {
    number2card[i].resize(i+1);
    for (int j = 1; j <= i; j++)
      number2card[i][j] = index2card[j];
  }
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

  for (int i = 0; i < cards; i++)
  {
    const int c = h % 3;
    if (c == CONVERT_NORTH)
      north += index2card[i+1];
    else if (c == CONVERT_SOUTH)
      south += index2card[i+1];
    
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

  for (int j = 1; j <= cards; j++)
  {
    const string s = index2card[j];
    if (nindex < nlen && north.substr(nindex, 1) == s)
    {
      h = CONVERT_NORTH;
      nindex++;
    }
    else if (sindex <= slen && south.substr(sindex, 1) == s)
    {
      h = CONVERT_SOUTH;
      sindex++;
    }
    else
      h = CONVERT_OPPS;
    
    holding = 3*holding + h;
  }

  return (nindex == nlen && sindex == slen);
}

