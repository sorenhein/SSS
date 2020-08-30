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


void Ranks::resize(const unsigned cardsIn)
{
  cards = cardsIn;

  // Worst case, leaving room for voids at rank 0.
  north.ranks.resize(cards+1); 
  south.ranks.resize(cards+1); 
  opps.ranks.resize(cards+1); 

  for (unsigned card = 0; card < cards+1; card++)
  {
    north.ranks[card].cards.resize(cards+1);
    south.ranks[card].cards.resize(cards+1);
    opps.ranks[card].cards.resize(cards+1);
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

  maxRank = 1; // First non-void rank
}


void Ranks::setRanks(const unsigned holding)
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

  north.setVoid(firstNorth);
  south.setVoid(firstSouth);
  opps.setVoid(firstOpps);

  north.setSingleRank();
  south.setSingleRank();
  opps.setSingleRank();
}


bool Ranks::dominates(
  const vector<RankInfo>& vec1,
  const vector<RankInfo>& vec2) const
{
  for (unsigned rank = maxRank+1; rank-- > 1; ) // Exclude void
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
  vector<char>& canonical2comb) const
{
  // For this purpose vec1 is considered "North".
  unsigned holding = 0;
  unsigned index = (cards > 13 ? 0 : 13-cards);

  canonical2comb.resize(cards > 13 ? cards : 13);

  for (unsigned rank = maxRank; rank > 0; rank--, index++) // Exclude void
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


void Ranks::canonicalUpdate(
  const vector<RankInfo>& vec1,
  const vector<RankInfo>& vec2,
  const vector<RankInfo>& oppsIn,
  const unsigned cardsNew,
  unsigned& holding3,
  unsigned& holding2) const
{
  // This is similar to canonical, but (a) doesn't keep track of card
  // names, and (b) generates both the binary and trinary holdings.
  // For this purpose vec1 is considered "North".
  holding3 = 0;
  holding2 = 0;
  unsigned index = (cardsNew > 13 ? 0 : 13-cardsNew);

  for (unsigned rank = maxRank; rank > 0; rank--, index++) // Exclude void
  {
    if (oppsIn[rank].count)
    {
      for (unsigned count = 0; count < oppsIn[rank].count; count++)
      {
        holding3 += (holding3 << 1) + CONVERT_OPPS;
        holding2 += holding2 + PAIR_EW;
      }
    }
    else
    {
      for (unsigned count = 0; count < vec1[rank].count; count++)
      {
        holding3 += (holding3 << 1) + CONVERT_NORTH;
        holding2 += holding2 + PAIR_NS;
      }

      for (unsigned count = 0; count < vec2[rank].count; count++)
      {
        holding3 += (holding3 << 1) + CONVERT_SOUTH;
        holding2 += holding2 + PAIR_NS;
      }
    }
  }
}


void Ranks::set(
  const unsigned holding,
  CombEntry& combEntry)
{
  Ranks::setRanks(holding);

  combEntry.rotateFlag = ! Ranks::dominates(north.ranks, south.ranks);
  if (combEntry.rotateFlag)
  {
    combEntry.canonicalHolding = Ranks::canonical(south.ranks, north.ranks,
      combEntry.canonical2comb);
  }
  else
  {
    combEntry.canonicalHolding = Ranks::canonical(north.ranks, south.ranks,
      combEntry.canonical2comb);
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
      return false;
    }
    else if (lead <= partner.min && lead > leader.min)
    {
      // If partner's lowest card is at least as high, lead lowest.
      return false;
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


void Ranks::updateHoldings(
  const vector<RankInfo>& vec1,
  const vector<RankInfo>& vec2,
  PlayEntry& play) const
{
  unsigned cardsNew = cards;

  // The leader always has a card.
  vector<RankInfo> vec1New = vec1;
  vec1New[play.lead].count--;
  cardsNew--;

  vector<RankInfo> vec2New = vec2;
  if (play.pard > 0)
  {
    vec2New[play.pard].count--;
    cardsNew--;
  }

  vector<RankInfo> oppsNew = opps.ranks;
  if (play.lho > 0)
  {
    oppsNew[play.lho].count--;
    cardsNew--;
  }

  if (play.rho > 0)
  {
    oppsNew[play.rho].count--;
    cardsNew--;
  }

  if (Ranks::dominates(vec1New, vec2New))
    Ranks::canonicalUpdate(vec1New, vec2New, oppsNew, cardsNew, 
      play.holdingNew3, play.holdingNew2);
  else
    Ranks::canonicalUpdate(vec2New, vec1New, oppsNew, cardsNew, 
      play.holdingNew3, play.holdingNew2);
}


void Ranks::setPlaysSide(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const SidePosition side,
  list<PlayEntry>& plays) const
{
  if (leader.len == 0)
    return;

  // Always lead the singleton rank.  If both have this, lead the
  // higher one, or if they're the same, the first one.
  if (partner.singleRank &&
      (! leader.singleRank || leader.max < partner.max ||
        (leader.max == partner.max && side == SIDE_SOUTH)))
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
          
          // Register the new play.
          plays.emplace_back(PlayEntry());
          PlayEntry& play = plays.back();
          play.update(side, lead, lho, pard, rho);

          Ranks::updateHoldings(leader.ranks, partner.ranks, play);
        }
      }
    }
  }
}


CombinationType Ranks::setPlays(
  list<PlayEntry>& plays,
  unsigned& terminalValue) const
{
  // If COMB_TRIVIAL, then only terminalValue is set.
  // Otherwise, plays are set.
  // TODO: Should probably be a tree structure.
  plays.clear();

  if (Ranks::trivial(terminalValue))
    return COMB_TRIVIAL;

  Ranks::setPlaysSide(north, south, SIDE_NORTH, plays);
  Ranks::setPlaysSide(south, north, SIDE_SOUTH, plays);
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

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
  {
    ss <<
      Ranks::strRankInfo(north.ranks[rank], "North") <<
      Ranks::strRankInfo(south.ranks[rank], "South") <<
      Ranks::strRankInfo(opps.ranks[rank], "Opps") << endl;
  }
  
  return ss.str() + "\n";
}

