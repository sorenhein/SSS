#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>
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

vector<unsigned> HOLDING3_FACTOR;
vector<vector<unsigned>> HOLDING3_ADDER;

vector<unsigned> HOLDING2_SHIFT;
vector<vector<unsigned>> HOLDING2_ADDER;

vector<unsigned> HOLDING3_RANK_FACTOR;
vector<unsigned> HOLDING3_RANK_ADDER;

vector<unsigned> HOLDING2_RANK_SHIFT;
vector<unsigned> HOLDING2_RANK_ADDER;


Ranks2::Ranks2()
{
  // https://stackoverflow.com/questions/8412630/
  // how-to-execute-a-piece-of-code-only-once
  if (static auto called = false; ! exchange(called, true))
    Ranks2::setConstants();

  north.ranks.clear();
  south.ranks.clear();
  opps.ranks.clear();

  full2reducedNorth.clear();
  full2reducedSouth.clear();
  full2reducedOpps.clear();

  fullCountNorth.clear();
  fullCountSouth.clear();
  fullCountOpps.clear();
}


Ranks2::~Ranks2()
{
}


void Ranks2::setConstants()
{
  HOLDING3_FACTOR.resize(MAX_CARDS+1);
  HOLDING3_FACTOR[0] = 1;
  for (unsigned c = 1; c < HOLDING3_FACTOR.size(); c++)
    HOLDING3_FACTOR[c] = 3 * HOLDING3_FACTOR[c-1];

  HOLDING3_ADDER.resize(MAX_CARDS+1);
  for (unsigned c = 0; c < HOLDING3_FACTOR.size(); c++)
  {
    HOLDING3_ADDER[c].resize(3);
    HOLDING3_ADDER[c][2] = HOLDING3_FACTOR[c] - 1;
    HOLDING3_ADDER[c][1] = HOLDING3_ADDER[c][2] / 2;
    HOLDING3_ADDER[c][0] = 0;
  }

  HOLDING2_SHIFT.resize(MAX_CARDS+1);
  for (unsigned c = 0; c < HOLDING2_SHIFT.size(); c++)
    HOLDING2_SHIFT[c] = c;

  HOLDING2_ADDER.resize(MAX_CARDS+1);
  for (unsigned c = 0; c < HOLDING2_SHIFT.size(); c++)
  {
    HOLDING2_ADDER[c].resize(2);
    HOLDING2_ADDER[c][1] = (c == 0 ? 0 : (1 << c) - 1);
    HOLDING2_ADDER[c][0] = 0;
  }

  // Store the counts of a rank in a 12-bit word.  As we only consider
  // a limited number of cards, we only fill out the table entries up
  // to a sum of 16 cards.
  assert(MAX_CARDS <= 15);
  HOLDING3_RANK_FACTOR.resize(4096);
  HOLDING3_RANK_ADDER.resize(4096);

  HOLDING2_RANK_SHIFT.resize(4096);
  HOLDING2_RANK_ADDER.resize(4096);

  for (unsigned oppCount = 0; oppCount < 16; oppCount++)
  {
    for (unsigned decl1Count = 0; 
        decl1Count <= MAX_CARDS - oppCount; decl1Count++)
    {
      for (unsigned decl2Count = 0; 
          decl2Count <= MAX_CARDS - oppCount - decl1Count; decl2Count++)
      {
        const unsigned sum = oppCount + decl1Count + decl2Count;
        const unsigned index = (oppCount << 8 ) |
          (decl1Count << 4) | decl2Count;
        
        HOLDING3_RANK_FACTOR[index] = HOLDING3_FACTOR[sum];

        HOLDING3_RANK_ADDER[index] = 
          HOLDING3_ADDER[oppCount][CONVERT_OPPS] *
            HOLDING3_FACTOR[decl1Count + decl2Count] +
          HOLDING3_ADDER[decl1Count][CONVERT_NORTH] *
            HOLDING3_FACTOR[decl2Count] +
          HOLDING3_ADDER[decl2Count][CONVERT_SOUTH];

        HOLDING2_RANK_SHIFT[index] = HOLDING2_SHIFT[sum];

        HOLDING2_RANK_ADDER[index] = 
          (HOLDING2_ADDER[oppCount][PAIR_EW] << 
            (decl1Count + decl2Count)) |
          HOLDING2_ADDER[decl1Count + decl2Count][PAIR_NS];
      }
    }
  }
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

  fullCountNorth.resize(cards+1);
  fullCountSouth.resize(cards+1);
  fullCountOpps.resize(cards+1);

  for (unsigned card = 0; card < cards+1; card++)
  {
    fullCountNorth[card].cards.resize(cards+1);
    fullCountSouth[card].cards.resize(cards+1);
    fullCountOpps[card].cards.resize(cards+1);
  }

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

    fullCountNorth[rank].clear();
    fullCountSouth[rank].clear();
    fullCountOpps[rank].clear();
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
  full2reducedOpps[0] = 1;
  fullCountOpps[0].add('-');

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
      fullCountOpps[maxRank].add(CARD_NAMES[i]);
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
        fullCountNorth[maxRank].add(CARD_NAMES[i]);
      }
      else
      {
        south.update(posSouth, maxRank, CARD_NAMES[i], firstSouth);
        full2reducedSouth[maxRank] = posSouth;
        fullCountSouth[maxRank].add(CARD_NAMES[i]);
      }

      prev_is_NS = true;
    }

    h /= 3;
  }

  north.setVoid(false);
  south.setVoid(false);

  if (north.len == 0)
  {
    fullCountNorth[0].add('-');
    full2reducedNorth[0] = 1;
  }
  if (south.len == 0)
  {
    fullCountSouth[0].add('-');
    full2reducedSouth[0] = 1;
  }

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
  const vector<RankInfo3>& fullCount1,
  const vector<RankInfo3>& fullCount2,
  unsigned& holding3,
  unsigned& holding2) const
{
  // This is similar to canonical, but (a) doesn't keep track of card
  // names, and (b) generates both the binary and trinary holdings.
  // It is also quite highly optimized.
  holding3 = 0;
  holding2 = 0;

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
  {
    const unsigned index = 
      (fullCountOpps[rank].count << 8) | 
      (fullCount1[rank].count << 4) | 
       fullCount2[rank].count;

    holding3 = 
      HOLDING3_RANK_FACTOR[index] * holding3 +
      HOLDING3_RANK_ADDER[index];
    holding2 = 
      (holding2 << HOLDING2_RANK_SHIFT[index]) |
      HOLDING2_RANK_ADDER[index];
  }
}


void Ranks2::set(
  const unsigned holding,
  CombEntry& combEntry)
{
// cout << "holding " << holding << endl;
  Ranks2::setRanks(holding);

  combEntry.rotateFlag = ! Ranks2::dominates(north.ranks, north.maxPos, 
    south.ranks, south.maxPos);
  if (combEntry.rotateFlag)
  {
    combEntry.canonicalHolding = Ranks2::canonical(south.ranks, north.ranks,
      full2reducedSouth, full2reducedNorth,
      combEntry.canonical2comb);
  }
  else
  {
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


bool Ranks2::pardOK(
  const PositionInfo& partner,
  const unsigned toBeat,
  const unsigned pard) const
{
  // Always "play" a void.
  if (partner.len == 0)
    return true;

  // By construction, count is > 0.

  // No rule concerning high cards.
  if (pard > toBeat)
    return true;

  // Play the lowest of losing cards.
  return (pard == partner.minRank);
}


void Ranks2::updateHoldings(
  const vector<RankInfo2>& vec1,
  const vector<RankInfo2>& vec2,
  const unsigned max1,
  const unsigned max2,
  const vector<RankInfo3>& fullCount1,
  const vector<RankInfo3>& fullCount2,
  PlayEntry& play)
{
  if (Ranks2::dominates(vec1, max1, vec2, max2))
  {
    Ranks2::canonicalUpdate(
      fullCount1, fullCount2, play.holdingNew3, play.holdingNew2);
  }
  else
  {
    Ranks2::canonicalUpdate(
      fullCount2, fullCount1, play.holdingNew3, play.holdingNew2);
  }
}


void Ranks2::setPlaysSideWithVoid(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const SidePosition side,
  vector<RankInfo3>& fullCount1,
  vector<RankInfo3>& fullCount2,
  vector<PlayEntry>& plays,
  unsigned &playNo)
{
  for (unsigned leadPos = 1; leadPos <= leader.maxPos; leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks2::leadOK(leader, partner, lead))
      continue;

    fullCount1[lead].count--;
    fullCountOpps[0].count--;

    for (unsigned pardPos = partner.minPos; 
        pardPos <= partner.maxPos; pardPos++)
    {
      const unsigned pard = partner.ranks[pardPos].rank;
      if (! Ranks2::pardOK(partner, lead, pard))
        continue;

      fullCount2[pard].count--;

      // toBeat = max(lead, pard)
      for (unsigned rhoPos = 1; rhoPos <= opps.maxPos; rhoPos++)
      {
        const unsigned rho = opps.ranks[rhoPos].rank;

        // TODO Lowest of rho cards < toBeat (no subterfuge left)
        // Lowest of rho cards > toBeat

        fullCountOpps[rho].count--;
          
        // Register the new play.
        if (playNo >= plays.size())
          plays.resize(plays.size() + PLAY_CHUNK_SIZE[cards]);

        PlayEntry& play = plays[playNo++];
        play.update(side, lead, 0, pard, rho);

        Ranks2::updateHoldings(leader.ranks, partner.ranks, 
          leader.maxPos, partner.maxPos, 
          fullCount1, fullCount2, play);

        fullCountOpps[rho].count++;
      }
      fullCount2[pard].count++;
    }
    fullCount1[lead].count++;
    fullCountOpps[0].count++;
  }
}


void Ranks2::setPlaysSideWithoutVoid(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const SidePosition side,
  vector<RankInfo3>& fullCount1,
  vector<RankInfo3>& fullCount2,
  vector<PlayEntry>& plays,
  unsigned &playNo)
{
  for (unsigned leadPos = 1; leadPos <= leader.maxPos; leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks2::leadOK(leader, partner, lead))
      continue;

    fullCount1[lead].count--;

    for (unsigned lhoPos = 1; lhoPos <= opps.maxPos; lhoPos++)
    {
      const unsigned lho = opps.ranks[lhoPos].rank;
      fullCountOpps[lho].count--;

      for (unsigned pardPos = partner.minPos; 
          pardPos <= partner.maxPos; pardPos++)
      {
        const unsigned pard = partner.ranks[pardPos].rank;
        if (! Ranks2::pardOK(partner, max(lead, lho), pard))
          continue;

        fullCount2[pard].count--;

        for (unsigned rhoPos = 0; rhoPos <= opps.maxPos; rhoPos++)
        {
          const unsigned rho = opps.ranks[rhoPos].rank;

          // Maybe the same single card has been played already.
          if (fullCountOpps[rho].count == 0)
            continue;
          
          fullCountOpps[rho].count--;

          // Register the new play.
          if (playNo >= plays.size())
            plays.resize(plays.size() + PLAY_CHUNK_SIZE[cards]);

          PlayEntry& play = plays[playNo++];
          play.update(side, lead, lho, pard, rho);

          Ranks2::updateHoldings(leader.ranks, partner.ranks, 
            leader.maxPos, partner.maxPos, 
            fullCount1, fullCount2, play);
        
          fullCountOpps[rho].count++;
        }
        fullCount2[pard].count++;
      }
      fullCountOpps[lho].count++;
    }
    fullCount1[lead].count++;
  }
}


void Ranks2::setPlaysSide(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const SidePosition side,
  vector<RankInfo3>& fullCount1,
  vector<RankInfo3>& fullCount2,
  vector<PlayEntry>& plays,
  unsigned &playNo)
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

  Ranks2::setPlaysSideWithVoid(leader, partner, side, 
    fullCount1, fullCount2, plays, playNo);
  Ranks2::setPlaysSideWithoutVoid(leader, partner, side,
    fullCount1, fullCount2, plays, playNo);
}


CombinationType Ranks2::setPlays(
  vector<PlayEntry>& plays,
  unsigned& playNo,
  unsigned& terminalValue)
{
  // If COMB_TRIVIAL, then only terminalValue is set.
  // Otherwise, plays are set.
  // TODO: Should probably be a tree structure.
  plays.clear();
  plays.resize(PLAY_CHUNK_SIZE[cards]);
  playNo = 0;

  if (Ranks2::trivial(terminalValue))
    return COMB_TRIVIAL;

  Ranks2::setPlaysSide(north, south, SIDE_NORTH, 
    fullCountNorth, fullCountSouth, plays, playNo);
  Ranks2::setPlaysSide(south, north, SIDE_SOUTH, 
    fullCountSouth, fullCountNorth, plays, playNo);
  return COMB_OTHER;
}


string Ranks2::strRankInfo(
  const RankInfo3& rankInfo,
  const string& player) const
{
  stringstream ss;
  if (rankInfo.count == 0)
    ss << setw(8) << "-" << setw(4) << "-" << setw(6) << "-";
  else
  {
    string concat = "";
    for (unsigned card = rankInfo.count; card-- > 0; )
      concat += rankInfo.cards[card];

    ss << 
      setw(8) << player << 
      setw(4) << rankInfo.count << 
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
      Ranks2::strRankInfo(fullCountNorth[pos], "North") <<
      Ranks2::strRankInfo(fullCountSouth[pos], "South") <<
      Ranks2::strRankInfo(fullCountOpps[pos], "Opps") <<
      endl;
  }
  
  return ss.str() + "\n";
}

