#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>
#include <cassert>

#include "Ranks.h"

#include "struct.h"
#include "const.h"

/*
 * This class performs some rank manipulation for an entire hand
 * consisting of North, South and opposing cards.
 *
 * The set() method sets up the rank data and determines whether the
 * holding is canonical, i.e. whether or not it can be reduced to
 * another equivalent holding.  
 * - It is determined which side "dominates", i.e. which side has
 *   higher or more high cards.
 * - If the dominant side has cards of the same rank as the other side, 
 *   then the higher of these cards are given to the dominant side.
 *
 * The setPlays() method sets up all meaningful plays for North-South.
 * If the holding has a trivial value, then no plays are returned,
 * and a terminal value (number of NS tricks) is returned instead.
 *
 * The key methods are quite heavily optimized for time.  For example:
 * - The play list is a vector and it only grows, by a chunk at a time,
 *   when we run out of space.  
 * - The creation of a holding is done by table lookup of each individual 
 *   rank.  
 * - The innermost loops of the play generation only have the minimum 
 *   instructions needed.
 * - The rank arrays are only zeroed out to the minimum extent between
 *   usages.
 *
 * TODO
 * - Derive a vector mapping new, collapsed ranks to original ones.
 *   Store it in plays.  Idea: In the play generation loop, keep track
 *   of ranks that are disappearing, storing them in a vector of original
 *   ranks.  When making the new holding3, keep track of skipped ranks
 *   and fill out the vector in this way.
 * - dominates() could be a method ">=" of PositionInfo.
 * - Debug that str() is still working the new way (correct card names).
 * - Once LHO is known to be void, RHO should win cheaply or duck
 *   all the way.  This goes in setPlaysWithVoid().
 * - It would be possible in principle to detect when LHO plays the K
 *   in front of AQ (then never play the queen).  Only when the king
 *   is the single card of its rank.  Probably too much overhead to
 *   check for it?
 * - It's probably safe always to play the ace from partner when
 *   LHO plays a rank just below it (the "king").
 */

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

  north.clear();
  south.clear();
  opps.clear();
}


Ranks::~Ranks()
{
}


void Ranks::setConstants()
{
  // First we set up tables for a single count of a certain position
  // (North, South, opps).  The factor is a power of 3 or 2 in order
  // to shift the trinary or binary holding up, and the adder is the
  // value (in "trits" or in bits) to add to the holding.

  vector<unsigned> HOLDING3_FACTOR;
  vector<vector<unsigned>> HOLDING3_ADDER;

  vector<unsigned> HOLDING2_SHIFT;
  vector<vector<unsigned>> HOLDING2_ADDER;

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

  // Then we set up tables for a complete rank, including counts with
  // North, South and opps.  Of course either North+South or opps will
  // have a rank, and not both, but the table works uniformly.

  // We store the counts of a rank in a 12-bit word.  As we only consider
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

  north.resize(cards);
  south.resize(cards);
  opps.resize(cards);

  maxRank = cards;
}


void Ranks::zero()
{
  north.zero();
  south.zero();
  opps.zero();

  maxRank = 0;
}


void Ranks::setRanks()
{
  Ranks::zero();

  // We choose prev_is_NS ("the previously seen card belonged to NS")
  // such that the first real card we see will result in an increase
  // in maxRank, i.e. in the running rank.  Therefore we will never
  // write to rank = 0 (void) in the loop itself.
  bool prev_is_NS = ((holding % 3) == POSITION_OPPS);

  unsigned posNorth = 1;
  unsigned posSouth = 1;
  unsigned posOpps = 0;

  bool firstNorth = true;
  bool firstSouth = true;
  bool firstOpps = true;

  // Have to set opps here already, as opps are not definitely void
  // but may be void, so we don't want the maximum values to get
  // reset to 0 by calling setVoid() after the loop below.
  opps.setVoid(true); // Have to do it first to make max come out right

  const unsigned imin = (cards > 13 ? 0 : 13-cards);
  unsigned h = holding;

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

      opps.update(posOpps, maxRank, firstOpps);
      prev_is_NS = false;
    }
    else
    {
      if (! prev_is_NS)
      {
        // We could get a mix of positions within the same rank,
        // not necessarily sorted by position.
        maxRank++;
        if (north.ranks[posNorth].count > 0)
          posNorth++;
        if (south.ranks[posSouth].count > 0)
          posSouth++;
      }

      if (c == POSITION_NORTH)
        north.update(posNorth, maxRank, firstNorth);
      else
        south.update(posSouth, maxRank, firstSouth);

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


bool Ranks::dominates(
  const PositionInfo& first,
  const PositionInfo& second) const
{
  // The rank vectors may not be of the same effective size.
  unsigned pos1 = first.maxPos + 1;  // One beyond end, as we first advance
  unsigned pos2 = second.maxPos + 1;

  while (true)
  {
    while (true)
    {
      // If we run out of vec2, vec1 wins even if it also runs out.
      if (pos2 == 0)
        return true;

      if (second.ranks[--pos2].count)
        break;
    }

    while (true)
    {
      // Otherwise vec2 wins.
      if (pos1 == 0)
        return false;

      if (first.ranks[--pos1].count)
        break;
    }

    if (first.ranks[pos1].rank > second.ranks[pos2].rank)
      return true;
    if (first.ranks[pos1].rank < second.ranks[pos2].rank)
      return false;
    if (first.ranks[pos1].count > second.ranks[pos2].count)
      return true;
    if (first.ranks[pos1].count < second.ranks[pos2].count)
      return false;
  }
}


unsigned Ranks::canonicalTrinary(
  const vector<unsigned>& fullCount1,
  const vector<unsigned>& fullCount2) const
{
  // This is similar to canonicalNew, but only does holding3.
  unsigned holding3 = 0;

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
  {
    const unsigned index = 
      (opps.fullCount[rank] << 8) | 
      (fullCount1[rank] << 4) | 
       fullCount2[rank];

    holding3 = 
      HOLDING3_RANK_FACTOR[index] * holding3 +
      HOLDING3_RANK_ADDER[index];
  }
  return holding3;
}


void Ranks::canonicalBoth(
  const vector<unsigned>& fullCount1,
  const vector<unsigned>& fullCount2,
  unsigned& holding3,
  unsigned& holding2) const
{
  // This is similar to canonicalTrinary, but generates both the binary 
  // and trinary holdings.
  holding3 = 0;
  holding2 = 0;

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
  {
    const unsigned index = 
      (opps.fullCount[rank] << 8) | 
      (fullCount1[rank] << 4) | 
       fullCount2[rank];

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
  holding = holdingIn;
  Ranks::setRanks();

  combEntry.rotateFlag = ! Ranks::dominates(north, south);

  if (combEntry.rotateFlag)
    combEntry.canonicalHolding = 
      Ranks::canonicalTrinary(south.fullCount, north.fullCount);
  else
    combEntry.canonicalHolding = 
      Ranks::canonicalTrinary(north.fullCount, south.fullCount);

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

  // No rule concerning high cards.
  if (pard > toBeat)
    return true;

  // Play the lowest of losing cards.
  return (pard == partner.minRank);
}


void Ranks::updateHoldings(
  const PositionInfo& leader,
  const PositionInfo& partner,
  PlayEntry& play) const
{
  if (Ranks::dominates(leader, partner))
  {
    Ranks::canonicalBoth(
      leader.fullCount, partner.fullCount, 
        play.holdingNew3, play.holdingNew2);
  }
  else
  {
    Ranks::canonicalBoth(
      partner.fullCount, leader.fullCount, 
        play.holdingNew3, play.holdingNew2);
  }
}


void Ranks::setPlaysSideWithVoid(
  PositionInfo& leader,
  PositionInfo& partner,
  const SidePosition side,
  vector<PlayEntry>& plays,
  unsigned &playNo)
{
  // For optimization we treat the case separately where LHO is void.

  for (unsigned leadPos = 1; leadPos <= leader.maxPos; leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    leader.fullCount[lead]--;
    opps.fullCount[0]--;

    for (unsigned pardPos = partner.minPos; 
        pardPos <= partner.maxPos; pardPos++)
    {
      const unsigned pard = partner.ranks[pardPos].rank;
      if (! Ranks::pardOK(partner, lead, pard))
        continue;

      partner.fullCount[pard]--;

      for (unsigned rhoPos = 1; rhoPos <= opps.maxPos; rhoPos++)
      {
        const unsigned rho = opps.ranks[rhoPos].rank;
        opps.fullCount[rho]--;
          
        // Register the new play.
        if (playNo >= plays.size())
          plays.resize(plays.size() + PLAY_CHUNK_SIZE[cards]);

        PlayEntry& play = plays[playNo++];
        play.update(side, lead, 0, pard, rho);

        Ranks::updateHoldings(leader, partner, play);

        opps.fullCount[rho]++;
      }
      partner.fullCount[pard]++;
    }
    leader.fullCount[lead]++;
    opps.fullCount[0]++;
  }
}


void Ranks::setPlaysSideWithoutVoid(
  PositionInfo& leader,
  PositionInfo& partner,
  const SidePosition side,
  vector<PlayEntry>& plays,
  unsigned &playNo)
{
  for (unsigned leadPos = 1; leadPos <= leader.maxPos; leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    leader.fullCount[lead]--;

    for (unsigned lhoPos = 1; lhoPos <= opps.maxPos; lhoPos++)
    {
      const unsigned lho = opps.ranks[lhoPos].rank;
      opps.fullCount[lho]--;

      for (unsigned pardPos = partner.minPos; 
          pardPos <= partner.maxPos; pardPos++)
      {
        const unsigned pard = partner.ranks[pardPos].rank;
        if (! Ranks::pardOK(partner, max(lead, lho), pard))
          continue;

        partner.fullCount[pard]--;

        for (unsigned rhoPos = 0; rhoPos <= opps.maxPos; rhoPos++)
        {
          const unsigned rho = opps.ranks[rhoPos].rank;

          // Maybe the same single card has been played already.
          if (opps.fullCount[rho] == 0)
            continue;
          
          opps.fullCount[rho]--;

          // Register the new play.
          if (playNo >= plays.size())
            plays.resize(plays.size() + PLAY_CHUNK_SIZE[cards]);

          PlayEntry& play = plays[playNo++];
          play.update(side, lead, lho, pard, rho);

          Ranks::updateHoldings(leader, partner, play);
        
          opps.fullCount[rho]++;
        }
        partner.fullCount[pard]++;
      }
      opps.fullCount[lho]++;
    }
    leader.fullCount[lead]++;
  }
}


void Ranks::setPlaysSide(
  PositionInfo& leader,
  PositionInfo& partner,
  const SidePosition side,
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

  Ranks::setPlaysSideWithVoid(leader, partner, side, plays, playNo);
  Ranks::setPlaysSideWithoutVoid(leader, partner, side, plays, playNo);
}


CombinationType Ranks::setPlays(
  vector<PlayEntry>& plays,
  unsigned& playNo,
  unsigned& terminalValue)
{
  // If COMB_TRIVIAL, then only terminalValue is set.
  // Otherwise, plays are set.
  // TODO: Should probably be a tree structure.
  // TODO: Use space allocation in a Combination.
  plays.clear();
  plays.resize(PLAY_CHUNK_SIZE[cards]);
  playNo = 0;

  if (Ranks::trivial(terminalValue))
    return COMB_TRIVIAL;

  Ranks::setPlaysSide(north, south, SIDE_NORTH, plays, playNo);
  Ranks::setPlaysSide(south, north, SIDE_SOUTH, plays, playNo);
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

