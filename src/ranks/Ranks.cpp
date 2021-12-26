#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cassert>

#include "Ranks.h"

#include "../plays/Plays.h"
#include "../combinations/CombEntry.h"
#include "../inputs/Control.h"

#include "cranks.h"

extern Control control;


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
    setRankConstants4();
    setRankConstants23();
    init_flag = true;
  }
  mtxRanks.unlock();
}


void Ranks::resize(const unsigned cardsIn)
{
  cards = cardsIn;

  north.resize(cards, SIDE_NORTH);
  south.resize(cards, SIDE_SOUTH);
  opps.resize(cards, SIDE_OPPS);
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

  maxGlobalRank = 0;
}


void Ranks::setPlayers()
{
  Ranks::zero();

  // We choose prev_is_NS ("the previously seen card belonged to NS")
  // such that the first real card we see will result in an increase
  // in maxGlobalRank, i.e. in the running rank.  Therefore we will never
  // write to rank = 0 (void) in the loop itself.
  bool prev_is_NS = ((holding3 % 3) == SIDE_OPPS);

  // Have to set opps here already, as opps are not definitely void
  // but may be, so we don't want the maximum values to get
  // reset to 0 by calling setVoid() after the loop below.
  opps.setVoid();

  const unsigned char cardsChar = static_cast<unsigned char>(cards);

  // Start the numbering from 1 in order to distinguish from void.
  const unsigned char absMin = (cardsChar > 13 ? 0 : 13-cardsChar) + 1;
  unsigned char rel = 1;
  unsigned h = holding3;
  holding4 = 0;

  for (unsigned char abs = absMin; abs < absMin+cardsChar; rel++, abs++)
  {
    const unsigned c = h % 3;

    // Spread out so each trit occupies 2 bits cleanly.
    holding4 = (holding4 >> 2) | (c << 2*(cardsChar-1));

    if (c == SIDE_OPPS)
    {
      if (prev_is_NS)
      {
        maxGlobalRank++;
        opps.updateStep(maxGlobalRank);
      }

      opps.update(maxGlobalRank, rel, abs);

      prev_is_NS = false;
    }
    else
    {
      if (! prev_is_NS)
      {
        // We could get a mix of positions within the same rank,
        // not necessarily sorted by position.  So we have to treat
        // North and South separately.
        maxGlobalRank++;
        north.updateStep(maxGlobalRank);
        south.updateStep(maxGlobalRank);
      }

      if (c == SIDE_NORTH)
        north.update(maxGlobalRank, rel, abs);
      else
        south.update(maxGlobalRank, rel, abs);

      prev_is_NS = true;
    }

    h /= 3;
  }

  // We have to have the names of BOTH players' cards before calling
  // finish(), as it relies on this.
  north.setNames();
  south.setNames();
  opps.setNames();

  north.finish();
  south.finish();
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
  unsigned h3 = 0;

  for (unsigned char rank = maxGlobalRank; rank > 0; rank--) // Exclude void
  {
    const unsigned index = 
      (static_cast<unsigned>(opps.count(rank)) << 8) | 
      (static_cast<unsigned>(dominant.count(rank)) << 4) | 
       static_cast<unsigned>(recessive.count(rank));

    h3 = 
      HOLDING3_RANK_FACTOR[index] * h3 +
      HOLDING3_RANK_ADDER[index];
  }
  return h3;
}


void Ranks::canonicalBoth(
  const Declarer& dominant,
  const Declarer& recessive,
  const Opponents& opponents,
  unsigned& holding3In,
  unsigned& holding2In) const
{
  // This is similar to canonicalTrinary, but generates both the binary 
  // and trinary holdings.
  holding3In = 0;
  holding2In = 0;

  for (unsigned char rank = maxGlobalRank; rank > 0; rank--) // Exclude void
  {
    const unsigned index = 
      (static_cast<unsigned>(opponents.count(rank)) << 8) | 
      (static_cast<unsigned>(dominant.count(rank)) << 4) | 
       static_cast<unsigned>(recessive.count(rank));

    holding3In = 
      HOLDING3_RANK_FACTOR[index] * holding3In +
      HOLDING3_RANK_ADDER[index];
    holding2In = 
      (holding2In << HOLDING2_RANK_SHIFT[index]) |
      HOLDING2_RANK_ADDER[index];
  }
}


void Ranks::setOwnRanks(CombReference& combRef) const
{
  // For an uncanonical holding we indicate a rotation if South
  // holds a higher top card than North.
  /*
  if (north.isVoid())
    combRef.rotateFlag = ! south.isVoid();
  else if (south.isVoid())
    combRef.rotateFlag = false;
  else
    combRef.rotateFlag = (south.top() > north.top());
    */

  combRef.rotateFlag = ! (north.greater(south, opps));

  combRef.holding3 = holding3;
  combRef.holding2 = uncanonicalBinary(holding4);
}


void Ranks::setReference(CombReference& combRef) const
{
  // This method is called from setRanks directly, so the players,
  // holding3 and holding4 are given.  If we're using ranked plays, 
  // we can rely on holding3.
  // It is also called from finishMinimal() which is only called when 
  // the players have been set up correctly, but holding3 has not.
  // In this case we're not using ranked plays.

// cout << "Enter setRef\n";
  combRef.rotateFlag = ! (north.greater(south, opps));

  if (control.runRankComparisons())
  {
// cout << "Calling orientedBoth with rotate " << combRef.rotateFlag << endl;

    orientedBoth(combRef.rotateFlag, cards, holding4,
      combRef.holding3, combRef.holding2);

// cout << "Got h3 " << combRef.holding3 << ", h2 " << combRef.holding2 << endl;
  }
  else
  {
    if (combRef.rotateFlag)
      Ranks::canonicalBoth(south, north, opps,
        combRef.holding3, combRef.holding2);
    else
      Ranks::canonicalBoth(north, south, opps,
        combRef.holding3, combRef.holding2);
  }

}


/*
void Ranks::setRanks(CombReference& combRef) const
{
  if (control.runRankComparisons())
    Ranks::setOwnRanks(combRef);
  else
    Ranks::setCanonicalRanks(combRef);
}
*/


void Ranks::setRanks(
  const unsigned holding3In,
  CombEntry& combEntry)
{
  holding3 = holding3In;

  Ranks::setPlayers();

  // Ranks::setRanks(combEntry.canonical);

  Ranks::setOwnRanks(combEntry.own);
  Ranks::setReference(combEntry.reference);

  combEntry.referenceFlag = (holding3 == combEntry.reference.holding3);
}


void Ranks::trivialRanked(
  const unsigned char tricks,
  Result& trivial) const
{
  if (opps.hasRank(maxGlobalRank))
    trivial.setTricks(tricks-1);
  else
  {
    // Play the highest card.
    Winners winners;
/*
    if (north.hasRank(maxGlobalRank))
      winners.set(SIDE_NORTH, north.top());

    // If both declarer sides have winners, keep both as a choice.
    if (south.hasRank(maxGlobalRank))
      winners.set(SIDE_SOUTH, south.top());
*/

    // TODO New idea -- does it work?
    // Only go by the actual winner (gasp!).

    /*
    if (north.greater(south, opps))
    {
      winners.set(SIDE_NORTH, north.top());
    }
    else
    {
      winners.set(SIDE_SOUTH, south.top());
    }
    */

    winners.set(north.top(), south.top());

    trivial.set(0, tricks, winners);
  }
}


bool Ranks::makeTrivial(Result& trivial) const
{
  if (north.isVoid() && south.isVoid())
  {
    trivial.setTricks(0);
    return true;
  }

  if (opps.isVoid())
  {
    trivial.setTricks(
      static_cast<unsigned char>(max(north.length(), south.length())));
    return true;
  }

  if (north.length() <= 1 && south.length() <= 1)
  {
    // North-South win their last trick if they have the highest card.
    Ranks::trivialRanked(1, trivial);
    return true;
  }

  if (opps.length() == 1)
  {
    // North-South win it all, or almost, if opponents have one card left.
    Ranks::trivialRanked(
      static_cast<unsigned char>(max(north.length(), south.length())), trivial);
    return true;
  }

  return false;
}


bool Ranks::leadOK(
  const Declarer& leader,
  const Declarer& partner,
  const unsigned char lead) const
{
  // By construction, count is always > 0.
  if (partner.isVoid())
  { 
    // If we have the top rank opposite a void, always play it.
    if (leader.maxFullRank() == maxGlobalRank && lead < maxGlobalRank)
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
  const unsigned char toBeat,
  const unsigned char pard) const
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
  if (control.runRankComparisons())
  {
    // Don't do any canonical reduction -- just play the cards.
    play.rotateFlag = ! (north.greater(south, opps));
    play.holding3 = orientedTrinary(play.cardsLeft, holding4, play);

// cout << "updateHoldings:\n";
// cout << play.strLine() << endl;
  }
  else
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
}


void Ranks::finish(Play& play) const
{
  // This could be a method in Play, but we'd have to pass in not
  // only play but also north and south, so we might as well leave
  // it here.

  play.trickNS = (max(play.lead(), play.pard()) > 
      max(play.lho(), play.rho()) ? 1 : 0);

  // Number of cards in play after this trick;
  play.cardsLeft = cards +
    (play.lhoPtr->isVoid() ? 1 : 0) +
    (play.pardPtr->isVoid() ? 1 : 0) +
    (play.rhoPtr->isVoid() ? 1 : 0) -
    4;

  Ranks::updateHoldings(play);

  if (north.isVoid())
    play.northCardsPtr = nullptr;
  else
    play.northCardsPtr = &north.getCards(true);

  if (south.isVoid())
    play.southCardsPtr = nullptr;
  else
    play.southCardsPtr = &south.getCards(true);

  if (play.trickNS)
  {
    if (play.side == SIDE_NORTH)
      play.currBest.set(* play.leadPtr, * play.pardPtr);
    else
      play.currBest.set(* play.pardPtr, * play.leadPtr);
  }
}


void Ranks::setPlaysLeadWithVoid(
  Declarer& leader,
  Declarer& partner,
  const unsigned char lead,
  Play& play,
  Plays& plays)
{
  opps.playRank(0);
  play.lhoPtr = opps.voidPtr();

  // for (auto& pardPtr: partner.getCards(false))
  for (auto& pardPtr: partner.getCards(control.runRankComparisons()))
  {
    play.pardPtr = pardPtr;
    const unsigned char pard = pardPtr->getRank();
    if (! Ranks::pardOK(partner, lead, pard))
      continue;

    play.pardCollapse = partner.playRank(pard, leader, maxGlobalRank);

    const unsigned char toBeat = max(lead, pard);

    for (auto& rhoPtr: opps.getCards())
    {
      play.rhoPtr = rhoPtr;
      const unsigned char rho = rhoPtr->getRank();
      // If LHO is known to be void, RHO can duck completely.
      if (rho < toBeat && rho != opps.minFullRank())
        continue;

      opps.playRank(rho);
          
      // Register the new play.
      Ranks::finish(play);
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
  const unsigned char lead,
  Play& play,
  Plays& plays)
{
  for (auto& lhoPtr: opps.getCards())
  {
    play.lhoPtr = lhoPtr;
    const unsigned char lho = lhoPtr->getRank();
    opps.playRank(lho);

    // for (auto& pardPtr: partner.getCards(false))
    for (auto& pardPtr: partner.getCards(control.runRankComparisons()))
    {
      play.pardPtr = pardPtr;
      const unsigned char pard = pardPtr->getRank();
      if (! Ranks::pardOK(partner, max(lead, lho), pard))
        continue;

      play.pardCollapse = partner.playRank(pard, leader, maxGlobalRank);

      // As LHO is not void, RHO may show out.  We do this separately,
      // as it is more convenient to store the plays in Player this way.
      // We don't need to "play" the void, as it does not affect
      // the holdings.

      // Register the void play.
      play.rhoPtr = opps.voidPtr();
      Ranks::finish(play);
      plays.log(play);
      
      // This loop excludes the RHO void.
      for (auto& rhoPtr: opps.getCards())
      {
        play.rhoPtr = rhoPtr;
        const unsigned char rho = rhoPtr->getRank();

        // Maybe the same single card has been played already.
        if (! opps.hasRank(rho))
          continue;
          
        opps.playRank(rho);

        // Register the new play.
        Ranks::finish(play);
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
  Play& play,
  Plays& plays)
{
  if (leader.isVoid())
    return;

  // Always lead the singleton rank.  If both have this, lead the
  // higher one, or if they're the same, the first one.
  if (partner.isSingleRanked() &&
      (! leader.isSingleRanked() || 
          leader.maxFullRank() < partner.maxFullRank() ||
        (leader.maxFullRank() == partner.maxFullRank() && play.side == SIDE_SOUTH)))
    return;

  // Don't lead a card by choice that's higher than partner's best one.
  if (! leader.isSingleRanked() && 
      ! partner.isVoid() && 
      leader.minFullRank() >= partner.maxFullRank())
    return;

  // for (auto& leadPtr: leader.getCards(false))
  for (auto& leadPtr: leader.getCards(control.runRankComparisons()))
  {
    // I wish I could write for (play.leadPtr: ...), but a declaration
    // is required.
    play.leadPtr = leadPtr;
    const unsigned char lead = play.leadPtr->getRank();
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    play.leadCollapse = leader.playRank(lead, partner, maxGlobalRank);

    // For optimization we treat the case separately where LHO is void.
    Ranks::setPlaysLeadWithVoid(leader, partner, lead, play, plays);

    Ranks::setPlaysLeadWithoutVoid(leader, partner, lead, play, plays);

    leader.restoreRank(lead);
  }
}


CombinationType Ranks::setPlays(
  Plays& plays,
  Result& trivial)
{
  // If COMB_CONSTANT, only terminalValue is set.
  // Otherwise, plays are set.

  if (Ranks::makeTrivial(trivial))
    return COMB_CONSTANT;

  Play play;

  // This will remain unchanged for all plays from this side.
  play.side = SIDE_NORTH;
  Ranks::setPlaysSide(north, south, play, plays);

  play.side = SIDE_SOUTH;
  Ranks::setPlaysSide(south, north, play, plays);
  return COMB_SIZE;
}


bool Ranks::partnerVoid() const
{
  // North always has the cards if there is a void on declarer's side.
  return south.isVoid();
}


void Ranks::finishMinimal(
  const unsigned holdingRef,
  list<CombReference>& minimals)
{
  Ranks::north.setNames();
  Ranks::south.setNames();
  Ranks::opps.setNames();

  Ranks::north.finish();
  Ranks::south.finish();

  // Check whether the new minimal holding is different.
  CombReference combRef;
  Ranks::setReference(combRef);

// cout << "finishMinimal different: reference " << holdingRef << 
  // ", constructed " << combRef.holding3 << endl;

  if (combRef.holding3 != holdingRef)
    minimals.emplace_back(combRef);
}


void Ranks::losingMinimal(
  unsigned char& relIndex,
  unsigned char& absIndex,
  Ranks& ranksNew) const
{
  const unsigned char nx = static_cast<unsigned char>(north.length());
  const unsigned char sx = static_cast<unsigned char>(south.length());
  if (nx > 0 || sx > 0)
  {
    ranksNew.maxGlobalRank++;
    ranksNew.north.updateSeveral(ranksNew.maxGlobalRank, nx, 
      relIndex, absIndex);
    ranksNew.south.updateSeveral(ranksNew.maxGlobalRank, sx, 
      relIndex, absIndex);
  }

  const unsigned char oppr = static_cast<unsigned char>(opps.length());
  if (oppr > 0)
  {
    ranksNew.maxGlobalRank++;
    ranksNew.opps.updateSeveral(ranksNew.maxGlobalRank, oppr, 
      relIndex, absIndex);
  }
}


void Ranks::lowMinimal(
  const unsigned char criticalRank, 
  const Winner& winner, 
  unsigned char& relIndex, 
  unsigned char& absIndex, 
  Ranks& ranksNew) const
{
  // Do the x's, i.e. all N-S cards below the critical rank.
  // It is possible that N-S have no x's.
  const unsigned char nx = north.countBelow(criticalRank, winner.north);
  const unsigned char sx = south.countBelow(criticalRank, winner.south);
  if (nx > 0 || sx > 0)
  {
    ranksNew.maxGlobalRank++;
    ranksNew.north.updateSeveral(ranksNew.maxGlobalRank, nx, 
      relIndex, absIndex);
    ranksNew.south.updateSeveral(ranksNew.maxGlobalRank, sx, 
      relIndex, absIndex);
  }

  // Do the opponents' x's.  These must exist as a winner has
  // to beat something by definition.
  const unsigned char ox = opps.countBelow(criticalRank);
  assert(ox > 0);
  ranksNew.maxGlobalRank++;
  ranksNew.opps.updateSeveral(ranksNew.maxGlobalRank, ox, 
    relIndex, absIndex);
}


void Ranks::criticalMinimal(
  const Winner& winner,
  unsigned char& relIndex,
  unsigned char& absIndex,
  Ranks& ranksNew) const
{
  // Do declarer's counts at the critical rank.
  ranksNew.maxGlobalRank++;
  unsigned char nr = 
    (winner.north.getRank() == 0 ? 0 : winner.north.getDepth()+1);
  unsigned char sr = 
    (winner.south.getRank() == 0 ? 0 : winner.south.getDepth()+1);

  ranksNew.north.updateSeveral(ranksNew.maxGlobalRank, nr, 
    relIndex, absIndex);
  ranksNew.south.updateSeveral(ranksNew.maxGlobalRank, sr, 
    relIndex, absIndex);
}


void Ranks::remainingMinimal(
  const unsigned char criticalRank,
  unsigned char& relIndex,
  unsigned char& absIndex,
  Ranks& ranksNew) const
{
  unsigned char nr, sr, oppr;
  for (unsigned char rank = criticalRank+1; rank <= cards; rank++)
  {
    nr = north.count(rank);
    sr = south.count(rank);
    oppr = opps.count(rank);
    if (nr == 0 && sr == 0 && oppr == 0)
      break;

    ranksNew.maxGlobalRank++;
    if (oppr == 0)
    {
      ranksNew.north.updateSeveral(ranksNew.maxGlobalRank, nr, 
        relIndex, absIndex);
      ranksNew.south.updateSeveral(ranksNew.maxGlobalRank, sr, 
        relIndex, absIndex);
    }
    else if (nr == 0 && sr == 0)
      ranksNew.opps.updateSeveral(ranksNew.maxGlobalRank, oppr, 
        relIndex, absIndex);
    else
      assert(false);
  }
}


bool Ranks::getMinimals(
  const Result& result,
  list<CombReference>& minimals) const
{
  // Returns true and does not fill minimals if the combination is 
  // already minimal.  The implementation is slow and methodical...
  // It follows Ranks::setPlayers() in structure.

  const unsigned char cardsChar = static_cast<unsigned char>(cards);

  if (control.runRankComparisons())
  {
    if (cards <= 2)
    {
      // Always minimal.
// cout << "Minimal by definition\n";
      return true;
    }
    else if (result.winnersInt.winners.empty())
    {
// cout << "Calling no-winner minimalizeRanked with:\n" <<
  // cards << ", " << opps.length() << ", " <<
  // north.length() << ", " << south.length() << ", holding4 " <<
  // holding4 << endl;

      const unsigned h4minimal = minimalizeRanked(
        opps.length(), north.length(), south.length(), holding4);

      if (h4minimal != holding4)
      {
        // Arrange North and South in the same order that greater()
        // would choose.  In this case this is just the N-S lengths.
        CombReference combRef;
        combRef.rotateFlag = (south.length() > north.length());
        orientedBoth(combRef.rotateFlag, cards, h4minimal,
          combRef.holding3, combRef.holding2);
      }
    }
    else
    {
      for (auto& winner: result.winnersInt.winners)
      {
        const unsigned char criticalNumber = 
          min(winner.north.getAbsNumber(), winner.south.getAbsNumber());
      
        // TODO Probably a more elegant way to do this based on numbers.
        const unsigned char criticalRank = 
          max(winner.north.getRank(), winner.south.getRank());
        assert(criticalRank > 0);

/*
cout << "winner-based, winner " << winner.str() << endl;
cout << "critNo " << +criticalNumber << endl;
cout << "  N " << +winner.north.getNumber() << endl;
cout << "  S " << +winner.south.getNumber() << endl;
cout << "critRank " << +criticalRank << endl;
*/

        const unsigned oppsCount = opps.countBelow(criticalRank);
        const unsigned northCount = 
          north.countBelow(criticalRank, winner.north);
        const unsigned southCount =
          south.countBelow(criticalRank, winner.south);

        const unsigned h4minimal = minimalizeRanked(
          oppsCount, northCount, southCount, holding4);

        unsigned holdingMin3, holdingMin2;
        uncanonicalBoth(h4minimal, holdingMin3, holdingMin2);

/*
cout << "counts opp " << oppsCount << ", N " << northCount << ", S " <<
  southCount << endl;
cout << "holding4 was " << holding4 << endl;
cout << "h4min " << h4minimal << ", h3min " << holdingMin3 <<
  ", h2min " << holdingMin2 <<endl;
*/

        if (holding3 != holdingMin3)
        {
          CombReference combRef;
          combRef.holding3 = holdingMin3;
          combRef.holding2 = holdingMin2;
          minimals.emplace_back(combRef);
// cout << "Got a winner minimal different from original\n";
        }
      }
    }
  }
  else
  {
    if (result.winnersInt.winners.empty())
    {
// cout << "getMinimals: no winners, holding4 " << holding4 << "\n";
      Ranks ranksTmp;
      ranksTmp.resize(cards);
      ranksTmp.zero();
      ranksTmp.opps.setVoid();

      unsigned char absIndex = (cardsChar > 13 ? 0 : 13-cardsChar) + 1;
      unsigned char relIndex = 1;

      // Declarer wins no tricks on rank.
      Ranks::losingMinimal(relIndex, absIndex, ranksTmp);

// cout << "after losingMinimal:\n" << Ranks::strTable();
// cout << "ranksTmp.holding4 " << ranksTmp.holding4 << endl;

      ranksTmp.finishMinimal(holding3, minimals);
// cout << "after finishMinimal:\n" << Ranks::strTable();
    }
    else
    {
      for (auto& winner: result.winnersInt.winners)
      {
        Ranks ranksTmp;
        ranksTmp.resize(cards);
        ranksTmp.zero();
        ranksTmp.opps.setVoid();

        // An unset winner has rank 0.
        const unsigned char criticalRank = 
          max(winner.north.getRank(), winner.south.getRank());
        assert(criticalRank > 0);

        // Do both sides' low cards below the critical points.
        unsigned char absIndex = (cardsChar > 13 ? 0 : 13-cardsChar) + 1;
        unsigned char relIndex = 1;

        Ranks::lowMinimal(criticalRank, winner, 
          relIndex, absIndex, ranksTmp);

        // Do declarer's counts at the critical rank.
        Ranks::criticalMinimal(winner, relIndex, absIndex, ranksTmp);

        // Copy the remaining ranks.
        Ranks::remainingMinimal(criticalRank, relIndex, absIndex, ranksTmp);

        ranksTmp.finishMinimal(holding3, minimals);
      }
    }
  }

  // It can happen that we map to the same minimal holding in more ways.
  // Actually this can also include a rotation.  Here we use the
  // CombReference == comparator which only compares holding3 and
  // ignores holding2 and especially rotationFlag.

  minimals.sort();
  minimals.unique();

  if (control.outputHolding())
  {
    for (auto& m: minimals)
      cout << "Minimal holding: " << m.strSimple() << endl;
    cout << "\n";
  }

  return minimals.empty();
}


unsigned char Ranks::maxRank() const
{
  return maxGlobalRank;
}


string Ranks::strTable() const
{
  // Makes a table with one rank per line.
  stringstream ss;
  ss << right <<
    "Rank" <<
    north.strRankHeader() <<
    south.strRankHeader() <<
    opps.strRankHeader() <<
    "\n";

  for (unsigned char rank = maxGlobalRank; rank > 0; rank--) // Exclude void
    ss <<
      setw(4) << right << +rank <<
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

