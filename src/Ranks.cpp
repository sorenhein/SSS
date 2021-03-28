#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <utility>
#include <codecvt>
#include <mutex>
#include <cassert>

#include "Plays.h"
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

  namesNorthNew.clear();
  namesSouthNew.clear();
  namesOppsNew.clear();

  cardsNorth.clear();
  cardsSouth.clear();
  cardsOpps.clear();
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

  north.resize(cards);
  south.resize(cards);
  opps.resize(cards);

  maxRank = cards;

  namesNorthNew.resize(cards+1);
  namesSouthNew.resize(cards+1);
  namesOppsNew.resize(cards+1);

  cardsNorth.resize(cards+1);
  cardsSouth.resize(cards+1);
  cardsOpps.resize(cards+1);
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

  for (unsigned i = 0; i < namesNorthNew.size(); i++)
    namesNorthNew[i].clear();
  for (unsigned i = 0; i < namesSouthNew.size(); i++)
    namesSouthNew[i].clear();
  for (unsigned i = 0; i < namesOppsNew.size(); i++)
    namesOppsNew[i].clear();
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


void Ranks::setRanksNew()
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

  // Keep track of opponents for the string names.
  unsigned minRankOpps = 0;
  unsigned iMinOpps = 0;

  unsigned numberNorth = 0;
  unsigned numberSouth = 0;
  unsigned numberOpps = 0;
  unsigned depthNorth = 0;
  unsigned depthSouth = 0;
  unsigned depthOpps = 0;

  // TMP
  vector<string> namesNorth(cards+1);
  vector<string> namesSouth(cards+1);
  vector<string> namesOpps(cards+1);
  Ranks::strSetFullNames(namesNorth, namesSouth, namesOpps);

  // Have to set opps here already, as opps are not definitely void
  // but may be void, so we don't want the maximum values to get
  // reset to 0 by calling setVoid() after the loop below.
  opps.setVoid(true); // Have to do it first to make max come out right

  const unsigned imin = (cards > 13 ? 0 : 13-cards);
  unsigned h = holding;

// cout<<"Start"<<endl;
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

      opps.update(posOpps, maxRank, firstOpps);
      cardsOpps[numberOpps].set(maxRank, depthOpps, numberOpps, 
        CARD_NAMES[i]);
      depthOpps++;
      numberOpps++;

      // Update the strings.
      if (minRankOpps == 0)
      {
        minRankOpps = maxRank;
        iMinOpps = i;
      }

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

        if (north.ranks[posNorth].count > 0)
          posNorth++;
        if (south.ranks[posSouth].count > 0)
          posSouth++;
      }

      if (c == POSITION_NORTH)
      {
        north.update(posNorth, maxRank, firstNorth);
        cardsNorth[numberNorth].set(maxRank, depthNorth, numberNorth, 
          CARD_NAMES[i]);
        depthNorth++;
        numberNorth++;
      }
      else
      {
        south.update(posSouth, maxRank, firstSouth);
        cardsSouth[numberSouth].set(maxRank, depthSouth, numberSouth,
          CARD_NAMES[i]);
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

  vector<string> namesNorthNewer;
  namesNorthNewer.resize(namesNorthNew.size());
  for (unsigned n = numberNorth; n-- > 0; )
  {
assert(n < cardsNorth.size());
    auto&c = cardsNorth[n];
assert(c.getRank() < namesNorthNewer.size());
    namesNorthNewer[c.getRank()] += c.getName();
  }

  vector<string> namesSouthNewer;
  namesSouthNewer.resize(namesSouthNew.size());
  for (unsigned n = numberSouth; n-- > 0; )
  {
assert(n < cardsSouth.size());
    auto&c = cardsSouth[n];
assert(c.getRank() < namesSouth.size());
    namesSouth[c.getRank()] += c.getName();
  }

  if (north.len > 0)
  {
    for (unsigned r = 1; r <= maxRank; r++)
    {
      if (namesNorth[r] != namesNorthNewer[r])
      {
        cout << "ERR r " << r << ": '" <<
          namesNorth[r]<< "' vs '" << namesNorthNewer[r] << "'" <<endl;
        assert(false);
      }
    }
  }
  else
  {
    assert(numberNorth == 0);
  }

  if (south.len > 0)
  {
    for (unsigned r = 1; r <= maxRank; r++)
    {
      if (namesSouth[r] != namesSouthNewer[r])
      {
        cout << "ERR r " << r << ": '" <<
          namesSouth[r]<< "' vs '" << namesSouthNewer[r] << "'" <<endl;
        assert(false);
      }
    }
  }
  else
  {
    assert(numberSouth == 0);
  }


  // Make the last multiple opponent rank into x's if it seems
  // low enough.
/*
  if (namesOppsNew[minRankOpps].size() > 1 && iMinOpps <= 6) // An eight
  {
    namesOppsNew[minRankOpps] = string(namesOppsNew[minRankOpps].size(), 'x');
    minRankOpps++;
  }
  
  // Replace multiple opponent ranks from the top with HH, etc.
  unsigned index = 0;
  for (unsigned rank = maxRank+1; rank-- > 1; )
  {
    if (namesOppsNew[rank].size() > 1)
    {
      namesOppsNew[rank] = 
        string(namesOppsNew[rank].size(), GENERIC_NAMES[index]);
      index++;
    }
  }
*/
}


void Ranks::countNumbers(
  vector<unsigned>& numbers,
  const PositionInfo& posInfo) const
{
  if (posInfo.len == 0)
  {
    // TODO Hopefully won't be necessary in new code
    numbers.resize(1);
    return;
  }

  // The highest card has number len-1 and it's downhill from there.
  numbers.resize(posInfo.maxRank+1);
  unsigned running = posInfo.len-1;
  for (unsigned r = posInfo.maxRank; r > 0; r--)
  {
    numbers[r] = running;
    running -= posInfo.fullCount[r];
  }
}


void Ranks::setOrderTablesRemainder(PositionInfo& posInfo)
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

  const unsigned l = posInfo.maxRank+1;
  posInfo.remainders.clear();
  posInfo.remainders.resize(l);

  // r is the full-rank index that we're punching out.
  for (unsigned r = 1; r < l; r++)
  {
assert(r < posInfo.fullCount.size());
    if (posInfo.fullCount[r] == 0)
      continue;

assert(r < posInfo.remainders.size());
    vector<Sidewinner>& remList = posInfo.remainders[r];
    remList.resize(posInfo.len); // TODO len-1?

    // The position counts up from the lowest card which is 0.
    unsigned pos = 0;

    // Fill out remList with information about the remaining cards, 
    // starting from below.
    for (unsigned s = 1; s < l; s++)
    {
assert(s < posInfo.fullCount.size());
      const unsigned val = posInfo.fullCount[s];
      if (val == 0)
        continue;

      // The depth is 0 if this is the highest of equals of that rank.
      // If this is the card we're punching out, increase the depth by 1
      // by starting with the second such card.
      const unsigned start = (r == s ? 1 : 0);
      for (unsigned d = start; d < val; d++, pos++)
      {
assert(pos < remList.size());
// cout << "r " << r << " s " << s << ": d " << d << ", pos " << pos << 
  // ", remList length" << remList.size() << endl;
        remList[pos].set(s, d, pos);
      }
    }

    remList.resize(pos);
  }
}


void Ranks::setOrderTablesWin(
  PositionInfo& posInfo,
  const WinningSide side,
  const PositionInfo& otherInfo,
  const WinningSide otherSide)
{
  // NS win this trick, so the winner to which a later NS winner maps
  // is more complicated to determine than in setOrderTablesLose().
  // It can either be the current-trick or the later-trick winner.
  // Also, either of those can be a single-side or a two-side winner.

  const unsigned lThis = posInfo.maxRank+1;
  const unsigned lOther = otherInfo.maxRank+1;

  posInfo.best.clear();
  posInfo.best.resize(lThis);

  // Count the numbers of each rank.
  vector<unsigned> numThis, numOther;
  Ranks::countNumbers(numThis, posInfo);
  Ranks::countNumbers(numOther, otherInfo);

  unsigned crank;

  // rThis is the full-rank index of the posInfo that we're punching out.
  // The posInfo side may be void.
  // TODO No it can't really?
  for (unsigned rThis = 0; rThis < lThis; rThis++)
  {
    if (posInfo.fullCount[rThis] == 0)
      continue;

    posInfo.best[rThis].resize(lOther);
    
    // rOther is the full-rank index of the other card played.
    for (unsigned rOther = 0; rOther < lOther; rOther++)
    {
      if (otherInfo.fullCount[rOther] == 0)
        continue;

      // Will hopefully not be necessary in new code.
      if (rThis == 0 && rOther == 0)
        continue;

assert(rThis < posInfo.best.size());
assert(rOther < posInfo.best[rThis].size());

      Winner& current = posInfo.best[rThis][rOther];
      current.reset();
      if (rThis > rOther)
      {
        // The depth starts from 0.
assert(rThis < numThis.size());
        current.set(side, rThis, 0, numThis[rThis]);
        crank = rThis;
      }
      else if (rThis < rOther)
      {
assert(rOther < numOther.size());
        current.set(otherSide, rOther, 0, numOther[rOther]);
        crank = rOther;
      }
      else
      {
assert(rThis < numThis.size());
assert(rOther < numOther.size());
        // Make two sub-winners as NS in some sense choose.
        current.set(side, rThis, 0, numThis[rThis]);
        current.set(otherSide, rOther, 0, numOther[rOther]);
        crank = rThis;
      }

    }
  }
}


void Ranks::setOrderTables()
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
  // for South it is actually 3(1), 5(2), 5(1) with the highest
  // of equals being played first.
  //
  // If North plays the Q on the first trick, then North has 1, 8.
  // So this is the reduced or punched-out order table for North's Q.
  //
  // The purpose of these tables is to figure out the lowest winning
  // rank in the current combination that corresponds to a winner in 
  // a following combination.  Let's say NS win this trick with the Q.
  // Then if the lowest winner of the next combination is the ace,
  // we're going to stick with the Q.  Conceptually in this case,
  //
  // North winner[North plays Q][South plays 8] = 1(x), 6(Q!).
  // The last one is not the ace, as it is higher than the Q.
  // South winner[North plays Q][South plays 8] = 5(T), 5(J).
  //
  // We need tables for each of North and South, for the case when
  // we win or lose the current trick, and yielding the list of ranks
  // and depths that will be the lowest winners overall, taking into
  // account the winner of the following combination.

// cout << "setting remainder for North\n";
  Ranks::setOrderTablesRemainder(north);
// cout << "setting remainder for South\n";
  Ranks::setOrderTablesRemainder(south);

  Ranks::setOrderTablesWin(north, WIN_NORTH, south, WIN_SOUTH);
  Ranks::setOrderTablesWin(south, WIN_SOUTH, north, WIN_NORTH);
}


void Ranks::resizeOrderTablesWin(
  PositionInfo& posInfo,
  const PositionInfo& otherInfo)
{
  // NS win this trick, so the winner to which a later NS winner maps
  // is more complicated to determine than in setOrderTablesLose().
  // It can either be the current-trick or the later-trick winner.
  // Also, either of those can be a single-side or a two-side winner.

  const unsigned lThis = posInfo.maxRank+1;
  const unsigned lOther = otherInfo.maxRank+1;

  posInfo.best.resize(lThis);

  for (unsigned rThis = 0; rThis < lThis; rThis++)
  {
    if (posInfo.fullCount[rThis] == 0)
      continue;

    posInfo.best[rThis].resize(lOther);
  }
}


void Ranks::resizeOrderTables()
{
  Ranks::resizeOrderTablesWin(north, south);
  Ranks::resizeOrderTablesWin(south, north);
}


unsigned Ranks::canonicalTrinary(
  const vector<unsigned>& fullCount1,
  const vector<unsigned>& fullCount2) const
{
  // This is similar to canonicalNew, but only does holding3.
  // Actually it only generates a canonical holding3 if there is
  // no rank reduction among the opponents' cards.  Therefore
  // Combinations::getPtr looks up the canonical index.
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
  // Ranks::setRanks();
  Ranks::setRanksNew();
  // Ranks::setOrderTables();

  // TODO Activate.
  // Ranks::resizeOrderTables();

  combEntry.rotateFlag = ! (north.greater(south, opps));

  if (combEntry.rotateFlag)
    Ranks::canonicalBoth(south.fullCount, north.fullCount,
      combEntry.canonicalHolding3, combEntry.canonicalHolding2);
  else
   Ranks::canonicalBoth(north.fullCount, south.fullCount,
      combEntry.canonicalHolding3, combEntry.canonicalHolding2);

  combEntry.canonicalFlag = (holding == combEntry.canonicalHolding3);
}


void Ranks::trivialRanked(
  const unsigned tricks,
  const unsigned winRank,
  TrickEntry& trivialEntry) const
{
  // Play the highest card.
  if (north.maxRank == winRank)
    trivialEntry.set(tricks, WIN_NORTH, winRank, 0, north.len-1);

  if (south.maxRank == winRank)
    trivialEntry.set(tricks, WIN_SOUTH, winRank, 0, south.len-1);
}


bool Ranks::trivial(TrickEntry& trivialEntry) const
{
  if (north.len == 0 && south.len == 0)
  {
    trivialEntry.set(0, WIN_NONE, 0, 0, 0);
    return true;
  }

  if (opps.len == 0)
  {
    trivialEntry.set(max(north.len, south.len), WIN_NONE, 0, 0, 0);
    return true;
  }

  if (north.len <= 1 && south.len <= 1)
  {
    // North-South win their last trick if they have the highest card.
    if (opps.maxRank == maxRank)
      trivialEntry.set(0, WIN_NONE, 0, 0, 0);
    else
      Ranks::trivialRanked(1, opps.maxRank+1, trivialEntry);

    return true;
  }

  if (opps.len == 1)
  {
    if (opps.maxRank == maxRank)
      trivialEntry.set(max(north.len, south.len)-1, WIN_NONE, 0, 0, 0);
    else
      Ranks::trivialRanked(max(north.len, south.len), opps.maxRank+1, 
        trivialEntry);

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
  { 
    // If we have the top rank opposite a void, always play it.
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

  // If LHO plays a "king" and partner has the "ace", there is no
  // point in not playing the ace.
  if (toBeat == opps.maxRank &&
      partner.maxRank > toBeat)
    return false;

  // Play the lowest of irrelevant cards.
  return (pard == partner.minRank);
}


void Ranks::updateHoldings(
  const PositionInfo& leader,
  const PositionInfo& partner,
  const SidePosition side,
  unsigned& holding3,
  bool& rotateFlag) const
{
  if (leader.greater(partner, opps))
  {
// cout << "leader >= partner, side " << (side == SIDE_NORTH ?
  // "North" : "South") << endl;
    holding3 = Ranks::canonicalTrinary(leader.fullCount, partner.fullCount);
    // rotateFlag is absolute.
    // If the leader is South, then rotate.
    rotateFlag = (side == SIDE_SOUTH);
  }
  else
  {
// cout << "leader < partner, side " << (side == SIDE_NORTH ?
  // "North" : "South") << endl;
    holding3 = Ranks::canonicalTrinary(partner.fullCount, leader.fullCount);
    // If the leader is South, then don't rotate.
    rotateFlag = (side == SIDE_NORTH);
  }
}


void Ranks::logPlay(
  Plays& plays,
  const PositionInfo& leader,
  const PositionInfo& partner,
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

  vector<Sidewinner> const * leadOrderPtr;
  vector<Sidewinner> const * pardOrderPtr;

  if (lead == 0)
    leadOrderPtr = nullptr;
  else
  {
assert(lead < leader.remainders.size());
// cout << "Looking up lead " << lead << endl;
    leadOrderPtr = &leader.remainders[lead];
  }

  if (pard == 0)
    pardOrderPtr = nullptr;
  else
  {
assert(pard < partner.remainders.size());
// cout << "Looking up pard " << pard << endl;
    pardOrderPtr = &partner.remainders[pard];
  }

  Winner const * winPtr;
  if (! trickNS)
    winPtr = nullptr;
  else
  {
    // TODO Probably generate on the fly if not already set.
assert(lead < leader.best.size());
assert(pard < leader.best[lead].size());
    winPtr = &leader.best[lead][pard];
    
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
  PositionInfo& leader,
  PositionInfo& partner,
  const SidePosition side,
  const unsigned lead,
  const bool leadCollapse,
  Plays& plays)
{
  unsigned holding3;
  bool rotateFlag;
  bool pardCollapse, rhoCollapse;

  opps.fullCount[0]--;
  for (unsigned pardPos = partner.minPos; 
      pardPos <= partner.maxPos; pardPos++)
  {
    const unsigned pard = partner.ranks[pardPos].rank;
    if (! Ranks::pardOK(partner, lead, pard))
      continue;

    partner.fullCount[pard]--;
    pardCollapse = (pard > 1 && 
      pard < maxRank &&
      partner.fullCount[pard] == 0 &&
      leader.fullCount[pard] == 0);
    const unsigned toBeat = max(lead, pard);

    for (unsigned rhoPos = 1; rhoPos <= opps.maxPos; rhoPos++)
    {
      const unsigned rho = opps.ranks[rhoPos].rank;
      // If LHO is known to be void, RHO can duck completely.
      if (rho < toBeat && rho != opps.minRank)
        continue;

      opps.fullCount[rho]--;
      rhoCollapse = (opps.fullCount[rho] == 0);
          
      // Register the new play.
      Ranks::updateHoldings(leader, partner, side, holding3, rotateFlag);
      Ranks::logPlay(plays, leader, partner, side, lead, 0, pard, rho,
        leadCollapse, pardCollapse, holding3, rotateFlag);
      // plays.log(side, lead, 0, pard, rho, 
        // leadCollapse, false, pardCollapse, rhoCollapse,
        // holding3, rotateFlag);

      opps.fullCount[rho]++;

      // If RHO wins, he should do so as cheaply as possible.
      if (rho > toBeat)
        break;
    }
    partner.fullCount[pard]++;
  }
  opps.fullCount[0]++;
}


void Ranks::setPlaysLeadWithoutVoid(
  PositionInfo& leader,
  PositionInfo& partner,
  const SidePosition side,
  const unsigned lead,
  const bool leadCollapse,
  Plays& plays)
{
  unsigned holding3;
  bool rotateFlag;
  bool lhoCollapse, pardCollapse, rhoCollapse;

  for (unsigned lhoPos = 1; lhoPos <= opps.maxPos; lhoPos++)
  {
    const unsigned lho = opps.ranks[lhoPos].rank;
    opps.fullCount[lho]--;
    lhoCollapse = (opps.fullCount[lho] == 0);

    for (unsigned pardPos = partner.minPos; 
        pardPos <= partner.maxPos; pardPos++)
    {
      const unsigned pard = partner.ranks[pardPos].rank;
      if (! Ranks::pardOK(partner, max(lead, lho), pard))
        continue;

      partner.fullCount[pard]--;
      pardCollapse = (pard > 1 && 
        pard != maxRank &&
        partner.fullCount[pard] == 0 &&
        leader.fullCount[pard] == 0);

      for (unsigned rhoPos = 0; rhoPos <= opps.maxPos; rhoPos++)
      {
        const unsigned rho = opps.ranks[rhoPos].rank;

        // Maybe the same single card has been played already.
        if (opps.fullCount[rho] == 0)
          continue;
          
        opps.fullCount[rho]--;
        rhoCollapse = (rho > 0 && opps.fullCount[rho] == 0);

        // Register the new play.
        Ranks::updateHoldings(leader, partner, side, holding3, rotateFlag);
        Ranks::logPlay(plays, leader, partner, side, lead, lho, pard, rho,
          leadCollapse, pardCollapse, holding3, rotateFlag);
        // plays.log(side, lead, lho, pard, rho,
          // leadCollapse, lhoCollapse, pardCollapse, rhoCollapse,
          // holding3, rotateFlag);
      
        opps.fullCount[rho]++;
      }
      partner.fullCount[pard]++;
    }
    opps.fullCount[lho]++;
  }
}


void Ranks::setPlaysSide(
  PositionInfo& leader,
  PositionInfo& partner,
  const SidePosition side,
  Plays& plays)
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

  bool leadCollapse;

  for (unsigned leadPos = 1; leadPos <= leader.maxPos; leadPos++)
  {
    const unsigned lead = leader.ranks[leadPos].rank;
    if (! Ranks::leadOK(leader, partner, lead))
      continue;

    leader.fullCount[lead]--;
    leadCollapse = (leader.fullCount[lead] == 0 && 
      partner.fullCount[lead] == 0 &&
      lead != 1 &&
      lead != maxRank);

    // For optimization we treat the case separately where LHO is void.
    Ranks::setPlaysLeadWithVoid(leader, partner, side, lead,
      leadCollapse, plays);

    Ranks::setPlaysLeadWithoutVoid(leader, partner, side, lead,
      leadCollapse, plays);

    leader.fullCount[lead]++;
  }
}


CombinationType Ranks::setPlays(
  Plays& plays,
  TrickEntry& trivialEntry)
{
  // If COMB_TRIVIAL, then only terminalValue is set.
  // Otherwise, plays are set.

  if (Ranks::trivial(trivialEntry))
    return COMB_TRIVIAL;

  Ranks::setPlaysSide(north, south, SIDE_NORTH, plays);
  Ranks::setPlaysSide(south, north, SIDE_SOUTH, plays);
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


wstring Ranks::strPlays(
  const PositionInfo& posInfo,
  const vector<string>& names) const
{
  if (posInfo.len == 0)
    return L"-";

  string s = "";
  for (unsigned rank = maxRank; rank > 0; rank--)
    s += names[rank];

  wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
  return conv.from_bytes(s);
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


wstring Ranks::strDiagram() const
{
  vector<string> namesNorth(cards+1);
  vector<string> namesSouth(cards+1);
  vector<string> namesOpps(cards+1);
  Ranks::strSetFullNames(namesNorth, namesSouth, namesOpps);

  // Makes a little box out of Unicode characters.
  wstringstream ss;
  ss << 
    "    " << Ranks::strPlays(north, namesNorth) << "\n" <<
    "    " << L"\u2554\u2550\u2550\u2557\n" <<
    "    " << L"\u2551  \u2551 miss " <<
      Ranks::strPlays(opps, namesOpps) << "\n" <<
    "    " << L"\u255A\u2550\u2550\u255D\n" <<
    "    " << Ranks::strPlays(south, namesSouth) << "\n";
  return ss.str();
}

