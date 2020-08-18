#include <iostream>
#include <iomanip>
#include <sstream>

#include "Ranks.h"

#include "const.h"


Ranks::Ranks()
{
  north.clear();
  south.clear();
  opps.clear();
}


Ranks::~Ranks()
{
}


void Ranks::resize(const int cards)
{
  // Worst case.  
  north.resize(cards); 
  south.resize(cards); 
  opps.resize(cards); 
}


void Ranks::clear()
{
  for (unsigned rank = 0; rank < north.size(); rank++)
  {
    north[rank].clear();
    south[rank].clear();
    opps[rank].clear();
  }

  maxRank = -1;
}


void Ranks::setRanks(
  const int holding,
  const int cards)
{
  Ranks::clear();

  // Find the owner of the first card so that we can consider the
  // predecessor to belong to someone else.
  bool prev_is_NS = ((holding % 3) == CONVERT_OPPS);
  const int imin = (cards > 13 ? 0 : 13-cards);
  int h = holding;

  for (int i = imin; i < imin+cards; i++)
  {
    const int c = h % 3;
    if (c == CONVERT_NORTH)
    {
      if (! prev_is_NS)
        maxRank++;

      north[maxRank].count++;
      north[maxRank].cards.push_back(CARD_NAMES[i]);
      prev_is_NS = true;
    }
    else if (c == CONVERT_SOUTH)
    {
      if (! prev_is_NS)
        maxRank++;

      south[maxRank].count++;
      south[maxRank].cards.push_back(CARD_NAMES[i]);
      prev_is_NS = true;
    }
    else
    {
      if (prev_is_NS)
        maxRank++;

      opps[maxRank].count++;
      opps[maxRank].cards.push_back(CARD_NAMES[i]);
      prev_is_NS = false;
    }

    h /= 3;
  }
}


bool Ranks::dominates(
  const vector<RankInfo>& vec1,
  const vector<RankInfo>& vec2) const
{
  for (int rank = maxRank; rank >= 0; rank--)
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
  vector<char>& canonical2comb) const
{
  // For this purpose vec1 is considered "North".
  int holding = 0;
  int index = (cards > 13 ? 0 : 13-cards);

  canonical2comb.resize(cards > 13 ? cards : 13);

  for (int rank = maxRank; rank >= 0; rank--, index++)
  {
    // const char canonicalCard = CARD_NAMES[index];

    if (opps[rank].count)
    {
      for (int count = 0; count < opps[rank].count; count++)
      {
        holding = 3*holding + CONVERT_OPPS;
        canonical2comb[index] = opps[rank].cards[count];
      }
    }
    else
    {
      for (int count = 0; count < vec1[rank].count; count++)
      {
        holding = 3*holding + CONVERT_NORTH;
        canonical2comb[index] = vec1[rank].cards[count];
      }

      for (int count = 0; count < vec2[rank].count; count++)
      {
        holding = 3*holding + CONVERT_SOUTH;
        canonical2comb[index] = vec2[rank].cards[count];
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
  {
    combEntry.canonicalHolding = Ranks::canonical(south, north,
      cards, combEntry.canonical2comb);
  }
  else
  {
    combEntry.canonicalHolding = Ranks::canonical(north, south,
      cards, combEntry.canonical2comb);
  }

  combEntry.canonicalFlag = (holding == combEntry.canonicalHolding);
}


string Ranks::strRankInfo(
  const RankInfo& rankInfo,
  const string& pos) const
{
  stringstream ss;

  if (rankInfo.count == 0)
  {
    ss << setw(8) << "-" << setw(4) << "-" << setw(6) << "-";
  }
  else
  {
    string concat = "";
    for (int card = rankInfo.count-1; card >= 0; card--)
      concat += rankInfo.cards[card];

    ss << setw(8) << pos << setw(4) << rankInfo.count << setw(6) << concat;
  }

  return ss.str();
}


string Ranks::str() const
{
  stringstream ss;
  ss << "Ranks:\n";

  ss <<  right <<
    setw(8) << "North" << setw(4) << "#" << setw(6) << "cards" <<
    setw(8) << "South" << setw(4) << "#" << setw(6) << "cards" <<
    setw(8) << "Opps" <<  setw(4) << "#" << setw(6) << "cards" << 
    "\n";

  for (int rank = maxRank; rank >= 0; rank--)
    ss <<
      Ranks::strRankInfo(north[rank], "North") <<
      Ranks::strRankInfo(south[rank], "South") <<
      Ranks::strRankInfo(opps[rank], "Opps") << endl;
  
  return ss.str() + "\n";
}

