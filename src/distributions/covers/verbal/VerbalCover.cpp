/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <cassert>

#include "VerbalCover.h"

#include "Expand.h"
#include "VerbalSide.h"

#include "../product/Profile.h"

#include "../../../languages/connections/cover/sentences.h"
#include "../../../languages/connections/cover/phrases.h"

#include "../../../utils/table.h"


extern Expand expand;


VerbalCover::VerbalCover()
{
  sentence = SENTENCE_SIZE;
  completions.resize(1);
  phrases.clear();
  length.reset();
}


void VerbalCover::setLength(const Term& lengthIn)
{
  length = lengthIn;
}


void VerbalCover::setLength(
  const unsigned char lower,
  const unsigned char upper,
  const unsigned char maximum)
{
  length.set(maximum, lower, upper);
}


/**********************************************************************/
/*                                                                    */
/*                          Help methods                              */
/*                                                                    */
/**********************************************************************/

void VerbalCover::getLengthEqualData(
  const unsigned char oppsLength,
  const VerbalSide& vside,
  const bool abstractableFlag)
{
  // Here lower and upper are identical.
  const unsigned char value = (vside.side == OPP_WEST ?
    length.lower() : oppsLength - length.lower());

  phrases.resize(2);

  if (value == 0)
  {
    phrases[0].setPhrase(vside.player());
    phrases[1].setPhrase(LENGTH_VERB_VOID);
  }
  else if (value == 1 ||
    (value == 2 && (! abstractableFlag || oppsLength > 4)) ||
    (value == 3 && (! abstractableFlag || oppsLength > 6)))
  {
    phrases[0].setPhrase(vside.player());
    phrases[1].setPhrase(LENGTH_VERB_XTON);
    phrases[1].setValues(value);
  }
  else if (! abstractableFlag)
  {
    assert(false);
  }
  else if (value + value == oppsLength)
  {
    phrases[0].setPhrase(PLAYER_SUIT);
    phrases[1].setPhrase(LENGTH_VERB_EVENLY);
  }
  else
  {
    phrases[0].setPhrase(PLAYER_SUIT);
    phrases[1].setPhrase(LENGTH_VERB_SPLIT);
    phrases[1].setValues(length.lower(), oppsLength - length.lower());
  }
}


void VerbalCover::getLengthInsideData(
  const unsigned char oppsLength,
  const VerbalSide& vside,
  const bool abstractableFlag)
{
  unsigned char vLower, vUpper;
  length.range(oppsLength, vside.side, vLower, vUpper);

  phrases.resize(2);

  if (vLower == 0)
  {
    if (vUpper+1 == oppsLength && abstractableFlag)
    {
      phrases[0].setPhrase(vside.otherPlayer());
      phrases[1].setPhrase(LENGTH_VERB_NOT_VOID);
    }
    else if (vUpper <= 3)
    {
      phrases[0].setPhrase(vside.player());
      phrases[1].setPhrase(LENGTH_VERB_XTON_ATMOST);
      phrases[1].setValues(vUpper);
    }
    else
    {
      phrases[0].setPhrase(vside.player());
      phrases[1].setPhrase(LENGTH_VERB_ATMOST);
      phrases[1].setValues(vUpper);
    }
  }
  else if (! abstractableFlag)
  {
    if (vLower == 1 && vUpper == 2)
    {
      phrases[0].setPhrase(vside.player());
      phrases[1].setPhrase(LENGTH_VERB_12);
    }
    else
    {
      phrases[0].setPhrase(vside.player());
      phrases[1].setPhrase(
        vLower+1 == vUpper ? LENGTH_VERB_RANGE : LENGTH_VERB_BETWEEN);
      phrases[1].setValues(vLower, vUpper);
    }
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
    phrases[0].setPhrase(PLAYER_NEITHER);
    phrases[1].setPhrase(LENGTH_VERB_VOID);
  }
  else if (vLower+1 == vUpper && vLower + vUpper == oppsLength)
  {
    phrases[0].setPhrase(PLAYER_SUIT);
    phrases[1].setPhrase(LENGTH_VERB_ODD_EVENLY);
  }
  else if (vLower + vUpper == oppsLength)
  {
    phrases[0].setPhrase(PLAYER_EACH);
    phrases[1].setPhrase(
      vLower+1 == vUpper ? LENGTH_VERB_RANGE : LENGTH_VERB_BETWEEN);
    phrases[1].setValues(vLower, vUpper);
  }
  else if (vLower == 1 && vUpper == 2)
  {
    phrases[0].setPhrase(vside.player());
    phrases[1].setPhrase(LENGTH_VERB_12);
  }
  else
  {
    phrases[0].setPhrase(vside.player());
    phrases[1].setPhrase(
      vLower+1 == vUpper ? LENGTH_VERB_RANGE : LENGTH_VERB_BETWEEN);
    phrases[1].setValues(vLower, vUpper);
  }
}


void VerbalCover::getLengthData(
  const unsigned char oppsLength,
  const VerbalSide& vside,
  const bool abstractableFlag)
{
  // If abstractableFlag is set, we must state the sentence from
  // the intended side (and not e.g. "The suit splits 2=2" instead
  // of "West has a doubleton").

  if (length.isEqual())
    VerbalCover::getLengthEqualData(oppsLength, vside, abstractableFlag);
  else
    VerbalCover::getLengthInsideData(oppsLength, vside, abstractableFlag);
}


/**********************************************************************/
/*                                                                    */
/*                          Fill methods                              */
/*                                                                    */
/**********************************************************************/

void VerbalCover::fillLengthOnly(
  const Term& lengthIn,
  const unsigned char oppsLength,
  const bool symmFlag)
{
  sentence = SENTENCE_LENGTH_ONLY;
  VerbalCover::setLength(lengthIn);

  const Opponent side = (symmFlag ? OPP_WEST : length.shorter(oppsLength));

  const VerbalSide vside = {side, symmFlag};
  VerbalCover::getLengthData(oppsLength, vside, true);
}


void VerbalCover::fillOnetopOnly(
  const Term& top,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  sentence = SENTENCE_ONETOP_ONLY;

  unsigned char vLower, vUpper;
  top.range(oppsSize, vside.side, vLower, vUpper);

  // Here lower and upper are different.
  phrases.resize(2);

  if (vLower + vUpper == oppsSize)
    phrases[0].setPhrase(PLAYER_EACH);
  else
    phrases[0].setPhrase(vside.player());

  if (vLower == 0)
  {
    phrases[1].setPhrase(TOPS_ATMOST);
    phrases[1].setValues(vUpper, onetopIndex);
  }
  else if (vUpper == oppsSize)
  {
    phrases[1].setPhrase(TOPS_ATLEAST);
    phrases[1].setValues(vLower, onetopIndex);
  }
  else if (vLower+1 == vUpper)
  {
    phrases[1].setPhrase(TOPS_ADJACENT);
    phrases[1].setValues(vLower, vUpper, onetopIndex);
  }
  else
  {
    phrases[1].setPhrase(TOPS_RANGE);
    phrases[1].setValues(vLower, vUpper, onetopIndex);
  }
}


void VerbalCover::fillLengthOrdinal(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  Phrase& phrase)
{
  unsigned char vLower, vUpper;
  length.range(oppsLength, simplestOpponent, vLower, vUpper);

  if (vLower == vUpper)
  {
    phrase.setPhrase(LENGTH_ORDINAL_EXACT);
    phrase.setValues(vLower);
  }
  else if (vLower == 0)
  {
    phrase.setPhrase(LENGTH_ORDINAL_ATMOST);
    phrase.setValues(vUpper);
  }
  else if (vLower + 1 == vUpper)
  {
    phrase.setPhrase(LENGTH_ORDINAL_ADJACENT);
    phrase.setValues(vLower, vUpper);
  }
  else
  {
    phrase.setPhrase(LENGTH_ORDINAL_RANGE);
    phrase.setValues(vLower, vUpper);
  }
}


void VerbalCover::fillOnetopLength(
  const Term& lengthIn,
  const Term& top,
  const Profile& sumProfile,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  VerbalCover::setLength(lengthIn);

  // Fill templateFills positions 0 and 1.
  VerbalCover::fillOnetopOnly(
    top, sumProfile[onetopIndex], onetopIndex, vside);

  sentence = SENTENCE_TOPS_LENGTH;

  phrases.resize(3);

  VerbalCover::fillLengthOrdinal(
    sumProfile.length(), vside.side, phrases[2]);
}


void VerbalCover::fillTopsActual(
  const Opponent side,
  Phrase& phrase)
{
  const Completion& completion = completions.front();

  if (! completion.expandable(side))
  {
    phrase.setPhrase(TOPS_ACTUAL);
    phrase.setSide(side);
    phrase.setBools(false);
  }
  else if (! completion.fullRanked(side))
  {
    phrase.setPhrase(TOPS_SOME_ACTUAL);
    phrase.setValues(
      completion.getTopsUsed(side),
      completion.getLowestRankActive(side));
  }
  else
  {
    phrase.setPhrase(TOPS_FULL_ACTUAL);
    phrase.setValues(completion.getLowestRankActive(side));
  }
}


void VerbalCover::fillOnesided(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  // length is already set.
  sentence = SENTENCE_TOPS_LENGTH;

  phrases.resize(3);

  phrases[0].setPhrase(vside.player());

  VerbalCover::fillTopsActual(vside.side, phrases[1]);

  VerbalCover::fillLengthOrdinal(
    sumProfile.length(), vside.side, phrases[2]);
}


void VerbalCover::fillTopsBothLength(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  // length is already set.
  sentence = SENTENCE_TOPS_BOTH_LENGTH;

  phrases.resize(3);

  const Completion& completion = completions.front();
  const bool bothExpandableFlag =
    completion.expandable(OPP_WEST) &&
    completion.expandable(OPP_EAST);

  phrases[0].setPhrase(BOTH_ONE_PLAYER_LENGTH);
  phrases[0].setBools(bothExpandableFlag);

  phrases[1].setPhrase(BOTH_ONE_PLAYER_LENGTH);
  phrases[1].setBools(bothExpandableFlag);

  const Opponent sideOther = (vside.side == OPP_WEST ? OPP_EAST : OPP_WEST);

  if (bothExpandableFlag)
  {
    phrases[0].setValues(
      completion.getLowestRankActive(vside.side),
      length.lower(),
      length.upper());

    phrases[1].setValues(
      completion.getLowestRankActive(sideOther),
      sumProfile.length() - length.upper(),
      sumProfile.length() - length.lower());
  }
  else
  {
    phrases[0].setSide(vside.side);
    phrases[0].setValues(length.lower(), length.upper());

    phrases[1].setSide(sideOther);
    phrases[1].setValues(
      sumProfile.length() - length.upper(), 
      sumProfile.length() - length.lower());
  }

  if (vside.symmFlag)
    phrases[2].setPhrase(EITHER_WAY);
  else
    phrases[2].setPhrase(ONE_WAY);
}


void VerbalCover::fillTopsBoth(const VerbalSide& vside)
{
  // This is an expansion with quite a lot of parameters in
  // a single Phrase.
  sentence = SENTENCE_TOPS_BOTH;

  phrases.resize(1);

  const Completion& completion = completions.front();
  const bool bothExpandableFlag =
    completion.expandable(OPP_WEST) &&
    completion.expandable(OPP_EAST);

  if (bothExpandableFlag)
  {
    // Needed to expand the two sides.
    Opponent side1, side2;
    if (vside.symmFlag)
    {
      side1 = vside.side;
      side2 = (side1 == OPP_WEST ? OPP_EAST : OPP_WEST);
    }
    else
    {
      side1 = OPP_WEST;
      side2 = OPP_EAST;
    }

    phrases[0].setValues(
      completion.getLowestRankActive(side1),
      completion.getLowestRankActive(side2));
  }
  else if (vside.symmFlag)
  {
    // Needed to know which side to state first.
    phrases[0].setSide(vside.side);
  }

  phrases[0].setBools(vside.symmFlag, bothExpandableFlag);

  if (vside.symmFlag)
    phrases[0].setPhrase(BOTH_EITHER_PLAYER);
  else
    phrases[0].setPhrase(BOTH_ONE_PLAYER);
}


void VerbalCover::fillTopsAndXes(const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_AND_XES;

  phrases.resize(3);
  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(TOPS_ACTUAL);
  phrases[1].setSide(vside.side);
  phrases[1].setBools(false);

  phrases[2].setPhrase(BOTTOMS_NORMAL);
  phrases[2].setSide(vside.side);
}


void VerbalCover::fillTopsAndLower(
  const VerbalSide& vside,
  const unsigned char numOptions)
{
  sentence = SENTENCE_TOPS_AND_LOWER;

  phrases.resize(4);

  phrases[0].setPhrase(vside.player());

  VerbalCover::fillTopsActual(vside.side, phrases[1]);

  const auto& completion = completions.front();
  const unsigned char freeLower = completion.getFreeLower(vside.side);
  const unsigned char freeUpper = completion.getFreeUpper(vside.side);

  if (freeLower == freeUpper)
  {
    phrases[2].setPhrase(COUNT_EXACT);
    phrases[2].setValues(freeLower);
  }
  else if (freeLower == 0)
  {
    phrases[2].setPhrase(COUNT_ATMOST);
    phrases[2].setValues(freeUpper);
  }
  else
  {
    phrases[2].setPhrase(COUNT_RANGE);
    phrases[2].setValues(freeLower, freeUpper);
  }

  if (completion.lowestRankIsUsed(vside.side))
  {
    phrases[3].setPhrase(TOPS_LOWER);
    phrases[3].setValues(freeUpper);
  }
  else
  {
    phrases[3].setPhrase(TOPS_BELOW);
    phrases[3].setValues(freeUpper, numOptions);
  }
}


void VerbalCover::fillBelow(
  const unsigned char numBottoms,
  const unsigned char rankNo,
  const VerbalSide& vside)
{
  sentence = SENTENCE_ONLY_BELOW;

  const auto& completion = completions.front();
  const unsigned char freeLower = completion.getFreeLower(OPP_WEST);
  const unsigned char freeUpper = completion.getFreeUpper(OPP_WEST);

  // Make a synthetic length of small cards.
  VerbalCover::setLength(freeLower, freeUpper, numBottoms);

  // Set templateFills numbers 0 and 1 (and the size of 2).
  VerbalCover::getLengthData(numBottoms, vside, false);
  
  phrases.resize(4);

  if (completion.getFreeUpper(vside.side) == 1)
    phrases[2].setPhrase(BELOW_NORMAL);
  else
    phrases[2].setPhrase(BELOW_COMPLETELY);

  phrases[3].setPhrase(TOPS_RANKS);
  phrases[3].setValues(rankNo);
}



void VerbalCover::fillHonorsEqual(
  const unsigned char numHonors,
  Phrase& phrase) const
{
  if (numHonors == 1)
  {
    phrase.setPhrase(HONORS_ONE);
  }
  else
  {
    phrase.setPhrase(HONORS_MULTIPLE);
    phrase.setValues(numHonors);
  }
}


void VerbalCover::fillSingular(
  const unsigned char lenCompletion,
  const VerbalSide& vside)
{
  phrases.resize(3);

  phrases[0].setPhrase(vside.player());

  const Completion& completion = completions.front();
  const Opponent side = vside.side;

  if (! completion.expandable(side) || completion.fullRanked(side))
  {
    sentence = SENTENCE_TOPS_LENGTH;

    VerbalCover::fillTopsActual(side, phrases[1]);

    phrases[2].setPhrase(LENGTH_ORDINAL_EXACT);
    phrases[2].setValues(lenCompletion);
  }
  else if (completion.highRanked(side))
  {
    sentence = SENTENCE_HONORS_ORDINAL;

    VerbalCover::fillHonorsEqual(
      completion.getTopsUsed(side), phrases[1]);

    phrases[2].setPhrase(LENGTH_ORDINAL_EXACT);
    phrases[2].setValues(lenCompletion);
  }
  else
  {
    sentence = SENTENCE_SINGULAR_EXACT;

    phrases[1].setPhrase(LENGTH_ORDINAL_EXACT);
    phrases[1].setValues(lenCompletion);

    phrases[2].setPhrase(TOPS_FULL_ACTUAL);
    phrases[2].setValues(completion.getLowestRankActive(side));
  }
}


void VerbalCover::fillCompletion(const VerbalSide& vside)
{
  phrases.resize(2);

  phrases[0].setPhrase(vside.player());

  if (! completions.front().expandable(vside.side))
  {
    sentence = SENTENCE_LIST;

    phrases[1].setPhrase(LIST_HOLDING_EXACT);
    phrases[1].setSide(vside.side);
    phrases[1].setBools(false);
  }
  else
  {
    // A bit of a kludge to allow the expansion.
    sentence = SENTENCE_ONETOP_ONLY;

    phrases[0].setPhrase(vside.player());
    VerbalCover::fillTopsActual(vside.side, phrases[1]);
  }
}


void VerbalCover::fillCompletionWithLows(const VerbalSide& vside)
{
  sentence = SENTENCE_LIST;

  phrases.resize(2);

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(LIST_HOLDING_WITH_LOWS);
  phrases[1].setSide(vside.side);
  phrases[1].setBools(false);
}


void VerbalCover::fillList(const VerbalSide& vside)
{
  sentence = SENTENCE_LIST;

  phrases.resize(completions.size() + 1);
  phrases[0].setPhrase(vside.player());

  for (size_t i = 1; i < phrases.size(); i++)
  {
    phrases[i].setPhrase(LIST_HOLDING_EXACT);
    phrases[i].setSide(vside.side);
    phrases[i].setBools(true);
  }
}

/**********************************************************************/
/*                                                                    */
/*                       Completion methods                           */
/*                                                                    */
/**********************************************************************/

void VerbalCover::push_back(const Completion& completionIn)
{
  completions.push_back(completionIn);
}


Completion& VerbalCover::getCompletion()
{
  return completions.front();
}


list<Completion>& VerbalCover::getCompletions()
{
  return completions;
}


/**********************************************************************/
/*                                                                    */
/*                         String method                              */
/*                                                                    */
/**********************************************************************/

string VerbalCover::str(const RanksNames& ranksNames) const
{
  return expand.get(sentence, ranksNames, completions, phrases);
}

