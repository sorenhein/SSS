#include <iostream>
#include <iomanip>
#include <sstream>
#include <codecvt>
#include <cassert>

#include "Player.h"

#include "struct.h"
#include "const.h"

/*
 * This class keeps track of player holdings within Ranks.  There is
 * one instance for North, one for South, and one for opposing cards.
 */


Player::Player()
{
  Player::clear();
}


Player::~Player()
{
}


void Player::clear()
{
  ranks.clear();
  fullCount.clear();
  names.clear();
  cards.clear();
}


void Player::resize(
  const unsigned cardsIn,
  const CardPosition sideIn)
{
  // Worst case, leaving room for voids at rank 0.
  ranks.resize(cardsIn+1);
  maxPos = cardsIn;

  fullCount.resize(cardsIn+1);
  names.resize(cardsIn+1);
  cards.resize(cardsIn+1);

  cardCount = cardsIn;
  maxRank = cardsIn;

  side = sideIn;
}


void Player::resizeBest(const Player& partner)
{
  const unsigned lThis = maxRank+1;
  const unsigned lOther = partner.maxRank+1;

  best.resize(lThis);

  for (unsigned rThis = 0; rThis < lThis; rThis++)
  {
    if (fullCount[rThis] == 0)
      continue;

    best[rThis].resize(lOther);
  }
}


void Player::zero()
{
  for (unsigned pos = 0; pos <= maxPos; pos++)
    ranks[pos].count = 0;

  for (unsigned rank = 0; rank <= maxRank; rank++)
    fullCount[rank] = 0;

  for (unsigned i = 0; i < names.size(); i++)
    names[i].clear();
        
  len = 0;

  numberNextCard = 0;
  firstUpdateFlag = true;

  maxRank = cardCount;

  depthNext = 0;
  posNext = 1;
}


void Player::updateStep(const unsigned rankNew)
{
  // Back down to the first card of the next rank.
  depthNext = 0;

  if (rankNew == maxRank+2)
  {
    // Player has this reduced rank already, so we advance.
    posNext++;
  }
}


void Player::update(
  const unsigned rank,
  const unsigned absCardNumber)
{
  ranks[posNext].add(rank);
  maxPos = posNext;

  fullCount[rank]++;
  maxRank = rank;

  if (firstUpdateFlag)
  {
    minPos = posNext;
    minRank = rank;
    minAbsCardNumber = absCardNumber;
    firstUpdateFlag = false;
  }

  len++;
  
  cards[numberNextCard].set(rank, depthNext, numberNextCard, 
    CARD_NAMES[absCardNumber]);
  numberNextCard++;
  depthNext++;
}


void Player::setVoid(const bool forceFlag)
{
  if (forceFlag || len == 0)
  {
    ranks[0].add(0);
    minPos = 0;
    maxPos = 0;

    fullCount[0] = 1;
    minRank = 0;

    // Only null out maxRank when it's a real void, and not just
    // a preliminary void for opponents at the beginning of setRanks().
    if (! forceFlag)
      maxRank = 0;
  }
}


void Player::setSingleRank()
{
  singleRank = (len >= 1 && minRank == maxRank);
}


void Player::setNames(const bool declarerFlag)
{
  if (declarerFlag)
  {
    // TODO Could do something with rbegin and rend
    for (unsigned number = len; number-- > 0; )
    {
      auto& c = cards[number];
      names[c.getRank()] += c.getName();
    }
  }
  else
  {
    // For the opponents we simplify more.
    unsigned index = 0;
    unsigned rankPrev = numeric_limits<unsigned>::max();

    // TODO Could do something with rbegin and rend
    for (unsigned number = len; number-- > 0; )
    {
      auto& c = cards[number];
      const unsigned r = c.getRank();
      if (r == rankPrev)
        continue;

      rankPrev = r;
      if (fullCount[r] > 1)
      {
        if (r == minRank && minAbsCardNumber <= 6) // ~ an eight
          names[r] = string(fullCount[r], 'x');
        else
          names[r] = string(fullCount[r], GENERIC_NAMES[index]);
        index++;
      }
      else if (fullCount[r] == 1)
      {
        names[r] = c.getName();
      }
    }
  }
}


void Player::setRemainders()
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
assert(r < fullCount.size());
    if (fullCount[r] == 0)
      continue;

assert(r < remainders.size());
    vector<Card>& remList = remainders[r];
    remList.resize(len); // TODO len-1?

    // The position counts up from the lowest card which is 0.
    unsigned pos = 0;

    // Fill out remList with information about the remaining cards,
    // starting from below.
    for (unsigned s = 1; s < l; s++)
    {
assert(s < fullCount.size());
      const unsigned val = fullCount[s];
      if (val == 0)
        continue;

      // The depth is 0 if this is the highest of equals of that rank.
      // If this is the card we're punching out, increase the depth by 1
      // by starting with the second such card.
      const unsigned start = (r == s ? 1 : 0);
      for (unsigned d = start; d < val; d++, pos++)
      {
assert(pos < remList.size());
// TODO Use cardsNorth from Ranks once it's over here instead?
// cout << "r " << r << " s " << s << ": d " << d << ", pos " << pos <<
  // ", remList length" << remList.size() << endl;
        remList[pos].set(s, d, pos, names[s].at(d));
      }
    }

    remList.resize(pos);
  }
}


void Player::countNumbers(vector<unsigned>& numbers) const
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
    running -= fullCount[r];
  }
}


void Player::setBest(const Player& partner)
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
  Player::countNumbers(numThis);
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
    if (fullCount[rThis] == 0)
      continue;

    best[rThis].resize(lOther);

    // rOther is the full-rank index of the other card played.
    for (unsigned rOther = 0; rOther < lOther; rOther++)
    {
      if (partner.fullCount[rOther] == 0)
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
        current.set(wside, rThis, 0, numThis[rThis], names[rThis].at(0));
        crank = rThis;
      }
      else if (rThis < rOther)
      {
assert(rOther < numOther.size());
        current.set(pside, rOther, 0, numOther[rOther],
          partner.names[rOther].at(0));
        crank = rOther;
      }
      else
      {
assert(rThis < numThis.size());
assert(rOther < numOther.size());
        // Make two sub-winners as NS in some sense choose.
        current.set(wside, rThis, 0, numThis[rThis],
          names[rThis].at(0));
        current.set(pside, rOther, 0, numOther[rOther],
          partner.names[rOther].at(0));
        crank = rThis;
      }
    }
  }
}


void Player::playFull(const unsigned rankFullIn)
{
  assert(fullCount[rankFullIn] > 0);
  fullCount[rankFullIn]--;
}


void Player::restoreFull(const unsigned rankFullIn)
{
  fullCount[rankFullIn]++;
}


bool Player::greater(
  const Player& p2,
  const Player& opps) const
{
  // There may be rank collapses from played EW cards.
  unsigned run1 = 0;
  unsigned run2 = 0;
  for (unsigned r = max(maxRank, p2.maxRank); ; r -= 2)
  {
    run1 += fullCount[r];
    run2 += p2.fullCount[r];
    if (r > 2 && opps.fullCount[r-1] == 0)
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


const Card& Player::top() const
{
  assert(len > 0);
  return cards[len-1];
}


bool Player::hasRank(const unsigned rankIn) const
{
  return (fullCount[rankIn] > 0);
}


unsigned Player::length() const
{
  return len;
}


unsigned Player::minFullRank() const
{
  return minRank;
}


unsigned Player::maxFullRank() const
{
  return maxRank;
}


unsigned Player::minNumber() const
{
  return minPos;
}


unsigned Player::maxNumber() const
{
  return maxPos;
}


unsigned Player::rankOfNumber(const unsigned no) const
{
  assert(no < ranks.size());
  return ranks[no].rank;
}


const vector<Card>& Player::remainder(const unsigned rank) const
{
  assert(rank < remainders.size());
  return remainders[rank];
}


const Winner& Player::getWinner(
  const unsigned lead,
  const unsigned pard) const
{
  assert(lead < best.size());
  assert(pard < best[lead].size());
  return best[lead][pard];
}


bool Player::isVoid() const
{
  return (len == 0);
}


bool Player::isSingleRanked() const
{
  return singleRank;
}


unsigned Player::count(const unsigned rankIn) const
{
  return fullCount[rankIn];
}


string Player::playerName() const
{
  if (side == POSITION_NORTH)
    return "North";
  else if (side == POSITION_SOUTH)
    return "South";
  else
    return "Opps";
}


string Player::strRankHeader() const
{
  stringstream ss;
  ss << right <<
    setw(8) << Player::playerName() <<
    setw(4) << "#" <<
    setw(6) << "cards";

  return ss.str();
}


string Player::strRank(const unsigned rank) const
{
  stringstream ss;
  if (fullCount[rank] == 0)
    ss << setw(8) << "-" << setw(4) << "-" << setw(6) << "-";
  else
    ss << 
      setw(8) << Player::playerName() <<
      setw(4) << fullCount[rank] <<
      setw(6) << names[rank];

  return ss.str();
}


wstring Player::wstr() const
{
  if (len == 0)
    return L"-";

  string s = "";
  for (unsigned rank = maxRank; rank > 0; rank--)
    s += names[rank];

  wstring_convert<codecvt_utf8_utf16<wchar_t>> conv;
  return conv.from_bytes(s);
}

