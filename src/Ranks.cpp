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


void Ranks::setPlayers()
{
  Ranks::zero();

  // We choose prev_is_NS ("the previously seen card belonged to NS")
  // such that the first real card we see will result in an increase
  // in maxRank, i.e. in the running rank.  Therefore we will never
  // write to rank = 0 (void) in the loop itself.
  bool prev_is_NS = ((holding % 3) == POSITION_OPPS);

  // Have to set opps here already, as opps are not definitely void
  // but may be , so we don't want the maximum values to get
  // reset to 0 by calling setVoid() after the loop below.
  opps.setVoid();

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
        opps.updateStep(maxRank);
      }

      opps.update(maxRank, i);

      prev_is_NS = false;
    }
    else
    {
      if (! prev_is_NS)
      {
        // We could get a mix of positions within the same rank,
        // not necessarily sorted by position.  So we have to treat
        // North and South separately.
        maxRank++;
        north.updateStep(maxRank);
        south.updateStep(maxRank);
      }

      if (c == POSITION_NORTH)
        north.update(maxRank, i);
      else
        south.update(maxRank, i);

      prev_is_NS = true;
    }

    h /= 3;
  }

  // We have to have the names of BOTH players' cards before calling
  // finish(), as it relies on this.
  north.setNames();
  south.setNames();
  opps.setNames();

  north.finish(south);
  south.finish(north);
}


unsigned Ranks::canonicalTrinary(
  const Declarer& dominant,
  const Declarer& recessive) const
{
  // This is similar to canonicalBoth, but only does holding3.
  // Actually it is only guaranteed to generate a canonical holding3 
  // if there is no rank reduction among the opponents' cards.  
  // For example, with AJ / KT missing Q9, if the trick goes
  // T, Q, A, - then we're left with J /K missing the 9, where
  // either side is >= the other.  Therefore the rotateFlag and the
  // order will depend on the order of comparison.
  // Therefore Combinations::getPtr looks up the canonical index.
  unsigned holding3 = 0;

  for (unsigned rank = maxRank; rank > 0; rank--) // Exclude void
  {
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
  const Declarer& dominant,
  const Declarer& recessive,
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

  Ranks::setPlayers();

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
  TrickEntry& trivialEntry) const
{
  if (opps.hasRank(maxRank))
    trivialEntry.setEmpty(tricks-1);
  else
  {
    // Play the highest card.
    if (north.hasRank(maxRank))
      trivialEntry.set(tricks, WIN_NORTH, north.top());

    // If both declarer sides have winners, keep both as a choice.
    if (south.hasRank(maxRank))
      trivialEntry.set(tricks, WIN_SOUTH, south.top());
  }
}


bool Ranks::trivial(TrickEntry& trivialEntry) const
{
  if (north.isVoid() && south.isVoid())
  {
    trivialEntry.setEmpty(0);
    return true;
  }

  if (opps.isVoid())
  {
    trivialEntry.setEmpty(max(north.length(), south.length()));
    return true;
  }

  if (north.length() <= 1 && south.length() <= 1)
  {
    // North-South win their last trick if they have the highest card.
    Ranks::trivialRanked(1, trivialEntry);
    return true;
  }

  if (opps.length() == 1)
  {
    // North-South win it all, or almost, if opponents have one card left.
    Ranks::trivialRanked(max(north.length(), south.length()), trivialEntry);
    return true;
  }

  return false;
}


bool Ranks::leadOK(
  const Declarer& leader,
  const Declarer& partner,
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
  const Declarer& partner,
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


void Ranks::updateHoldings(Play& play) const
{
  if (north.greater(south, opps))
  {
    play.holding3 = Ranks::canonicalTrinary(north, south);

    // North (without the played card) is still >= South (without
    // the played card).  So we can justify not rotating, but see
    // also the comment in canonicalTrinary().
    play.rotateFlag = false;
  }
  else
  {
    // South is strictly greater than North, so there is no
    // ambiguity here.
    play.holding3 = Ranks::canonicalTrinary(south, north);
    play.rotateFlag = true;
  }
}


void Ranks::finish(
  const Declarer& leader,
  Play& play) const
{
  Ranks::updateHoldings(play);

  play.trickNS = (max(play.lead(), play.pard()) > 
      max(play.lho(), play.rho()) ? 1 : 0);

  // Number of cards in play after this trick;
  play.cardsLeft = cards +
    (play.lhoPtr->isVoid() ? 1 : 0) +
    (play.pardPtr->isVoid() ? 1 : 0) +
    (play.rhoPtr->isVoid() ? 1 : 0) -
    4;

  if (north.isVoid())
    play.northCardsPtr = nullptr;
  else
    play.northCardsPtr = &north.getCards(true);

  if (south.isVoid())
    play.southCardsPtr = nullptr;
  else
    play.southCardsPtr = &south.getCards(true);

  if (! play.trickNS)
    play.currBestPtr = nullptr;
  else
  {
    // TODO Probably generate on the fly if not already set.
    play.currBestPtr = &leader.getWinner(play.lead(), play.pard());
  }
}


void Ranks::setPlaysLeadWithVoid(
  Declarer& leader,
  Declarer& partner,
  const unsigned lead,
  Play& play,
  Plays& plays)
{
  opps.playRank(0);
  play.lhoPtr = opps.voidPtr();

  for (auto& pardPtr: partner.getCards(false))
  {
    play.pardPtr = pardPtr;
    const unsigned pard = pardPtr->getRank();
    if (! Ranks::pardOK(partner, lead, pard))
      continue;

    play.pardCollapse = partner.playRank(pard, leader, maxRank);

    const unsigned toBeat = max(lead, pard);

    for (auto& rhoPtr: opps.getCards())
    {
      play.rhoPtr = rhoPtr;
      const unsigned rho = rhoPtr->getRank();
      // If LHO is known to be void, RHO can duck completely.
      if (rho < toBeat && rho != opps.minFullRank())
        continue;

      opps.playRank(rho);
          
      // Register the new play.
      Ranks::finish(leader, play);
      plays.log(play);

      opps.restoreRank(rho);

      // If RHO wins, he should do so as cheaply as possible.
      if (rho > toBeat)
        break;
    }

    partner.restoreRank(pard);
  }

  opps.restoreRank(0);
}


void Ranks::setPlaysLeadWithoutVoid(
  Declarer& leader,
  Declarer& partner,
  const unsigned lead,
  Play& play,
  Plays& plays)
{
  for (auto& lhoPtr: opps.getCards())
  {
    play.lhoPtr = lhoPtr;
    const unsigned lho = lhoPtr->getRank();
    opps.playRank(lho);

    for (auto& pardPtr: partner.getCards(false))
    {
      play.pardPtr = pardPtr;
      const unsigned pard = pardPtr->getRank();
      if (! Ranks::pardOK(partner, max(lead, lho), pard))
        continue;

      play.pardCollapse = partner.playRank(pard, leader, maxRank);

      // As LHO is not void, RHO may show out.  We do this separately,
      // as it is more convenient to store the plays in Player this way.
      // We don't need to "play" the void, as it does not affect
      // the holdings.

      // Register the void play.
      play.rhoPtr = opps.voidPtr();
      Ranks::finish(leader, play);
      plays.log(play);
      
      // This loop excludes the RHO void.
      for (auto& rhoPtr: opps.getCards())
      {
        play.rhoPtr = rhoPtr;
        const unsigned rho = rhoPtr->getRank();

        // Maybe the same single card has been played already.
        if (! opps.hasRank(rho))
          continue;
          
        opps.playRank(rho);

        // Register the new play.
        Ranks::finish(leader, play);
        plays.log(play);
      
        opps.restoreRank(rho);
      }

      partner.restoreRank(pard);
    }
    opps.restoreRank(lho);
  }
}


void Ranks::setPlaysSide(
  Declarer& leader,
  Declarer& partner,
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

  Play play;
  if (side == SIDE_NORTH)
    play.side = POSITION_NORTH;
  else
    play.side = POSITION_SOUTH;

  for (auto& leadPtr: leader.getCards(false))
  {
    // I wish I could write for (play.leadPtr: ...), but a declaration
    // is required.
    play.leadPtr = leadPtr;
    const unsigned lead = play.leadPtr->getRank();
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    play.leadCollapse = leader.playRank(lead, partner, maxRank);

    // For optimization we treat the case separately where LHO is void.
    Ranks::setPlaysLeadWithVoid(leader, partner, lead, play, plays);

    Ranks::setPlaysLeadWithoutVoid(leader, partner, lead, play, plays);

    leader.restoreRank(lead);
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

