#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>
#include <cassert>

#include "Ranks.h"

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


Ranks::Ranks()
{
  // https://stackoverflow.com/questions/8412630/
  // how-to-execute-a-piece-of-code-only-once
  if (static auto called = false; ! exchange(called, true))
    Ranks::setConstants();

  north.ranks.clear();
  south.ranks.clear();
  opps.ranks.clear();

  north.fullCount.clear();
  south.fullCount.clear();
  opps.fullCount.clear();
}


Ranks::~Ranks()
{
}


void Ranks::setConstants()
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
          HOLDING3_ADDER[oppCount][POSITION_OPPS] *
            HOLDING3_FACTOR[decl1Count + decl2Count] +
          HOLDING3_ADDER[decl1Count][POSITION_NORTH] *
            HOLDING3_FACTOR[decl2Count] +
          HOLDING3_ADDER[decl2Count][POSITION_SOUTH];

        HOLDING2_RANK_SHIFT[index] = HOLDING2_SHIFT[sum];

        HOLDING2_RANK_ADDER[index] = 
          (HOLDING2_ADDER[oppCount][PAIR_EW] << 
            (decl1Count + decl2Count)) |
          HOLDING2_ADDER[decl1Count + decl2Count][PAIR_NS];
      }
    }
  }
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

  north.fullCount.resize(cards+1);
  south.fullCount.resize(cards+1);
  opps.fullCount.resize(cards+1);

  for (unsigned card = 0; card < cards+1; card++)
  {
    north.fullCount[card].cards.resize(cards+1);
    south.fullCount[card].cards.resize(cards+1);
    opps.fullCount[card].cards.resize(cards+1);
  }

  maxRank = cards;
  north.maxRank = cards;
  south.maxRank = cards;
  opps.maxRank = cards;
}


void Ranks::clear()
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
    north.fullCount[rank].clear();
    south.fullCount[rank].clear();
    opps.fullCount[rank].clear();
  }

  maxRank = 0;
}


void Ranks::setRanks()
{
  Ranks::clear();

  // If the first card belongs to EW, there will be an uptick (from 0).
  // If it does belong to NS, there will only be an uptick if there
  // is a count > 0, which will not be the case.
  bool prev_is_NS = ((holding % 3) == POSITION_OPPS);
  unsigned posNorth = 1;
  unsigned posSouth = 1;
  unsigned posOpps = 0;

  const unsigned imin = (cards > 13 ? 0 : 13-cards);
  unsigned h = holding;

  bool firstNorth = true;
  bool firstSouth = true;
  bool firstOpps = true;
  opps.setVoid(true); // Have to do it first to make max come out right
  // fullCountOpps[0].add('-');
  opps.fullCount[0].add('-');

  for (unsigned i = imin; i < imin+cards; i++)
  {
    const unsigned c = h % 3;
    if (c == POSITION_OPPS)
    {
      if (prev_is_NS)
      {
        maxRank++;
        posOpps++;
      }

      opps.update(posOpps, maxRank, CARD_NAMES[i], firstOpps);
      opps.fullCount[maxRank].add(CARD_NAMES[i]);
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

      if (c == POSITION_NORTH)
      {
        north.update(posNorth, maxRank, CARD_NAMES[i], firstNorth);
        north.fullCount[maxRank].add(CARD_NAMES[i]);
      }
      else
      {
        south.update(posSouth, maxRank, CARD_NAMES[i], firstSouth);
        south.fullCount[maxRank].add(CARD_NAMES[i]);
      }

      prev_is_NS = true;
    }

    h /= 3;
  }

  north.setVoid(false);
  south.setVoid(false);

  if (north.len == 0)
    north.fullCount[0].add('-');
  if (south.len == 0)
    south.fullCount[0].add('-');

  north.setSingleRank();
  south.setSingleRank();
  opps.setSingleRank();
}


bool Ranks::dominates(
  const vector<ReducedRankInfo>& vec1,
  const unsigned max1,
  const vector<ReducedRankInfo>& vec2,
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


unsigned Ranks::canonicalTrinary(
  const vector<FullRankInfo>& fullCount1,
  const vector<FullRankInfo>& fullCount2) const
{
  // This is similar to canonicalNew, but only does holding3.
  unsigned holding3 = 0;

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
  {
    const unsigned index = 
      (opps.fullCount[rank].count << 8) | 
      (fullCount1[rank].count << 4) | 
       fullCount2[rank].count;

    holding3 = 
      HOLDING3_RANK_FACTOR[index] * holding3 +
      HOLDING3_RANK_ADDER[index];
  }
  return holding3;
}


void Ranks::canonicalBoth(
  const vector<FullRankInfo>& fullCount1,
  const vector<FullRankInfo>& fullCount2,
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
      (opps.fullCount[rank].count << 8) | 
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


void Ranks::set(
  const unsigned holdingIn,
  CombEntry& combEntry)
{
// cout << "holding " << holdingIn << endl;
  holding = holdingIn;
  Ranks::setRanks();

  combEntry.rotateFlag = ! Ranks::dominates(north.ranks, north.maxPos, 
    south.ranks, south.maxPos);

  if (combEntry.rotateFlag)
  {
    combEntry.canonicalHolding = 
      Ranks::canonicalTrinary(south.fullCount, north.fullCount);
  }
  else
  {
    combEntry.canonicalHolding = 
      Ranks::canonicalTrinary(north.fullCount, south.fullCount);
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


bool Ranks::leadOK(
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


bool Ranks::pardOK(
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


void Ranks::updateHoldings(
  const vector<ReducedRankInfo>& vec1,
  const vector<ReducedRankInfo>& vec2,
  const unsigned max1,
  const unsigned max2,
  const vector<FullRankInfo>& fullCount1,
  const vector<FullRankInfo>& fullCount2,
  PlayEntry& play)
{
  if (Ranks::dominates(vec1, max1, vec2, max2))
  {
    Ranks::canonicalBoth(
      fullCount1, fullCount2, play.holdingNew3, play.holdingNew2);
  }
  else
  {
    Ranks::canonicalBoth(
      fullCount2, fullCount1, play.holdingNew3, play.holdingNew2);
  }
}


void Ranks::setPlaysSideWithVoid(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const SidePosition side,
  vector<FullRankInfo>& fullCount1,
  vector<FullRankInfo>& fullCount2,
  vector<PlayEntry>& plays,
  unsigned &playNo)
{
  for (unsigned leadPos = 1; leadPos <= leader.maxPos; leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    fullCount1[lead].count--;
    opps.fullCount[0].count--;

    for (unsigned pardPos = partner.minPos; 
        pardPos <= partner.maxPos; pardPos++)
    {
      const unsigned pard = partner.ranks[pardPos].rank;
      if (! Ranks::pardOK(partner, lead, pard))
        continue;

      fullCount2[pard].count--;

      // toBeat = max(lead, pard)
      for (unsigned rhoPos = 1; rhoPos <= opps.maxPos; rhoPos++)
      {
        const unsigned rho = opps.ranks[rhoPos].rank;

        // TODO Lowest of rho cards < toBeat (no subterfuge left)
        // Lowest of rho cards > toBeat

        opps.fullCount[rho].count--;
          
        // Register the new play.
        if (playNo >= plays.size())
          plays.resize(plays.size() + PLAY_CHUNK_SIZE[cards]);

        PlayEntry& play = plays[playNo++];
        play.update(side, lead, 0, pard, rho);

        Ranks::updateHoldings(leader.ranks, partner.ranks, 
          leader.maxPos, partner.maxPos, 
          fullCount1, fullCount2, play);

        opps.fullCount[rho].count++;
      }
      fullCount2[pard].count++;
    }
    fullCount1[lead].count++;
    opps.fullCount[0].count++;
  }
}


void Ranks::setPlaysSideWithoutVoid(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const SidePosition side,
  vector<FullRankInfo>& fullCount1,
  vector<FullRankInfo>& fullCount2,
  vector<PlayEntry>& plays,
  unsigned &playNo)
{
  for (unsigned leadPos = 1; leadPos <= leader.maxPos; leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    fullCount1[lead].count--;

    for (unsigned lhoPos = 1; lhoPos <= opps.maxPos; lhoPos++)
    {
      const unsigned lho = opps.ranks[lhoPos].rank;
      opps.fullCount[lho].count--;

      for (unsigned pardPos = partner.minPos; 
          pardPos <= partner.maxPos; pardPos++)
      {
        const unsigned pard = partner.ranks[pardPos].rank;
        if (! Ranks::pardOK(partner, max(lead, lho), pard))
          continue;

        fullCount2[pard].count--;

        for (unsigned rhoPos = 0; rhoPos <= opps.maxPos; rhoPos++)
        {
          const unsigned rho = opps.ranks[rhoPos].rank;

          // Maybe the same single card has been played already.
          if (opps.fullCount[rho].count == 0)
            continue;
          
          opps.fullCount[rho].count--;

          // Register the new play.
          if (playNo >= plays.size())
            plays.resize(plays.size() + PLAY_CHUNK_SIZE[cards]);

          PlayEntry& play = plays[playNo++];
          play.update(side, lead, lho, pard, rho);

          Ranks::updateHoldings(leader.ranks, partner.ranks, 
            leader.maxPos, partner.maxPos, 
            fullCount1, fullCount2, play);
        
          opps.fullCount[rho].count++;
        }
        fullCount2[pard].count++;
      }
      opps.fullCount[lho].count++;
    }
    fullCount1[lead].count++;
  }
}


void Ranks::setPlaysSide(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const SidePosition side,
  vector<FullRankInfo>& fullCount1,
  vector<FullRankInfo>& fullCount2,
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

  Ranks::setPlaysSideWithVoid(leader, partner, side, 
    fullCount1, fullCount2, plays, playNo);
  Ranks::setPlaysSideWithoutVoid(leader, partner, side,
    fullCount1, fullCount2, plays, playNo);
}


CombinationType Ranks::setPlays(
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

  if (Ranks::trivial(terminalValue))
    return COMB_TRIVIAL;

  // TODO Don't need to pass .fullCount separately
  Ranks::setPlaysSide(north, south, SIDE_NORTH, 
    north.fullCount, south.fullCount, plays, playNo);
  Ranks::setPlaysSide(south, north, SIDE_SOUTH, 
    south.fullCount, north.fullCount, plays, playNo);
  return COMB_OTHER;
}


void Ranks::strSetFullNames(
  vector<string>& namesNorth,
  vector<string>& namesSouth,
  vector<string>& namesOpps) const
{
  // Start by full names. This is a small version of setRanks().
  // As it is only required when we want str(), it is not calculated
  // by default in setRanks.
  const unsigned imin = (cards > 13 ? 0 : 13-cards);
  bool prev_is_NS = ((holding % 3) == POSITION_OPPS);
  unsigned h = holding;
  unsigned rank = 0;
  unsigned minRankOpps = 0;
  unsigned maxRankOpps = 0;
  unsigned iMinOpps = 0;

  for (unsigned i = imin; i < imin+cards; i++)
  {
    const unsigned c = h % 3;
    if (c == POSITION_OPPS)
    {
      if (prev_is_NS)
        rank++;

      namesOpps[rank] = CARD_NAMES[i] + namesOpps[rank];
      if (minRankOpps == 0)
      {
        minRankOpps = rank;
        iMinOpps = i;
      }
      maxRankOpps = rank;
      prev_is_NS = false;
    }
    else
    {
      if (! prev_is_NS)
        rank++;

      if (c == POSITION_NORTH)
        namesNorth[rank] = CARD_NAMES[i] + namesNorth[rank];
      else
        namesSouth[rank] = CARD_NAMES[i] + namesSouth[rank];

      prev_is_NS = true;
    }

    h /= 3;
  }

  // Make the last multiple opponent rank into x's if it seems
  // low enough.
  if (namesOpps[minRankOpps].size() > 1 && iMinOpps <= 6) // An eight
  {
    namesOpps[minRankOpps] = string(namesOpps[minRankOpps].size(), 'x');
    minRankOpps++;
  }
  
  // Replace multiple opponent ranks from the top with HH, etc.
  unsigned index = 0;
  for (rank = maxRankOpps+1; rank-- > minRankOpps; )
  {
    if (namesOpps[rank].size() > 1)
    {
      namesOpps[rank] = 
        string(namesOpps[rank].size(), GENERIC_NAMES[index]);
      index++;
    }
  }
}


string Ranks::strPosition(
  const string& cardString,
  const string& player) const
{
  stringstream ss;
  if (cardString.empty())
    ss << setw(8) << "-" << setw(4) << "-" << setw(6) << "-";
  else
    ss << 
      setw(8) << player << 
      setw(4) << cardString.size() << 
      setw(6) << cardString;

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

  vector<string> namesNorth(cards+1);
  vector<string> namesSouth(cards+1);
  vector<string> namesOpps(cards+1);
  Ranks::strSetFullNames(namesNorth, namesSouth, namesOpps);

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
  {
    ss <<
      Ranks::strPosition(namesNorth[rank], "North") <<
      Ranks::strPosition(namesSouth[rank], "South") <<
      Ranks::strPosition(namesOpps[rank], "Opps") <<
      "\n";
  }

  return ss.str() + "\n";
}

