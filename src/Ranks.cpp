#include <iostream>
#include <iomanip>
#include <sstream>

#include "Ranks.h"

#include "const.h"


Ranks::Ranks()
{
  Ranks::reset();
}


Ranks::~Ranks()
{
}


void Ranks::reset()
{
  north.clear();
  south.clear();
  opps.clear();
}


void Ranks::setRanks(
  const int holding,
  const int cards)
{
  // Worst case.  
  north.resize(cards); 
  south.resize(cards); 
  opps.resize(cards); 

  // Find the owner of the first card so that we can consider the
  // predecessor to belong to someone else.
  bool prev_is_NS = ((holding % 3) == CONVERT_OPPS);
  int runningRank = -1;

  const int imin = (cards > 13 ? 0 : 13-cards);
  int h = holding;

  for (int i = imin; i < imin+cards; i++)
  {
    const int c = h % 3;
    if (c == CONVERT_NORTH)
    {
      if (! prev_is_NS)
        runningRank++;

      north[runningRank].count++;
      north[runningRank].cards.push_back(to_string(CARD_NAMES[i]));
      prev_is_NS = true;
    }
    else if (c == CONVERT_SOUTH)
    {
      if (! prev_is_NS)
        runningRank++;

      south[runningRank].count++;
      south[runningRank].cards.push_back(to_string(CARD_NAMES[i]));
      prev_is_NS = true;
    }
    else
    {
      if (prev_is_NS)
        runningRank++;

      opps[runningRank].count++;
      opps[runningRank].cards.push_back(to_string(CARD_NAMES[i]));
      prev_is_NS = false;
    }

    h /= 3;
  }

  north.resize(runningRank+1);
  south.resize(runningRank+1);
  opps.resize(runningRank+1);
}


bool Ranks::dominates(
  const vector<RankInfo>& vec1,
  const vector<RankInfo>& vec2) const
{
  for (int rank = vec1.size()-1; rank >= 0; rank--)
  {
    if (vec1[rank].count > vec2[rank].count)
      return true;
    if (vec1[rank].count < vec2[rank].count)
      return false;
  }
  return true;
}


int Ranks::canonical(
  const vector<RankInfo>& vec1,
  const vector<RankInfo>& vec2,
  const int cards,
  map<string, string>& canonical2comb) const
{
  // For this purpose vec1 is considered "North".
  int holding = 0;
  int index = (cards > 13 ? 0 : 13-cards);

  for (unsigned rank = 0; rank < vec1.size(); rank++, index++)
  {
    const string canonicalCard = to_string(CARD_NAMES[index]);

    if (opps[rank].count)
    {
      for (int count = 0; count < opps[rank].count; count++)
      {
        holding = 3*holding + CONVERT_OPPS;
        canonical2comb[canonicalCard] = opps[rank].cards[count];
      }
    }
    else
    {
      for (int count = 0; count < vec1[rank].count; count++)
      {
        holding = 3*holding + CONVERT_NORTH;
        canonical2comb[canonicalCard] = vec1[rank].cards[count];
      }

      for (int count = 0; count < vec2[rank].count; count++)
      {
        holding = 3*holding + CONVERT_SOUTH;
        canonical2comb[canonicalCard] = vec2[rank].cards[count];
      }
    }
  }
  return holding;
}


void Ranks::set(
  const int holding,
  const int cards,
  CombEntry& combEntry)
{
  Ranks::setRanks(holding, cards);

  combEntry.rotateFlag = ! Ranks::dominates(north, south);
  if (combEntry.rotateFlag)
    combEntry.canonicalHolding = Ranks::canonical(south, north,
      cards, combEntry.canonical2comb);
  else
    combEntry.canonicalHolding = Ranks::canonical(north, south,
      cards, combEntry.canonical2comb);

  combEntry.canonicalFlag = (holding == combEntry.canonicalHolding);
}

