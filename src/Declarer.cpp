#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Declarer.h"
#include "Opponents.h"

/*
 * This is a specialized class derived from Player.  Its members
 * only make sense (or are only used) for North and South, not for
 * opponents.
 */


void Declarer::resizeBest(const Declarer& partner)
{
  const unsigned lThis = maxRank+1;
  const unsigned lOther = partner.maxRank+1;

  best.resize(lThis);

  for (unsigned rThis = 0; rThis < lThis; rThis++)
  {
    if (rankInfo[rThis].count == 0)
      continue;

    best[rThis].resize(lOther);
  }
}


void Declarer::setVoid()
{
  if (len > 0)
    return;

  // A real void.
  minRank = 0;

  rankInfo[0].count = 1;
  rankInfo[0].ptr = nullptr;

  // For North-South, a void goes in cardsNew, cardsPtr and
  // ranksPtr.  This is recognized by forceFlag == false.

 maxRank = 0;
  cards.emplace_back(Card());
  cardsPtr.push_back(&cards.front());
  ranksPtr.push_back(&cards.front());
}


void Declarer::setSingleRank()
{
  singleRank = (len >= 1 && ranksPtr.size() == 1);
}


void Declarer::setNames()
{
  // As setVoid may create an extra "card", this must be done first.
  Declarer::setVoid();

  for (auto cit = cards.rbegin(); cit != cards.rend(); cit++)
    rankInfo[cit->getRank()].names += cit->getName();
}


void Declarer::fixDepths()
{
  // In Ranks we loop through the cards from the bottom up, so they
  // are numbered from 0 up.  Also, the depths within a rank are
  // numbered from 0 up.  The depths are not used that much, but
  // when we print the card in Card, it is useful for depth == 0 to
  // be the highest and not the lowest card within a rank.  In this
  // way we can easily decorate the names in a useful way.
  // We cannot avoid the problem by looping the other way in Ranks --
  // then we'd have the problem with the ranks.
  // So we fix (flip) the ranks from from e.g. 0, 1, 2 to 2, 1, 0
  // in this inelegant piece of post-processing.

  for (unsigned cno = 0; cno < cards.size(); cno++)
  {
    if (cards[cno].getDepth() == 1)
    {
      // Find the index past the current rank (may be beyond the end).
      unsigned dno = cno+1;
      while (dno < cards.size() && cards[dno].getDepth() > 0)
        dno++;

      unsigned maxDepth = dno - cno;

      // Flip the ranks, starting from the zero depth.
      for (cno--; cno < dno; cno++)
        cards[cno].flipDepth(maxDepth);
    }
  }
}


void Declarer::setRemainders()
{
  // Example: North AQx, South JT8, defenders have 7 cards.
  //
  // Rank Cards North South
  //    1     x     1     0
  //    2   3-7     0     0
  //    3     8     0     1
  //    4     9     0     0
  //    5    JT     0     2
  //    6     Q     1     0
  //    7     K     0     0
  //    8     A     1     0
  //
  // Then the order table for North (from below) is 1, 6, 8.
  // For South it is 3, 5, 5.  Each of these also has a depth, so
  // for South it is actually 3(0), 5(1), 5(0) with the highest
  // of equals being played first.
  //
  // If North plays the Q on the first trick, then North has 1, 8.
  // So this is the reduced or punched-out order table for North's Q.
  //
  // The purpose of these tables is to figure out the lowest winning
  // rank in the current combination that corresponds to
  // a following combination.
  //
  // If the posInfo side is void, there is an (unset) entry at
  // position 0 of the list for the void "card".

  const unsigned l = maxRank+1;
  remainders.clear();
  remainders.resize(l);

  // r is the full-rank index that we're punching out.
  for (unsigned r = 1; r < l; r++)
  {
    if (rankInfo[r].count == 0)
      continue;

assert(r < remainders.size());
    vector<Card>& remList = remainders[r];
    remList.resize(len); // TODO len-1?

    // The position counts up from the lowest card which is 0.
    unsigned pos = 0;
    unsigned number = 0;

    // Fill out remList with information about the remaining cards,
    // starting from below.
    for (unsigned s = 1; s < l; s++)
    {
      const unsigned val = rankInfo[s].count;
      if (val == 0)
        continue;

      // The depth is 0 if this is the highest of equals of that rank.
      // If this is the card we're punching out, increase the depth by 1
      // by starting with the second such card.
      const unsigned start = (r == s ? 1 : 0);

/*
      for (unsigned d = start; d < val; d++, pos++)
      {
assert(pos < remList.size());
// TODO Use cardsNorth from Ranks once it's over here instead?
// cout << "r " << r << " s " << s << ": d " << d << ", pos " << pos <<
  // ", remList length" << remList.size() << endl;
        remList[pos].set(s, d, pos, rankInfo[s].names.at(d));
      }
*/

      // Start at the highest depth.
      for (unsigned d = val; d-- > start; pos++, number++)
      {
assert(pos < remList.size());
        remList[pos].set(s, d, number, rankInfo[s].names.at(d));
      }

      if (r == s)
        number++;


    }

    remList.resize(pos);
    assert(pos+1 == cards.size());

    /*
    for (unsigned p = 0, cno = 0; p < pos; p++, cno++)
    {
      if (cards[cno].getRank() == r &&
          cards[cno].getDepth() == 0)
        cno++;

      if (!cards[cno].identical(remList[p]))
      {
wcout << Player::wstr() << endl;
        cout << "p " << p << ", cno " << cno << ", r " << r << endl;
        cout << "remList (presumed OK)\n";
        for (unsigned p1 = 0; p1 < pos; p1++)
          cout << p1 << ": " << remList[p1].strDebug(Player::playerName());
        cout << endl << "cards list (trying to match)\n";
        for (unsigned c1 = 0; c1 < cards.size(); c1++)
          cout << c1 << ": " << cards[c1].strDebug(Player::playerName());
        cout << endl;
        assert(cards[cno].identical(remList[p]));
      }
    }
    */
  }
}


void Declarer::countNumbers(vector<unsigned>& numbers) const
{
  // TODO Probably we can just use cards to look this up?
  if (len == 0)
  {
    // TODO Hopefully won't be necessary long-term.
    numbers.resize(1);
    return;
  }

  numbers.resize(maxRank+1);
  unsigned running = len-1;
  for (unsigned r = maxRank; r > 0; r--)
  {
    numbers[r] = running;
    running -= rankInfo[r].count;
  }
}


void Declarer::setBest(const Declarer& partner)
{
  // NS win this trick, so the winner to which a later NS winner maps
  // is more complicated to determine than in setOrderTablesLose().
  // It can either be the current-trick or the later-trick winner.
  // Also, either of those can be a single-side or a two-side winner.

  const unsigned lThis = maxRank+1;
  const unsigned lOther = partner.maxRank+1;

  best.clear();
  best.resize(lThis);

  // Count the numbers of each rank.
  vector<unsigned> numThis, numOther;
  Declarer::countNumbers(numThis);
  partner.countNumbers(numOther);

  unsigned crank;

  // TODO Can we store CardPosition in Winner, too?  Or do we need
  // WinningSide?
  assert(side != POSITION_OPPS);
  WinningSide wside, pside;
  if (side == POSITION_NORTH)
  {
    wside = WIN_NORTH;
    pside = WIN_SOUTH;
  }
  else
  {
    wside = WIN_SOUTH;
    pside = WIN_NORTH;
  }

  // rThis is the full-rank index of the posInfo that we're punching out.
  // The posInfo side may be void.
  // TODO No it can't really?
  for (unsigned rThis = 0; rThis < lThis; rThis++)
  {
    if (rankInfo[rThis].count == 0)
      continue;

    best[rThis].resize(lOther);

    // rOther is the full-rank index of the other card played.
    for (unsigned rOther = 0; rOther < lOther; rOther++)
    {
      if (partner.rankInfo[rOther].count == 0)
        continue;

      // Will hopefully not be necessary in new code.
      if (rThis == 0 && rOther == 0)
        continue;

assert(rThis < best.size());
assert(rOther < best[rThis].size());

      Winner& current = best[rThis][rOther];
      current.reset();
      if (rThis > rOther)
      {
        // The depth starts from 0.
assert(rThis < numThis.size());

// Card ctmp;
// ctmp.set(rThis, 0, numThis[rThis], names[rThis].at(0));
// assert(rThis < cardsPtr.size());
// assert(ctmp.identical(* cardsPtr[rThis]));

        current.set(wside, rThis, 0, numThis[rThis], rankInfo[rThis].names.at(0));
        crank = rThis;
      }
      else if (rThis < rOther)
      {
assert(rOther < numOther.size());
        current.set(pside, rOther, 0, numOther[rOther],
          partner.rankInfo[rOther].names.at(0));
        crank = rOther;
      }
      else
      {
assert(rThis < numThis.size());
assert(rOther < numOther.size());
        // Make two sub-winners as NS in some sense choose.
        current.set(wside, rThis, 0, numThis[rThis],
          rankInfo[rThis].names.at(0));
        current.set(pside, rOther, 0, numOther[rOther],
          partner.rankInfo[rOther].names.at(0));
        crank = rThis;
      }
    }
  }
}


void Declarer::finish(const Declarer& partner)
{
  Declarer::setSingleRank();
  Declarer::fixDepths();
  Declarer::setRemainders();
  Declarer::setBest(partner);
}


bool Declarer::greater(
  const Declarer& p2,
  const Opponents& opps) const
{
  // There may be rank collapses from played EW cards.
  unsigned run1 = 0;
  unsigned run2 = 0;
  for (unsigned r = max(maxRank, p2.maxRank); ; r -= 2)
  {
    run1 += rankInfo[r].count;
    run2 += p2.rankInfo[r].count;

    // TODO Could use a has() method?
    // if (r > 2 && opps.rankInfo[r-1].count == 0)
    if (r > 2 && ! opps.hasRank(r-1))
      continue;  // EW collapse
    else if (run1 > run2)
      return true;
    else if (run1 < run2)
      return false;
    else if (r <= 2)
      return true; // Nothing else happens, so equality
    
    run1 = 0;
    run2 = 0;
  }
}


const Card& Declarer::top() const
{
  assert(! cards.empty());
  return cards.back();
}


const vector<Card>& Declarer::remainder(const unsigned rank) const
{
  assert(rank < remainders.size());
  return remainders[rank];
}


const Winner& Declarer::getWinner(
  const unsigned lead,
  const unsigned pard) const
{
  assert(lead < best.size());
  assert(pard < best[lead].size());
  return best[lead][pard];
}


const deque<Card const *>& Declarer::getCards(const bool fullFlag) const
{
  if (fullFlag)
    return cardsPtr;
  else
    return ranksPtr;
}


bool Declarer::isSingleRanked() const
{
  return singleRank;
}

