#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cassert>

#include "Plays.h"
#include "Ranks.h"

#include "struct.h"
#include "const.h"

/*
 * This class performs rank manipulation for an entire hand
 * consisting of North, South and opposing cards.
 *
 * The set() method sets up the rank data and determines whether the
 * holding is canonical, i.e. whether or not it can be reduced to
 * another equivalent holding.  
 * - It is determined which side dominates, i.e. which side has
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
 */

mutex mtxRanks;
static bool init_flag = false;

vector<unsigned> HOLDING3_RANK_FACTOR;
vector<unsigned> HOLDING3_RANK_ADDER;

vector<unsigned> HOLDING2_RANK_SHIFT;
vector<unsigned> HOLDING2_RANK_ADDER;


Ranks::Ranks()
{
  mtxRanks.lock();
  if (! init_flag)
  {
    Ranks::setConstants();
    init_flag = true;
  }
  mtxRanks.unlock();

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

  assert(POSITION_NORTH == 0);
  assert(POSITION_SOUTH == 1);

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
    HOLDING2_ADDER[c][1] = (c == 0u ? 0u : (1u << c) - 1u);
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

  north.resize(cards, POSITION_NORTH);
  south.resize(cards, POSITION_SOUTH);
  opps.resize(cards, POSITION_OPPS);

  maxRank = cards;
}


unsigned Ranks::size() const
{
  return cards;
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

  // bool firstNorth = true;
  // bool firstSouth = true;
  // bool firstOpps = true;

  unsigned numberNorth = 0;
  unsigned numberSouth = 0;
  unsigned numberOpps = 0;
  unsigned depthNorth = 0;
  unsigned depthSouth = 0;
  unsigned depthOpps = 0;

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
        depthOpps = 0;
      }

      opps.update(posOpps, maxRank, depthOpps, numberOpps, i);
      depthOpps++;
      numberOpps++;

      prev_is_NS = false;
    }
    else
    {
      if (! prev_is_NS)
      {
        // We could get a mix of positions within the same rank,
        // not necessarily sorted by position.
        maxRank++;
        depthNorth = 0;
        depthSouth = 0;

        if (north.hasReducedRank(posNorth))
          posNorth++;
        if (south.hasReducedRank(posSouth))
          posSouth++;
      }

      if (c == POSITION_NORTH)
      {
        north.update(posNorth, maxRank, depthNorth, numberNorth, i);
        depthNorth++;
        numberNorth++;
      }
      else
      {
        south.update(posSouth, maxRank, depthSouth, numberSouth, i);
        depthSouth++;
        numberSouth++;
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


unsigned Ranks::canonicalTrinary(
  const Player& dominant,
  const Player& recessive) const
  // const vector<unsigned>& fullCount1,
  // const vector<unsigned>& fullCount2) const
{
  // This is similar to canonicalNew, but only does holding3.
  // Actually it only generates a canonical holding3 if there is
  // no rank reduction among the opponents' cards.  Therefore
  // Combinations::getPtr looks up the canonical index.
  unsigned holding3 = 0;

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
  {
    // const unsigned index = 
      // (opps.fullCount[rank] << 8) | 
      // (fullCount1[rank] << 4) | 
       // fullCount2[rank];

    const unsigned index = 
      (opps.count(rank) << 8) | 
      (dominant.count(rank) << 4) | 
       recessive.count(rank);

    holding3 = 
      HOLDING3_RANK_FACTOR[index] * holding3 +
      HOLDING3_RANK_ADDER[index];
  }
  return holding3;
}


void Ranks::canonicalBoth(
  const Player& dominant,
  const Player& recessive,
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
      (opps.count(rank) << 8) | 
      (dominant.count(rank) << 4) | 
       recessive.count(rank);

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

  north.setNames(true);
  south.setNames(true);
  opps.setNames(false);

  north.setRemainders();
  south.setRemainders();

  north.setBest(south);
  south.setBest(north);

  combEntry.rotateFlag = ! (north.greater(south, opps));

  if (combEntry.rotateFlag)
    Ranks::canonicalBoth(south, north,
      combEntry.canonicalHolding3, combEntry.canonicalHolding2);
  else
   Ranks::canonicalBoth(north, south,
      combEntry.canonicalHolding3, combEntry.canonicalHolding2);

  combEntry.canonicalFlag = (holding == combEntry.canonicalHolding3);
}


void Ranks::trivialRanked(
  const unsigned tricks,
  const unsigned winRank,
  TrickEntry& trivialEntry) const
{
  // Play the highest card.
  if (north.maxFullRank() == winRank)
  {
    // trivialEntry.set(tricks, WIN_NORTH, winRank, 0, north.length()-1,
      // namesNorth[winRank].at(0));
    trivialEntry.set(tricks, WIN_NORTH, north.top());
  }

  if (south.maxFullRank() == winRank)
  {
    // trivialEntry.set(tricks, WIN_SOUTH, winRank, 0, south.length()-1,
      // namesSouth[winRank].at(0));
    trivialEntry.set(tricks, WIN_SOUTH, south.top());
  }
}


bool Ranks::trivial(TrickEntry& trivialEntry) const
{
  if (north.isVoid() && south.isVoid())
  {
    trivialEntry.set(0, WIN_NONE, 0, 0, 0,' ');
    return true;
  }

  if (opps.isVoid())
  {
    trivialEntry.set(max(north.length(), south.length()), 
      WIN_NONE, 0, 0, 0, ' ');
    return true;
  }

  if (north.length() <= 1 && south.length() <= 1)
  {
    // North-South win their last trick if they have the highest card.
    if (opps.maxFullRank() == maxRank)
      trivialEntry.set(0, WIN_NONE, 0, 0, 0, ' ');
    else
      // TODO Isn't this the same as maxRank?
      Ranks::trivialRanked(1, opps.maxFullRank()+1, trivialEntry);

    return true;
  }

  if (opps.length() == 1)
  {
    if (opps.maxFullRank() == maxRank)
      trivialEntry.set(max(north.length(), south.length())-1, 
        WIN_NONE, 0, 0, 0, ' ');
    else
      // TODO Isn't this the same as maxRank?
      Ranks::trivialRanked(max(north.length(), south.length()), 
        opps.maxFullRank()+1, trivialEntry);

    return true;
  }

  return false;
}


bool Ranks::leadOK(
  const Player& leader,
  const Player& partner,
  const unsigned lead) const
{
  // By construction, count is always > 0.
  if (partner.isVoid())
  { 
    // If we have the top rank opposite a void, always play it.
    if (leader.maxFullRank() == maxRank && lead < maxRank)
      return false;
  }
  else if (! leader.isSingleRanked())
  {
    // Both sides have 2+ ranks.  
    if (lead >= partner.maxFullRank())
    {
      // Again, don't lead a too-high card.
      return false;
    }
    else if (lead <= partner.minFullRank() && 
      lead > leader.minFullRank())
    {
      // If partner's lowest card is at least as high, lead lowest.
      return false;
    }
  }
  return true;
}


bool Ranks::pardOK(
  const Player& partner,
  const unsigned toBeat,
  const unsigned pard) const
{
  // Always "play" a void.
  if (partner.isVoid())
    return true;

  // No rule concerning high cards.
  if (pard > toBeat)
    return true;

  // If LHO plays a "king" and partner has the "ace", there is no
  // point in not playing the ace.
  if (toBeat == opps.maxFullRank() &&
      partner.maxFullRank() > toBeat)
    return false;

  // Play the lowest of irrelevant cards.
  return (pard == partner.minFullRank());
}


void Ranks::updateHoldings(
  const Player& leader,
  const Player& partner,
  const SidePosition side,
  unsigned& holding3,
  bool& rotateFlag) const
{
  if (leader.greater(partner, opps))
  {
// cout << "leader >= partner, side " << (side == SIDE_NORTH ?
  // "North" : "South") << endl;
    holding3 = Ranks::canonicalTrinary(leader, partner);
    // rotateFlag is absolute.
    // If the leader is South, then rotate.
    rotateFlag = (side == SIDE_SOUTH);
  }
  else
  {
// cout << "leader < partner, side " << (side == SIDE_NORTH ?
  // "North" : "South") << endl;
    holding3 = Ranks::canonicalTrinary(partner, leader);
    // If the leader is South, then don't rotate.
    rotateFlag = (side == SIDE_NORTH);
  }
}


void Ranks::logPlay(
  Plays& plays,
  const Player& leader,
  const Player& partner,
  const SidePosition side,
  const unsigned lead,
  const unsigned lho,
  const unsigned pard,
  const unsigned rho,
  const bool leadCollapse,
  const bool pardCollapse,
  const unsigned holding3,
  const bool rotateFlag) const
{
  const unsigned trickNS = (max(lead, pard) > max(lho, rho) ? 1 : 0);

  vector<Card> const * leadOrderPtr;
  vector<Card> const * pardOrderPtr;

  if (lead == 0)
    leadOrderPtr = nullptr;
  else
  {
// assert(lead < leader.remainders.size());
// cout << "Looking up lead " << lead << endl;
    // leadOrderPtr = &leader.remainders[lead];
    leadOrderPtr = &leader.remainder(lead);
  }

  if (pard == 0)
    pardOrderPtr = nullptr;
  else
  {
// assert(pard < partner.remainders.size());
// cout << "Looking up pard " << pard << endl;
    // pardOrderPtr = &partner.remainders[pard];
    pardOrderPtr = &partner.remainder(pard);
  }

  Winner const * winPtr;
  if (! trickNS)
    winPtr = nullptr;
  else
  {
    // TODO Probably generate on the fly if not already set.
// assert(lead < leader.best.size());
// assert(pard < leader.best[lead].size());
    // winPtr = &leader.best[lead][pard];
    winPtr = &leader.getWinner(lead, pard);
  }

// cout << "Ranks::logPlay " << side << ", " << lead << ", " << lho <<
  // ", " << pard << ", " << rho << ", trick " << trickNS << endl;

// cout << "Ranks::logPlay " << 
  // (leadOrderPtr ? to_string(leadOrderPtr->size()) : "null") << ", " <<
  // (pardOrderPtr ? to_string(pardOrderPtr->size()) : "null") << endl;

  plays.logFull(side, lead, lho, pard, rho, 
    trickNS, leadCollapse, pardCollapse,
    leadOrderPtr, pardOrderPtr, winPtr,
    holding3, rotateFlag);
}


void Ranks::setPlaysLeadWithVoid(
  Player& leader,
  Player& partner,
  const SidePosition side,
  const unsigned lead,
  const bool leadCollapse,
  Plays& plays)
{
  unsigned holding3;
  bool rotateFlag;
  bool pardCollapse, rhoCollapse;

  opps.playFull(0);
  for (unsigned pardPos = partner.minNumber(); 
      pardPos <= partner.maxNumber(); pardPos++)
  {
    // const unsigned pard = partner.ranks[pardPos].rank;
    const unsigned pard = partner.rankOfNumber(pardPos);
    if (! Ranks::pardOK(partner, lead, pard))
      continue;

    partner.playFull(pard);
    pardCollapse = (pard > 1 && 
      pard < maxRank &&
      ! partner.hasFullRank(pard) &&
      ! leader.hasFullRank(pard));
    const unsigned toBeat = max(lead, pard);

    for (unsigned rhoPos = 1; rhoPos <= opps.maxNumber(); rhoPos++)
    {
      // const unsigned rho = opps.ranks[rhoPos].rank;
      const unsigned rho = opps.rankOfNumber(rhoPos);
      // If LHO is known to be void, RHO can duck completely.
      if (rho < toBeat && rho != opps.minFullRank())
        continue;

      opps.playFull(rho);
      rhoCollapse = ! opps.hasFullRank(rho);
          
      // Register the new play.
      Ranks::updateHoldings(leader, partner, side, holding3, rotateFlag);
      Ranks::logPlay(plays, leader, partner, side, lead, 0, pard, rho,
        leadCollapse, pardCollapse, holding3, rotateFlag);

      opps.restoreFull(rho);

      // If RHO wins, he should do so as cheaply as possible.
      if (rho > toBeat)
        break;
    }
    partner.restoreFull(pard);
  }
  opps.restoreFull(0);
}


void Ranks::setPlaysLeadWithoutVoid(
  Player& leader,
  Player& partner,
  const SidePosition side,
  const unsigned lead,
  const bool leadCollapse,
  Plays& plays)
{
  unsigned holding3;
  bool rotateFlag;
  bool lhoCollapse, pardCollapse, rhoCollapse;

  for (unsigned lhoPos = 1; lhoPos <= opps.maxNumber(); lhoPos++)
  {
    // const unsigned lho = opps.ranks[lhoPos].rank;
    const unsigned lho = opps.rankOfNumber(lhoPos);
    opps.playFull(lho);
    lhoCollapse = ! opps.hasFullRank(lho);

    for (unsigned pardPos = partner.minNumber(); 
        pardPos <= partner.maxNumber(); pardPos++)
    {
      // const unsigned pard = partner.ranks[pardPos].rank;
      const unsigned pard = partner.rankOfNumber(pardPos);
      if (! Ranks::pardOK(partner, max(lead, lho), pard))
        continue;

      partner.playFull(pard);
      pardCollapse = (pard > 1 && 
        pard != maxRank &&
        ! partner.hasFullRank(pard) &&
        ! leader.hasFullRank(pard));

      for (unsigned rhoPos = 0; rhoPos <= opps.maxNumber(); rhoPos++)
      {
        // const unsigned rho = opps.ranks[rhoPos].rank;
        const unsigned rho = opps.rankOfNumber(rhoPos);

        // Maybe the same single card has been played already.
        if (! opps.hasFullRank(rho))
          continue;
          
        opps.playFull(rho);
        rhoCollapse = (rho > 0 && ! opps.hasFullRank(rho));

        // Register the new play.
        Ranks::updateHoldings(leader, partner, side, holding3, rotateFlag);
        Ranks::logPlay(plays, leader, partner, side, lead, lho, pard, rho,
          leadCollapse, pardCollapse, holding3, rotateFlag);
      
        opps.restoreFull(rho);
      }
      partner.restoreFull(pard);
    }
    opps.restoreFull(lho);
  }
}


void Ranks::setPlaysSide(
  Player& leader,
  Player& partner,
  const SidePosition side,
  Plays& plays)
{
  if (leader.isVoid())
    return;

  // Always lead the singleton rank.  If both have this, lead the
  // higher one, or if they're the same, the first one.
  if (partner.isSingleRanked() &&
      (! leader.isSingleRanked() || 
          leader.maxFullRank() < partner.maxFullRank() ||
        (leader.maxFullRank() == partner.maxFullRank() && side == SIDE_SOUTH)))
    return;

  // Don't lead a card by choice that's higher than partner's best one.
  if (! leader.isSingleRanked() && 
      ! partner.isVoid() && 
      leader.minFullRank() >= partner.maxFullRank())
    return;

  bool leadCollapse;

  for (unsigned leadPos = 1; leadPos <= leader.maxNumber(); leadPos++)
  {
    const unsigned lead = leader.rankOfNumber(leadPos);
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    leader.playFull(lead);
    leadCollapse = (! leader.hasFullRank(lead) && 
      ! partner.hasFullRank(lead) &&
      lead != 1 &&
      lead != maxRank);

    // For optimization we treat the case separately where LHO is void.
    Ranks::setPlaysLeadWithVoid(leader, partner, side, lead,
      leadCollapse, plays);

    Ranks::setPlaysLeadWithoutVoid(leader, partner, side, lead,
      leadCollapse, plays);

    leader.restoreFull(lead);
  }
}


CombinationType Ranks::setPlays(
  Plays& plays,
  TrickEntry& trivialEntry)
{
  // If COMB_TRIVIAL, only terminalValue is set.
  // Otherwise, plays are set.

  if (Ranks::trivial(trivialEntry))
    return COMB_TRIVIAL;

  Ranks::setPlaysSide(north, south, SIDE_NORTH, plays);
  Ranks::setPlaysSide(south, north, SIDE_SOUTH, plays);
  return COMB_OTHER;
}


string Ranks::strTable() const
{
  // Makes a table with one rank per line.
  stringstream ss;
  ss << right <<
    north.strRankHeader() <<
    south.strRankHeader() <<
    opps.strRankHeader() <<
    "\n";

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
    ss <<
      north.strRank(rank) <<
      south.strRank(rank) <<
      opps.strRank(rank) <<
      "\n";

  return ss.str() + "\n";
}


wstring Ranks::wstrDiagram() const
{
  // Makes a little box out of Unicode characters.
  wstringstream wss;
  wss << 
    "    " << north.wstr() << "\n" <<
    "    " << L"\u2554\u2550\u2550\u2557\n" <<
    "    " << L"\u2551  \u2551 miss " << opps.wstr() << "\n" <<
    "    " << L"\u255A\u2550\u2550\u255D\n" <<
    "    " << south.wstr() << "\n";

  return wss.str();
}

