#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "Ranks2.h"

#include "struct.h"
#include "const.h"

#define BIGINT 99

const vector<unsigned> PLAY_CHUNK_SIZE =
{
    1, //  0
    2, //  1
    2, //  2
    2, //  3
    4, //  4
    6, //  5
   10, //  6
   15, //  7
   25, //  8
   40, //  9
   50, // 10
   70, // 11
   90, // 12
  110, // 13
  130, // 14
  150, // 15
};


Ranks2::Ranks2()
{
  north.ranks.clear();
  south.ranks.clear();
  opps.ranks.clear();

  full2reducedNorth.clear();
  full2reducedSouth.clear();
  full2reducedOpps.clear();
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

  full2reducedNorth.resize(cards+1, BIGINT);
  full2reducedSouth.resize(cards+1, BIGINT);
  full2reducedOpps.resize(cards+1, BIGINT);

  maxRank = cards;
  north.maxRank = cards;
  south.maxRank = cards;
  opps.maxRank = cards;
}


void Ranks2::clear()
{
  for (unsigned rank = 0; rank <= north.maxRank; rank++)
    north.ranks[rank].clear();
  for (unsigned rank = 0; rank <= south.maxRank; rank++)
    south.ranks[rank].clear();
  for (unsigned rank = 0; rank <= opps.maxRank; rank++)
    opps.ranks[rank].clear();

  north.len = 0;
  south.len = 0;
  opps.len = 0;

  for (unsigned rank = 0; rank <= maxRank; rank++)
  {
    full2reducedNorth[rank] = BIGINT;
    full2reducedSouth[rank] = BIGINT;
    full2reducedOpps[rank] = BIGINT;
  }

  maxRank = 0;
}


void Ranks2::setRanks(const unsigned holding)
{
  Ranks2::clear();

  // If the first card belongs to EW, there will be an uptick (from 0).
  // If it does belong to NS, there will only be an uptick if there
  // is a count > 0, which will not be the case.
  bool prev_is_NS = ((holding % 3) == CONVERT_OPPS);
  unsigned posNorth = 1;
  unsigned posSouth = 1;
  unsigned posOpps = 0;

  const unsigned imin = (cards > 13 ? 0 : 13-cards);
  unsigned h = holding;

  bool firstNorth = true;
  bool firstSouth = true;
  bool firstOpps = true;
  opps.setVoid(true); // Have to do it first to make max come out right

  for (unsigned i = imin; i < imin+cards; i++)
  {
    const unsigned c = h % 3;
    if (c == CONVERT_OPPS)
    {
      if (prev_is_NS)
      {
        maxRank++;
        posOpps++;
      }

      opps.update(posOpps, maxRank, CARD_NAMES[i], firstOpps);
      full2reducedOpps[maxRank] = posOpps;
      prev_is_NS = false;
    }
    else
    {
      if (! prev_is_NS)
      {
        maxRank++;
        if (north.ranks[posNorth].count > 0)
          posNorth++;
        if (south.ranks[posSouth].count > 0)
          posSouth++;
      }

      if (c == CONVERT_NORTH)
      {
        north.update(posNorth, maxRank, CARD_NAMES[i], firstNorth);
        full2reducedNorth[maxRank] = posNorth;
      }
      else
      {
        south.update(posSouth, maxRank, CARD_NAMES[i], firstSouth);
        full2reducedSouth[maxRank] = posSouth;
      }

      prev_is_NS = true;
    }

    h /= 3;
  }

  north.setVoid(false);
  south.setVoid(false);

  north.setSingleRank();
  south.setSingleRank();
  opps.setSingleRank();
}


bool Ranks2::dominates(
  const vector<RankInfo2>& vec1,
  const unsigned max1,
  const vector<RankInfo2>& vec2,
  const unsigned max2) const
{
  // The rank vectors may not be of the same effective size.
  unsigned pos1 = max1 + 1;  // One beyond end, as we first advance
  unsigned pos2 = max2 + 1;

  while (true)
  {
    while (true)
    {
      // If we run out of vec2, vec1 wins even if it also runs out.
      if (pos2 == 0)
        return true;

      if (vec2[--pos2].count)
        break;
    }

    while (true)
    {
      // Otherwise vec2 wins.
      if (pos1 == 0)
        return false;

      if (vec1[--pos1].count)
        break;
    }

    if (vec1[pos1].rank > vec2[pos2].rank)
      return true;
    if (vec1[pos1].rank < vec2[pos2].rank)
      return false;
    if (vec1[pos1].count > vec2[pos2].count)
      return true;
    if (vec1[pos1].count < vec2[pos2].count)
      return false;
  }
}


unsigned Ranks2::canonical(
  const vector<RankInfo2>& vec1,
  const vector<RankInfo2>& vec2,
  const vector<unsigned>& full2reduced1,
  const vector<unsigned>& full2reduced2,
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
      if (full2reduced1[rank] < BIGINT)
      {
        const unsigned pos = full2reduced1[rank];
        for (unsigned count = 0; count < vec1[pos].count; count++)
        {
          holding += (holding << 1) + CONVERT_NORTH;
          canonical2comb[index] = vec1[pos].cards[count];
        }
      }

      if (full2reduced2[rank] < BIGINT)
      {
        const unsigned pos = full2reduced2[rank];
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

  combEntry.rotateFlag = ! Ranks2::dominates(north.ranks, north.maxPos, 
    south.ranks, south.maxPos);
  if (combEntry.rotateFlag)
  {
// cout << "South dominates\n";
    combEntry.canonicalHolding = Ranks2::canonical(south.ranks, north.ranks,
      full2reducedSouth, full2reducedNorth,
      combEntry.canonical2comb);
  }
  else
  {
// cout << "North dominates\n";
    combEntry.canonicalHolding = Ranks2::canonical(north.ranks, south.ranks,
      full2reducedNorth, full2reducedSouth,
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
    terminalValue = (opps.maxRank != maxRank);
    return true;
  }

  if (opps.len <= 1)
  {
    if (opps.maxRank == maxRank)
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
  const unsigned lead) const
{
  // By construction, count is always > 0.
  if (partner.len == 0)
  { // If we have the top rank opposite a void, always play it.
    if (leader.maxRank == maxRank && lead < maxRank)
      return false;
  }
  else if (! leader.singleRank)
  {
    // Both sides have 2+ ranks.  
    if (lead >= partner.maxRank)
    {
      // Again, don't lead a too-high card.
      return false;
    }
    else if (lead <= partner.minRank && lead > leader.minRank)
    {
      // If partner's lowest card is at least as high, lead lowest.
      return false;
    }
  }
  return true;
}


bool Ranks2::rhoOK(
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
  return (pard == partner.minRank);
}


void Ranks2::updateHoldings(
  const vector<RankInfo2>& vec1,
  const vector<RankInfo2>& vec2,
  const unsigned max1, // TODO Many too many arguments
  const unsigned max2,
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

  if (Ranks2::dominates(vec1New, max1, vec2New, max2))
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
  vector<PlayEntry>& plays,
  unsigned &playNo) const
{
  if (leader.len == 0)
    return;

  // Always lead the singleton rank.  If both have this, lead the
  // higher one, or if they're the same, the first one.
  if (partner.singleRank &&
      (! leader.singleRank || leader.maxRank < partner.maxRank ||
        (leader.maxRank == partner.maxRank && side == SIDE_SOUTH)))
    return;

  // Don't lead a card by choice that's higher than partner's best one.
  if (! leader.singleRank && 
      partner.len > 0 && 
      leader.minRank >= partner.maxRank)
    return;
       
  for (unsigned leadPos = 1; leadPos <= leader.maxPos; leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks2::leadOK(leader, partner, lead))
      continue;

    for (unsigned lhoPos = 0; lhoPos <= opps.maxPos; lhoPos++)
    {
      const unsigned lho = opps.ranks[lhoPos].rank;
      const unsigned lhoCount = opps.ranks[lhoPos].count;
      // if (! Ranks2::lhoOK(lho, lhoCount))
        // continue;

      for (unsigned pardPos = partner.minPos; pardPos <= partner.maxPos; pardPos++)
      {
        const unsigned pard = partner.ranks[pardPos].rank;
        if (! Ranks2::pardOK(partner, max(lead, lho), pardPos, pard))
          continue;

        for (unsigned rhoPos = 0; rhoPos <= opps.maxPos; rhoPos++)
        {
          const unsigned rho = opps.ranks[rhoPos].rank;
          const unsigned rhoCount = opps.ranks[rhoPos].count;
          if (! Ranks2::rhoOK(rho, rhoCount, lho == rho))
            continue;
          
          // Register the new play.
          if (playNo >= plays.size())
            plays.resize(plays.size() + PLAY_CHUNK_SIZE[cards]);

          PlayEntry& play = plays[playNo++];
          play.update(side, lead, lho, pard, rho);

          // This takes 67 out of 70 seconds, so commented out for now.
          // Ranks2::updateHoldings(leader.ranks, partner.ranks, 
          //   leader.maxPos, partner.maxPos,
          //   leadPos, lhoPos, pardPos, rhoPos, play);
        }
      }
    }
  }
}


CombinationType Ranks2::setPlays(
  vector<PlayEntry>& plays,
  unsigned& playNo,
  unsigned& terminalValue) const
{
  // If COMB_TRIVIAL, then only terminalValue is set.
  // Otherwise, plays are set.
  // TODO: Should probably be a tree structure.
  plays.clear();
  plays.resize(PLAY_CHUNK_SIZE[cards]);
  playNo = 0;

  if (Ranks2::trivial(terminalValue))
    return COMB_TRIVIAL;

  Ranks2::setPlaysSide(north, south, SIDE_NORTH, plays, playNo);
  Ranks2::setPlaysSide(south, north, SIDE_SOUTH, plays, playNo);
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

