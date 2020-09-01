#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include "Ranks2.h"

#include "struct.h"
#include "const.h"

#define BIGINT 99


Ranks2::Ranks2()
{
  north.ranks.clear();
  south.ranks.clear();
  opps.ranks.clear();
}


Ranks2::~Ranks2()
{
}


void Ranks2::resize(const unsigned cardsIn)
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


void Ranks2::clear()
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

  full2reducedNorth.clear();
  full2reducedSouth.clear();
  full2reducedOpps.clear();
}


void Ranks2::setRanks(const unsigned holding)
{
  Ranks2::clear();
  full2reducedNorth.resize(cards+1, BIGINT);
  full2reducedSouth.resize(cards+1, BIGINT);
  full2reducedOpps.resize(cards+1, BIGINT);

  // Find the owner of the first card so that we can consider the
  // predecessor to belong to someone else.
  CardPosition prev_is = static_cast<CardPosition>(holding % 3);
  const unsigned imin = (cards > 13 ? 0 : 13-cards);
  unsigned h = holding;

  unsigned posNorth = 0; // So actual ranks will start from 1
  unsigned posSouth = 0;
  unsigned posOpps = 0;

  bool firstNorth = true;
  bool firstSouth = true;
  bool firstOpps = true;

  for (unsigned i = imin; i < imin+cards; i++)
  {
    const unsigned c = h % 3;
    if (c == CONVERT_NORTH)
    {
      if (prev_is == CONVERT_OPPS)
      {
        maxRank++;
        posNorth++;
      }

      north.update(posNorth, maxRank, CARD_NAMES[i], firstNorth);
      full2reducedNorth[maxRank] = posNorth;
      prev_is = CONVERT_NORTH;
    }
    else if (c == CONVERT_SOUTH)
    {
      if (prev_is == CONVERT_OPPS)
      {
        maxRank++;
        posSouth++;
      }

      south.update(posSouth, maxRank, CARD_NAMES[i], firstSouth);
      full2reducedSouth[maxRank] = posSouth;
      prev_is = CONVERT_SOUTH;
    }
    else
    {
      if (prev_is != CONVERT_OPPS)
      {
        maxRank++;
        posOpps++;
      }

      opps.update(posOpps, maxRank, CARD_NAMES[i], firstOpps);
      full2reducedOpps[maxRank] = posOpps;
      prev_is = CONVERT_OPPS;
    }

    h /= 3;
  }

  north.ranks.resize(posNorth+1);
  south.ranks.resize(posSouth+1);
  opps.ranks.resize(posOpps+1);

  north.setVoid(false);
  south.setVoid(false);
  opps.setVoid(true);

  north.setSingleRank();
  south.setSingleRank();
  opps.setSingleRank();
}


bool Ranks2::dominates(
  const vector<RankInfo2>& vec1,
  const vector<RankInfo2>& vec2) const
{
  for (unsigned pos = vec1.size(); pos-- > 0; )
  {
    if (vec1[pos].rank > vec2[pos].rank)
      return true;
    if (vec1[pos].rank < vec2[pos].rank)
      return false;
    if (vec1[pos].count > vec2[pos].count)
      return true;
    if (vec1[pos].count < vec2[pos].count)
      return false;
  }
  return true;
}


unsigned Ranks2::canonical(
  const vector<RankInfo2>& vec1,
  const vector<RankInfo2>& vec2,
  vector<char>& canonical2comb) const
{
  // For this purpose vec1 is considered "North".
  unsigned holding = 0;
  unsigned index = (cards > 13 ? 0 : 13-cards);

  canonical2comb.resize(cards > 13 ? cards : 13);

  for (unsigned rank = maxRank; rank > 0; rank--, index++) // Exclude void
  {
    if (full2reducedOpps[rank] < BIGINT)
    {
      const unsigned pos = full2reducedOpps[rank];
      for (unsigned count = 0; count < opps.ranks[pos].count; count++)
      {
        holding += (holding << 1) + CONVERT_OPPS;
        canonical2comb[index] = opps.ranks[pos].cards[count];
      }
    }
    else 
    {
      if (full2reducedNorth[rank] < BIGINT)
      {
        const unsigned pos = full2reducedNorth[rank];
        for (unsigned count = 0; count < vec1[pos].count; count++)
        {
          holding += (holding << 1) + CONVERT_NORTH;
          canonical2comb[index] = vec1[pos].cards[count];
        }
      }

      if (full2reducedSouth[rank] < BIGINT)
      {
        const unsigned pos = full2reducedSouth[rank];
        for (unsigned count = 0; count < vec2[pos].count; count++)
        {
          holding += (holding << 1) + CONVERT_SOUTH;
          canonical2comb[index] = vec2[pos].cards[count];
        }
      }
    }
  }
  return holding;
}


void Ranks2::canonicalUpdate(
  const vector<RankInfo2>& vec1,
  const vector<RankInfo2>& vec2,
  const vector<RankInfo2>& oppsIn,
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
    if (full2reducedOpps[rank] < BIGINT)
    {
      const unsigned pos = full2reducedOpps[rank];
      for (unsigned count = 0; count < oppsIn[pos].count; count++)
      {
        holding3 += (holding3 << 1) + CONVERT_OPPS;
        holding2 += holding2 + PAIR_EW;
      }
    }
    else
    {
      if (full2reducedNorth[rank] < BIGINT)
      {
        const unsigned pos = full2reducedNorth[rank];
        for (unsigned count = 0; count < vec1[pos].count; count++)
        {
          holding3 += (holding3 << 1) + CONVERT_NORTH;
          holding2 += holding2 + PAIR_NS;
        }
      }

      if (full2reducedSouth[rank] < 2*cards)
      {
        const unsigned pos = full2reducedSouth[rank];
        for (unsigned count = 0; count < vec2[pos].count; count++)
        {
          holding3 += (holding3 << 1) + CONVERT_SOUTH;
          holding2 += holding2 + PAIR_NS;
        }
      }
    }
  }
}


void Ranks2::set(
  const unsigned holding,
  CombEntry& combEntry)
{
  Ranks2::setRanks(holding);

  combEntry.rotateFlag = ! Ranks2::dominates(north.ranks, south.ranks);
  if (combEntry.rotateFlag)
  {
    combEntry.canonicalHolding = Ranks2::canonical(south.ranks, north.ranks,
      combEntry.canonical2comb);
  }
  else
  {
    combEntry.canonicalHolding = Ranks2::canonical(north.ranks, south.ranks,
      combEntry.canonical2comb);
  }

  combEntry.canonicalFlag = (holding == combEntry.canonicalHolding);
}


bool Ranks2::trivial(unsigned& terminalValue) const
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
    terminalValue = (opps.max != maxRank);
    return true;
  }

  if (opps.len <= 1)
  {
    if (opps.max == maxRank)
      terminalValue = max(north.len, south.len) - 1;
    else
      terminalValue = max(north.len, south.len);
    return true;
  }

  return false;
}


bool Ranks2::leadOK(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const unsigned leadPos,
  const unsigned lead) const
{
  if (leader.ranks[leadPos].count == 0)
  {
    // Could be an in-between rank that the leader doesn't have.
    return false;
  }
  else if (partner.len == 0)
  { // If we have the top rank opposite a void, always play it.
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


bool Ranks2::oppOK(
  const unsigned card,
  const unsigned count,
  const bool alreadyPlayed) const
{
  if (card == 0)
    // Both opponents cannot be void.
    return ! alreadyPlayed;

  else if (count == 0)
    return false;
  else if (count == 1 && alreadyPlayed)
    // Can only play a card once.
    return false;
  else
    return true;
}


bool Ranks2::pardOK(
  const PositionInfo& partner,
  const unsigned toBeat,
  const unsigned pardPos,
  const unsigned pard) const
{
  // Always "play" a void.
  if (partner.len == 0)
    return true;

  if (partner.ranks[pardPos].count == 0)
  {
    // Could be an in-between rank that partner doesn't have.
    return false;
  }

  // No rule concerning high cards.
  if (pard > toBeat)
    return true;

  // Play the lowest of losing cards.
  return (pard == partner.min);
}


void Ranks2::updateHoldings(
  const vector<RankInfo2>& vec1,
  const vector<RankInfo2>& vec2,
  const unsigned leadPos,
  const unsigned lhoPos,
  const unsigned pardPos,
  const unsigned rhoPos,
  PlayEntry& play) const
{
  unsigned cardsNew = cards;

  // The leader always has a card.
  vector<RankInfo2> vec1New = vec1;
  vec1New[leadPos].count--;
  cardsNew--;

  vector<RankInfo2> vec2New = vec2;
  if (play.pard > 0)
  {
    vec2New[pardPos].count--;
    cardsNew--;
  }

  vector<RankInfo2> oppsNew = opps.ranks;
  if (play.lho > 0)
  {
    oppsNew[lhoPos].count--;
    cardsNew--;
  }

  if (play.rho > 0)
  {
    oppsNew[rhoPos].count--;
    cardsNew--;
  }

  if (Ranks2::dominates(vec1New, vec2New))
    Ranks2::canonicalUpdate(vec1New, vec2New, oppsNew, cardsNew, 
      play.holdingNew3, play.holdingNew2);
  else
    Ranks2::canonicalUpdate(vec2New, vec1New, oppsNew, cardsNew, 
      play.holdingNew3, play.holdingNew2);
}


void Ranks2::setPlaysSide(
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
       
  for (unsigned leadPos = 1; leadPos <= leader.ranks.size(); leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks2::leadOK(leader, partner, leadPos, lead))
      continue;

    for (unsigned lhoPos = 0; lhoPos <= opps.max; lhoPos++)
    {
      const unsigned lho = opps.ranks[lhoPos].rank;
      const unsigned lhoCount = opps.ranks[lhoPos].count;
      if (! Ranks2::oppOK(lho, lhoCount, false))
        continue;

      for (unsigned pardPos = partner.min; pardPos <= partner.max; pardPos++)
      {
        const unsigned pard = partner.ranks[pardPos].rank;
        if (! Ranks2::pardOK(partner, max(lead, lho), pardPos, pard))
          continue;

        for (unsigned rhoPos = 0; rhoPos <= opps.max; rhoPos++)
        {
          const unsigned rho = opps.ranks[rhoPos].rank;
          const unsigned rhoCount = opps.ranks[rhoPos].count;
          if (! Ranks2::oppOK(rho, rhoCount, lho == rho))
            continue;
          
          // Register the new play.
          plays.emplace_back(PlayEntry());
          PlayEntry& play = plays.back();
          play.update(side, lead, lho, pard, rho);

          // This takes 67 out of 70 seconds, so commented out for now.
          // Ranks2::updateHoldings(leader.ranks, partner.ranks, 
          //   leadPos, lhoPos, pardPos, rhoPos, play);
        }
      }
    }
  }
}


CombinationType Ranks2::setPlays(
  list<PlayEntry>& plays,
  unsigned& terminalValue) const
{
  // If COMB_TRIVIAL, then only terminalValue is set.
  // Otherwise, plays are set.
  // TODO: Should probably be a tree structure.
  plays.clear();

  if (Ranks2::trivial(terminalValue))
    return COMB_TRIVIAL;

  Ranks2::setPlaysSide(north, south, SIDE_NORTH, plays);
  Ranks2::setPlaysSide(south, north, SIDE_SOUTH, plays);
  return COMB_OTHER;
}


string Ranks2::strRankInfo(
  const vector<RankInfo2>& rankInfo,
  const unsigned rank,
  const string& player) const
{
  stringstream ss;
  if (rank == BIGINT)
    ss << setw(8) << "-" << setw(4) << "-" << setw(6) << "-";
  else
  {
    string concat = "";
    for (unsigned card = rankInfo[rank].count; card-- > 0; )
      concat += rankInfo[rank].cards[card];

    ss << 
      setw(8) << player << 
      setw(4) << rankInfo[rank].count << 
      setw(6) << concat;
  }

  return ss.str();
}


string Ranks2::str() const
{
  stringstream ss;
  ss << "Ranks:\n";

  ss <<  right <<
    setw(8) << "North" << setw(4) << "#" << setw(6) << "cards" <<
    setw(8) << "South" << setw(4) << "#" << setw(6) << "cards" <<
    setw(8) << "Opps" <<  setw(4) << "#" << setw(6) << "cards" << 
    "\n";

  for (unsigned pos = maxRank; pos > 0; pos--) // Exclude void
  {
    ss << 
      Ranks2::strRankInfo(north.ranks, full2reducedNorth[pos], "North") <<
      Ranks2::strRankInfo(south.ranks, full2reducedSouth[pos], "South") <<
      Ranks2::strRankInfo(opps.ranks, full2reducedOpps[pos], "Opps") <<
      endl;
  }
  
  return ss.str() + "\n";
}

