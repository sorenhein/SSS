#include <iostream>
#include <iomanip>
#include <sstream>

#include "Ranks.h"

#include "struct.h"
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


void Ranks::resize(const unsigned cards)
{
  // Worst case.  
  north.resize(cards); 
  south.resize(cards); 
  opps.resize(cards); 

  for (unsigned card = 0; card < cards; card++)
  {
    north[card].cards.resize(cards);
    south[card].cards.resize(cards);
    opps[card].cards.resize(cards);
  }
}


void Ranks::clear()
{
  for (unsigned rank = 0; rank < north.size(); rank++)
  {
    north[rank].clear();
    south[rank].clear();
    opps[rank].clear();
  }

  maxRank = 0;
}


void Ranks::setRanks(
  const unsigned holding,
  const unsigned cards)
{
  Ranks::clear();

  // Find the owner of the first card so that we can consider the
  // predecessor to belong to someone else.
  bool prev_is_NS = ((holding % 3) != CONVERT_OPPS);
  const unsigned imin = (cards > 13 ? 0 : 13-cards);
  unsigned h = holding;

  for (unsigned i = imin; i < imin+cards; i++)
  {
    const unsigned c = h % 3;
    if (c == CONVERT_NORTH)
    {
      if (! prev_is_NS)
        maxRank++;

      north[maxRank].add(CARD_NAMES[i]);
      prev_is_NS = true;
    }
    else if (c == CONVERT_SOUTH)
    {
      if (! prev_is_NS)
        maxRank++;

      south[maxRank].add(CARD_NAMES[i]);
      prev_is_NS = true;
    }
    else
    {
      if (prev_is_NS)
        maxRank++;

      opps[maxRank].add(CARD_NAMES[i]);
      prev_is_NS = false;
    }

    h /= 3;
  }
}


bool Ranks::dominates(
  const vector<RankInfo>& vec1,
  const vector<RankInfo>& vec2) const
{
  for (unsigned rank = maxRank+1; rank-- > 0; )
  {
    if (vec1[rank].count > vec2[rank].count)
      return true;
    if (vec1[rank].count < vec2[rank].count)
      return false;
  }
  return true;
}


unsigned Ranks::canonical(
  const vector<RankInfo>& vec1,
  const vector<RankInfo>& vec2,
  const unsigned cards,
  vector<char>& canonical2comb) const
{
  // For this purpose vec1 is considered "North".
  unsigned holding = 0;
  unsigned index = (cards > 13 ? 0 : 13-cards);

  canonical2comb.resize(cards > 13 ? cards : 13);

  for (unsigned rank = maxRank+1; rank-- > 0; index++)
  {
    if (opps[rank].count)
    {
      for (unsigned count = 0; count < opps[rank].count; count++)
      {
        holding += (holding << 1) + CONVERT_OPPS;
        canonical2comb[index] = opps[rank].cards[count];
      }
    }
    else
    {
      for (unsigned count = 0; count < vec1[rank].count; count++)
      {
        holding += (holding << 1) + CONVERT_NORTH;
        canonical2comb[index] = vec1[rank].cards[count];
      }

      for (unsigned count = 0; count < vec2[rank].count; count++)
      {
        holding += (holding << 1) + CONVERT_SOUTH;
        canonical2comb[index] = vec2[rank].cards[count];
      }
    }
  }
  return holding;
}


void Ranks::set(
  const unsigned holding,
  const unsigned cards,
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


const vector<RankInfo>& Ranks::oppsRank() const
{
  return opps;
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
    for (unsigned card = rankInfo.count; card-- > 0; )
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

  for (unsigned rank = maxRank+1; rank-- > 0; )
  {
    ss <<
      Ranks::strRankInfo(north[rank], "North") <<
      Ranks::strRankInfo(south[rank], "South") <<
      Ranks::strRankInfo(opps[rank], "Opps") << endl;
  }
  
  return ss.str() + "\n";
}

