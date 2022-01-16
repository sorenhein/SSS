/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cassert>

#include "Ranks.h"

#include "cranked.h"
#include "canon.h"

#include "../plays/Plays.h"
#include "../combinations/CombEntry.h"
#include "../inputs/Control.h"

#include ".././const.h"

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


Ranks::Ranks()
{
  mtxRanks.lock();
  if (! init_flag)
  {
    setRankedConstants();
    setCanonicalConstants();
    init_flag = true;
  }
  mtxRanks.unlock();
}


void Ranks::resize(const unsigned char cardsIn)
{
  cards = cardsIn;

  north.resize(cards, SIDE_NORTH);
  south.resize(cards, SIDE_SOUTH);
  opps.resize(cards, SIDE_OPPS);
}


unsigned char Ranks::size() const
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

  if (control.runRankComparisons())
  {
    completion.resize(cards);
    completion.set(north.getCards(true), south.getCards(true));
  }
}


void Ranks::setReference(CombEntry& centry) const
{
  if (control.runRankComparisons())
  {
    unsigned holding3Ref, holding2Ref;
    const bool rotateFlag = ! (north.tops(south));

    rankedBoth(rotateFlag, cards, holding4, holding3Ref, holding2Ref);
    
    centry.setReferenceHolding(holding3Ref, holding2Ref, rotateFlag);
  }
  else
  {
    // combRef.rotateFlag = ! (north.greater(south, opps));
    unsigned holding3Ref, holding2Ref;
    const bool rotateFlag = ! (north.greater(south, opps));

    if (rotateFlag)
    {
      canonicalBoth(south, north, opps, maxGlobalRank,
        holding3Ref, holding2Ref);
    }
    else
    {
      canonicalBoth(north, south, opps, maxGlobalRank,
        holding3Ref, holding2Ref);
    }
    centry.setReferenceHolding(holding3Ref, holding2Ref, rotateFlag);
  }
}


void Ranks::setRanks(
  const unsigned holding3In,
  CombEntry& combEntry)
{
  holding3 = holding3In;

  Ranks::setPlayers();

  Ranks::setReference(combEntry);

  combEntry.setReference(holding3 == combEntry.getHolding3());
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
    Winner winner;
    winner.setHigherOf(north.top(), south.top());
    trivial.set(0, tricks, winner);
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
      static_cast<unsigned char>(max(north.length(), south.length())), 
        trivial);
    return true;
  }

  return false;
}


bool Ranks::sideOKRanked(
  const Declarer& leader,
  const Declarer& partner) const
{
  if (partner.isSingleRun())
  {
    if (! leader.isSingleRun())
    {
      // Always lead the only "singleton" (maybe not highest card).
      return false;
    }
    else if (partner.minAbsNumber() > leader.minAbsNumber())
    {
      // Lead the higher "singleton" (maybe not highest card).
     return false;
    }
  }

  // Don't lead a card that's higher than partner's best one.
  if (! leader.isSingleRun())
  {
    if (leader.minAbsNumber() > partner.maxAbsNumber())
      return false;
  }

  return true;
}


bool Ranks::sideOKUnranked(
  const Declarer& leader,
  const Declarer& partner,
  const Play& play) const
{
  if (partner.isSingleRanked())
  {
    // Always lead the single rank.
    if (! leader.isSingleRanked())
      return false;
    // If both have single ranks, lead the higher one.
    else if (leader.maxFullRank() < partner.maxFullRank())
      return false;
    // If they're the same, lead from North.
    if (leader.maxFullRank() == partner.maxFullRank() && 
        play.side == SIDE_SOUTH)
    {
      // Pick North in case of apparent symmetry.
      return false;
    }
  }

  // Don't lead a card that's higher than partner's best one.
  if (! leader.isSingleRanked())
  {
    if (leader.minFullRank() >= partner.maxFullRank())
      return false;
  }

  return true;
}


bool Ranks::sideOK(
  const Declarer& leader,
  const Declarer& partner,
  const Play& play) const
{
  if (leader.isVoid())
    return false;
  else if (partner.isVoid())
    return true;
  else if (control.runRankComparisons())
    return Ranks::sideOKRanked(leader, partner);
  else
    return Ranks::sideOKUnranked(leader, partner, play);
}


bool Ranks::leadOKRanked(
  const Declarer& leader,
  const Declarer& partner,
  Card const * leadPtr) const
{
  // Both sides have 2+ ranks.  
  const unsigned char leadAbsNumber = leadPtr->getAbsNumber();
  if (leadAbsNumber < partner.minAbsNumber() &&
      leadAbsNumber > leader.minAbsNumber())
  {
    // If leader's lowest number is enough, lead lowest.
    return false;
  }
  else if (leadAbsNumber > partner.maxAbsNumber())
  {
    // Again, don't lead a too-high card.
    return false;
  }
  else
    return true;
}


bool Ranks::leadOKUnranked(
  const Declarer& leader,
  const Declarer& partner,
  const unsigned char lead) const
{
  // Both sides have 2+ ranks.  
  if (lead <= partner.minFullRank() && 
    lead > leader.minFullRank())
  {
    // If partner's lowest rank is at least as high, lead lowest.
    return false;
  }
  else if (lead >= partner.maxFullRank())
  {
    // Again, don't lead a too-high card.
    return false;
  }
  else
    return true;
}


bool Ranks::leadOK(
  const Declarer& leader,
  const Declarer& partner,
  Card const * leadPtr,
  const unsigned char lead) const
{
  // By construction, count is always > 0.
  if (partner.isVoid())
  { 
    // If we have the top rank opposite a void, always play it.
    if (leader.maxFullRank() == maxGlobalRank && lead < maxGlobalRank)
      return false;
    else
      return true;
  }
  else if (leader.isSingleRanked())
  {
    // We already did all we could in sideOK.
    return true;
  }
  else if (control.runRankComparisons())
    return Ranks::leadOKRanked(leader, partner, leadPtr);
  else
    return Ranks::leadOKUnranked(leader, partner, lead);
}


bool Ranks::pardOK(
  const Declarer& partner,
  Card const * pardPtr,
  const unsigned char toBeat,
  const unsigned char pard) const
{
  // Always "play" a void.
  if (partner.isVoid())
    return true;

  // No rule concerning high cards.
  if (pard > toBeat)
    return true;

  // Permit same-rank plays only if ranks matter.
  if (pard == toBeat && control.runRankComparisons())
    return true;

  // If LHO plays a "king" and partner has the "ace", there is no
  // point in not playing the ace.
  if (toBeat == opps.maxFullRank() &&
      partner.maxFullRank() > toBeat)
    return false;

  // Play the lowest of irrelevant cards.
  if (control.runRankComparisons())
    return (pardPtr->getAbsNumber() == partner.minAbsNumber());
  else
    return (pard == partner.minFullRank());
}


bool Ranks::rhoOK(
  const unsigned toBeat,
  const unsigned rho) const
{
  // This only applies when "partner" (mostly LHO) is known to be void.
  if (rho > toBeat)
    return true;
  else if (rho != opps.minFullRank())
  {
    // If ducking, duck low.
    return false;
  }
  else if (! control.runRankComparisons() && opps.maxFullRank() > toBeat)
  {
    // Don't duck if you can win.
    return false;
  }
  else
    return true;
}


void Ranks::updateHoldings(Play& play) const
{
  if (control.runRankComparisons())
  {
    // Don't do any canonical reduction -- just play the cards.
    // We can't use Declarer::tops() here as only rankInfo has
    // been updated with the plays, so isVoid() and top() don't work.
    // Instead we do some bit magic to find the highest remaining card.

    rankedTrinary(play.cardsLeft, holding4, play,
      play.holding3, play.rotateFlag);
  }
  else
  {
    // TODO The top aspect of greater seems wrong here.
    // See Declarer.cpp, greater()
    if (north.greater(south, opps))
    {
      play.holding3 = canonicalTrinary(north, south, opps, maxGlobalRank);

      // North (without the played card) is still >= South (without
      // the played card).  So we can justify not rotating, but see
      // also the comment in canonicalTrinary().
      play.rotateFlag = false;
    }
    else
    {
      // South is strictly greater than North, so there is no
      // ambiguity here.
      play.holding3 = canonicalTrinary(south, north, opps, maxGlobalRank);
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

  play.completionPtr = &completion;

  if (play.trickNS)
  {
    // TODO Is this only needed if runRankComparisons?

    Card const * northCardPtr, * southCardPtr;
    if (play.side == SIDE_NORTH)
    {
      northCardPtr = play.leadPtr;
      southCardPtr = play.pardPtr;
    }
    else
    {
      northCardPtr = play.pardPtr;
      southCardPtr = play.leadPtr;
    }

    if (* northCardPtr > * southCardPtr)
    {
      // Consider the next-higher South card of that rank played as well.
      southCardPtr = completion.get(northCardPtr->getAbsNumber());
      if (! southCardPtr)
      {
        play.currBest.set(SIDE_NORTH, * northCardPtr);
        return;
      }
    }
    else
    {
      northCardPtr = completion.get(southCardPtr->getAbsNumber());
      if (! northCardPtr)
      {
        play.currBest.set(SIDE_SOUTH, * southCardPtr);
        return;
      }
    }

    // The general case.
    play.currBest.setBoth(* northCardPtr, * southCardPtr);
  }
}


void Ranks::setPlaysLeadLHOVoid(
  Declarer& leader,
  Declarer& partner,
  const unsigned char lead,
  Play& play,
  Plays& plays)
{
  opps.playRank(0);
  play.lhoPtr = opps.voidPtr();

  for (auto& pardPtr: partner.getCards(control.runRankComparisons()))
  {
    play.pardPtr = pardPtr;
    const unsigned char pard = pardPtr->getRank();
    if (! Ranks::pardOK(partner, pardPtr, lead, pard))
      continue;

    play.pardCollapse = partner.playRank(pard, leader, maxGlobalRank);

    const unsigned char toBeat = max(lead, pard);

    for (auto& rhoPtr: opps.getCards())
    {
      play.rhoPtr = rhoPtr;
      const unsigned char rho = rhoPtr->getRank();
      if (! Ranks::rhoOK(toBeat, rho))
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


void Ranks::setPlaysLeadLHONotVoid(
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

    for (auto& pardPtr: partner.getCards(control.runRankComparisons()))
    {
      play.pardPtr = pardPtr;
      const unsigned char pard = pardPtr->getRank();
      if (! Ranks::pardOK(partner, pardPtr, max(lead, lho), pard))
        continue;

      play.pardCollapse = partner.playRank(pard, leader, maxGlobalRank);

      // As LHO is not void, RHO may show out.  We do this separately,
      // as it is more convenient to store the plays in Player this way.
      // We don't need to "play" the void, as it does not affect
      // the holdings.

      if (! partner.isVoid() || Ranks::rhoOK(lead, lho))
      {
        // Register the void play for this LHO play.
        play.rhoPtr = opps.voidPtr();
        Ranks::finish(play);
        plays.log(play);
      }
      
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
  if (! Ranks::sideOK(leader, partner, play))
    return;

  for (auto& leadPtr: leader.getCards(control.runRankComparisons()))
  {
    // I wish I could write for (play.leadPtr: ...), but a declaration
    // is required.
    play.leadPtr = leadPtr;
    const unsigned char lead = play.leadPtr->getRank();
    if (! Ranks::leadOK(leader, partner, leadPtr, lead))
      continue;

    play.leadCollapse = leader.playRank(lead, partner, maxGlobalRank);

    // For optimization we treat the case separately where LHO is void.
    Ranks::setPlaysLeadLHOVoid(leader, partner, lead, play, plays);
    Ranks::setPlaysLeadLHONotVoid(leader, partner, lead, play, plays);

    leader.restoreRank(lead);
  }
}


CombinationType Ranks::setPlays(
  Plays& plays,
  Result& trivial)
{
  // If COMB_TRIVIAL, only terminalValue is set.
  // Otherwise, plays are set.

  if (Ranks::makeTrivial(trivial))
    return COMB_TRIVIAL;

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


void Ranks::addMinimal(
  const unsigned char absNumber,
  CombEntry& centry,
  bool& ownFlag) const
{
  unsigned oppsCount, northCount, southCount;
  bool rotateFlag;

  if (absNumber == 0 || absNumber == UCHAR_NOT_SET)
  {
    // Arrange North and South by length, which is all it takes here.
    oppsCount = opps.length();
    northCount = north.length();
    southCount = south.length();
    rotateFlag = false;
  }
  else
  {
    // As there is a winner, North will still have the 
    // highest card and there will never be a rotation.
    oppsCount = opps.countBelowAbsNumber(absNumber);
    northCount = north.countBelowAbsNumber(absNumber);
    southCount = south.countBelowAbsNumber(absNumber);
    rotateFlag = false;
  }

  // Keep the orientation.
  const unsigned h4minimal = rankedMinimalize(rotateFlag, cards,
    oppsCount, northCount, southCount, holding4);

  if (h4minimal != holding4)
  {
    unsigned holding3Min;
    rankedTrinary(rotateFlag, cards, h4minimal, holding3Min);

    centry.addMinimal(holding3Min, rotateFlag);

/*
if (h4minimal < holding4)
{
  cout << "holding4 " << holding4 << ", h4minimal " << h4minimal << endl;
assert(h4minimal > holding4);
}
*/
  }
  else
    ownFlag = true;
}


bool Ranks::getMinimals(
  const list<Result>& resultList,
  CombEntry& centry) const
{
  // TODO If ownFlag stays, combine with bool return value somehow.

  // In the context of rank comparisons, fills out the minimals.
  assert(control.runRankComparisons());

  bool ownFlag = false;
  if (cards <= 2)
  {
    // Always minimal.
    ownFlag = true;
  }
  else if (resultList.empty())
  {
    Ranks::addMinimal(0, centry, ownFlag);
  }
  else
  {
    for (auto& res: resultList)
      Ranks::addMinimal(res.winAbsNumber(), centry, ownFlag);
  }

  centry.consolidateMinimals();

  // if (ownFlag && ! centry.minimalsEmpty())
  if (ownFlag)
    centry.addMinimalSelf();

  if (control.outputHolding())
    cout << centry.strMinimals();

  // The combination is only non-minimal if its minimals (1+)
  // are all different from itself.
  return (ownFlag || centry.minimalsEmpty());
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

  // Excludes void.
  for (unsigned char rank = maxGlobalRank; rank > 0; rank--) 
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

