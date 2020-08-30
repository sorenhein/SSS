#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include "Ranks.h"

#include "struct.h"
#include "const.h"


Ranks::Ranks()
{
  north.ranks.clear();
  south.ranks.clear();
  opps.ranks.clear();
}


Ranks::~Ranks()
{
}


void Ranks::resize(const unsigned cards)
{
  // Worst case.  
  north.ranks.resize(cards); 
  south.ranks.resize(cards); 
  opps.ranks.resize(cards); 

  for (unsigned card = 0; card < cards; card++)
  {
    north.ranks[card].cards.resize(cards);
    south.ranks[card].cards.resize(cards);
    opps.ranks[card].cards.resize(cards);
  }
}


void Ranks::clear()
{
  for (unsigned rank = 0; rank < north.ranks.size(); rank++)
  {
    north.ranks[rank].clear();
    south.ranks[rank].clear();
    opps.ranks[rank].clear();
  }

  north.len = 0;
  south.len = 0;
  opps.len = 0;

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
  bool firstNorth = true;
  bool firstSouth = true;
  bool firstOpps = true;

  for (unsigned i = imin; i < imin+cards; i++)
  {
    const unsigned c = h % 3;
    if (c == CONVERT_NORTH)
    {
      if (! prev_is_NS)
        maxRank++;

      north.update(maxRank, CARD_NAMES[i], firstNorth);
      prev_is_NS = true;
    }
    else if (c == CONVERT_SOUTH)
    {
      if (! prev_is_NS)
        maxRank++;

      south.update(maxRank, CARD_NAMES[i], firstSouth);
      prev_is_NS = true;
    }
    else
    {
      if (prev_is_NS)
        maxRank++;

      opps.update(maxRank, CARD_NAMES[i], firstOpps);
      prev_is_NS = false;
    }

    h /= 3;
  }

  north.setSingleRank();
  south.setSingleRank();
  opps.setSingleRank();
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
    if (opps.ranks[rank].count)
    {
      for (unsigned count = 0; count < opps.ranks[rank].count; count++)
      {
        holding += (holding << 1) + CONVERT_OPPS;
        canonical2comb[index] = opps.ranks[rank].cards[count];
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

  combEntry.rotateFlag = ! Ranks::dominates(north.ranks, south.ranks);
  if (combEntry.rotateFlag)
  {
    combEntry.canonicalHolding = Ranks::canonical(south.ranks, north.ranks,
      cards, combEntry.canonical2comb);
  }
  else
  {
    combEntry.canonicalHolding = Ranks::canonical(north.ranks, south.ranks,
      cards, combEntry.canonical2comb);
  }

  combEntry.canonicalFlag = (holding == combEntry.canonicalHolding);
}


bool Ranks::trivial(unsigned& terminalValue) const
{
  if (north.len == 0 && south.len == 0)
  {
    terminalValue = 0;
    return true;
  }

  if (opps.len == 0)
  {
    terminalValue = max(north.len, south.len);
    return true;
  }

  if (north.len <= 1 && south.len <= 1)
  {
    // North-South win their last trick if they have the highest card.
    terminalValue = (opps.ranks[maxRank].count == 0);
    return true;
  }

  if (opps.len <= 1)
  {
    if (opps.ranks[maxRank].count == 0)
      terminalValue = max(north.len, south.len);
    else
      terminalValue = max(north.len, south.len) - 1;
    return true;
  }

  return false;
}


bool Ranks::leadOK(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const unsigned lead) const
{
  if (partner.len == 0)
  {
    // If we have the top rank opposite a void, always play it.
    if (leader.max == maxRank && lead < maxRank)
      return false;
  }
  else if (! leader.singleRank)
  {
    // Both sides have 2+ ranks.  
    if (lead >= partner.max)
    {
      // Again, don't lead a too-high card.
      return false;;
    }
    else if (lead <= partner.min && lead > leader.min)
    {
      // If partner's lowest card is at least as high, lead lowest.
      return false;;
    }
  }
  return true;
}


bool Ranks::pardOK(
  const PositionInfo& partner,
  const unsigned toBeat,
  const unsigned pard) const
{
  // Always "play" a void.
  if (partner.len == 0)
    return true;

  // No rule concerning high cards.
  if (pard > toBeat)
    return true;

  // Play the lowest of losing cards.
  return (pard == partner.min);
}


void Ranks::setPlaysSide(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const bool firstFlag,
  vector<PlayEntry>& plays) const
{
  UNUSED(plays);

  if (leader.len == 0)
    return;

  // Always lead the singleton rank.  If both have this, lead the
  // higher one, or if they're the same, the first one.
  if (partner.singleRank &&
      (! leader.singleRank || leader.max < partner.max ||
        (leader.max == partner.max && ! firstFlag)))
    return;

  // Don't lead a card by choice that's higher than partner's best one.
  if (! leader.singleRank && partner.len > 0 && leader.min >= partner.max)
    return;
       
  for (unsigned lead = leader.min; lead <= leader.max; lead++)
  {
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    for (unsigned lho = opps.min; lho <= opps.max; lho++)
    {
      for (unsigned pard = partner.min; pard <= partner.max; pard++)
      {
        if (! Ranks::pardOK(partner, max(lead, lho), pard))
          continue;

        for (unsigned rho = opps.min; rho <= opps.max; rho++)
        {
          // Can only play a card once.
          if (opps.ranks[lho].count == 1 && lho == rho)
            continue;
          
          // log the 5 plays
          // Store trick winner, known E/W voids
          // Figure out the new holding3 and holding2
        }
      }
    }
  }
}


CombinationType Ranks::setPlays(
  vector<PlayEntry>& plays,
  unsigned& terminalValue) const
{
  // If COMB_TRIVIAL, then only terminalValue is set.
  // Otherwise, plays are set.
  // TODO: Should probably be a tree structure.
  plays.clear();

  if (Ranks::trivial(terminalValue))
    return COMB_TRIVIAL;

  Ranks::setPlaysSide(north, south, true, plays);
  Ranks::setPlaysSide(south, north, false, plays);
  return COMB_OTHER;
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
      Ranks::strRankInfo(north.ranks[rank], "North") <<
      Ranks::strRankInfo(south.ranks[rank], "South") <<
      Ranks::strRankInfo(opps.ranks[rank], "Opps") << endl;
  }
  
  return ss.str() + "\n";
}

