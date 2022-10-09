/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <cassert>

#include "VerbalCover.h"

#include "VerbalTemplates.h"
#include "VerbalDimensions.h"
#include "VerbalSide.h"

#include "../product/Profile.h"

#include "../../../utils/table.h"


extern VerbalTemplates verbalTemplates;


VerbalCover::VerbalCover()
{
  sentence = SENTENCE_SIZE;
  completions.resize(1);
  slots.clear();
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

  slots.resize(2);

  if (value == 0)
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_VOID);
  }
  else if (value == 1 ||
    (value == 2 && (! abstractableFlag || oppsLength > 4)) ||
    (value == 3 && (! abstractableFlag || oppsLength > 6)))
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_XTON);
    slots[1].setValues(value);
  }
  else if (! abstractableFlag)
  {
    assert(false);
  }
  else if (value + value == oppsLength)
  {
    slots[0].setPhrase(PLAYER_SUIT);
    slots[1].setPhrase(LENGTH_VERB_EVENLY);
  }
  else
  {
    slots[0].setPhrase(PLAYER_SUIT);
    slots[1].setPhrase(LENGTH_VERB_SPLIT);
    slots[1].setValues(length.lower(), oppsLength - length.lower());
  }
}


void VerbalCover::getLengthInsideData(
  const unsigned char oppsLength,
  const VerbalSide& vside,
  const bool abstractableFlag)
{
  unsigned char vLower, vUpper;
  length.range(oppsLength, vside.side, vLower, vUpper);

  slots.resize(2);

  if (vLower == 0)
  {
    if (vUpper <= 3)
    {
      slots[0].setPhrase(vside.player());
      slots[1].setPhrase(LENGTH_VERB_XTON_ATMOST);
      slots[1].setValues(vUpper);
    }
    else
    {
      slots[0].setPhrase(vside.player());
      slots[1].setPhrase(LENGTH_VERB_ATMOST);
      slots[1].setValues(vUpper);
    }
  }
  else if (! abstractableFlag)
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_RANGE);
    slots[1].setValues(vLower, vUpper);
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
    slots[0].setPhrase(PLAYER_NEITHER);
    slots[1].setPhrase(LENGTH_VERB_VOID);
  }
  else if (vLower+1 == vUpper && vLower + vUpper == oppsLength)
  {
    slots[0].setPhrase(PLAYER_SUIT);
    slots[1].setPhrase(LENGTH_VERB_ODD_EVENLY);
  }
  else if (vLower + vUpper == oppsLength)
  {
    slots[0].setPhrase(PLAYER_EACH);
    slots[1].setPhrase(LENGTH_VERB_RANGE);
    slots[1].setValues(vLower, vUpper);
  }
  else if (vLower == 1 && vUpper == 2)
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_12);
  }
  else
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_RANGE);
    slots[1].setValues(vLower, vUpper);
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
  slots.resize(2);

  if (vLower + vUpper == oppsSize)
    slots[0].setPhrase(PLAYER_EACH);
  else
    slots[0].setPhrase(vside.player());

  if (vLower == 0)
  {
    slots[1].setPhrase(TOPS_ATMOST);
    slots[1].setValues(vUpper, onetopIndex);
  }
  else if (vUpper == oppsSize)
  {
    slots[1].setPhrase(TOPS_ATLEAST);
    slots[1].setValues(vLower, onetopIndex);
  }
  else
  {
    slots[1].setPhrase(TOPS_RANGE);
    slots[1].setValues(vLower, vUpper, onetopIndex);
  }
}


void VerbalCover::fillLengthOrdinal(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  Slot& slot)
{
  unsigned char vLower, vUpper;
  length.range(oppsLength, simplestOpponent, vLower, vUpper);

  if (vLower == vUpper)
  {
    slot.setPhrase(LENGTH_ORDINAL_EXACT);
    slot.setValues(vLower);
  }
  else if (vLower == 0)
  {
    slot.setPhrase(LENGTH_ORDINAL_ATMOST);
    slot.setValues(vUpper);
  }
  else if (vLower + 1 == vUpper)
  {
    slot.setPhrase(LENGTH_ORDINAL_ADJACENT);
    slot.setValues(vLower, vUpper);
  }
  else
  {
    slot.setPhrase(LENGTH_ORDINAL_RANGE);
    slot.setValues(vLower, vUpper);
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

  slots.resize(3);

  VerbalCover::fillLengthOrdinal(
    sumProfile.length(), vside.side, slots[2]);
}


void VerbalCover::fillOnesided(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  // length is already set.
  sentence = SENTENCE_TOPS_LENGTH;

  slots.resize(3);

  slots[0].setPhrase(vside.player());

  slots[1].setPhrase(TOPS_ACTUAL);
  slots[1].setSide(vside.side);
  slots[1].setBools(true);

  VerbalCover::fillLengthOrdinal(
    sumProfile.length(), vside.side, slots[2]);
}


void VerbalCover::fillTwosided(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  // length is already set.
  VerbalCover::fillOnesided(sumProfile, vside);

  sentence = SENTENCE_TOPS_LENGTH_WITHOUT;
  slots.resize(4);

  slots[3].setPhrase(TOPS_ACTUAL);
  slots[3].setSide(vside.side == OPP_WEST ? OPP_EAST : OPP_WEST);
  slots[3].setBools(true);
}


void VerbalCover::fillTopsExcluding(const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_EXCLUDING;

  slots.resize(4);

  slots[0].setPhrase(vside.player());

  slots[1].setPhrase(TOPS_ACTUAL);
  slots[1].setSide(vside.side);
  slots[1].setBools(true);

  const Opponent sideOther = (vside.side == OPP_WEST ? OPP_EAST : OPP_WEST);
  const unsigned topsFull = completions.front().getTopsFull(sideOther);

  if (topsFull <= 1)
    slots[2].setPhrase(EXCLUDING_NOT);
  else if (topsFull == 2)
    slots[2].setPhrase(EXCLUDING_NEITHER);
  else
    slots[2].setPhrase(EXCLUDING_NONE);

  slots[3].setPhrase(TOPS_ACTUAL);
  slots[3].setSide(sideOther);
  slots[3].setBools(true);
}


void VerbalCover::fillTopsAndXes(const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_AND_XES;

  slots.resize(3);
  slots[0].setPhrase(vside.player());

  slots[1].setPhrase(TOPS_ACTUAL);
  slots[1].setSide(vside.side);
  slots[1].setBools(false);

  slots[2].setPhrase(BOTTOMS_NORMAL);
  slots[2].setSide(vside.side);
}


void VerbalCover::fillTopsAndLower(
  const VerbalSide& vside,
  const unsigned char numOptions)
{
  sentence = SENTENCE_TOPS_AND_LOWER;

  slots.resize(4);

  slots[0].setPhrase(vside.player());

  slots[1].setPhrase(TOPS_ACTUAL);
  slots[1].setSide(vside.side);
  slots[1].setBools(false);

  const auto& completion = completions.front();
  const unsigned char freeLower = completion.getFreeLower(vside.side);
  const unsigned char freeUpper = completion.getFreeUpper(vside.side);

  if (freeLower == freeUpper)
  {
    slots[2].setPhrase(COUNT_EXACT);
    slots[2].setValues(freeLower);
  }
  else if (freeLower == 0)
  {
    slots[2].setPhrase(COUNT_ATMOST);
    slots[2].setValues(freeUpper);
  }
  else
  {
    slots[2].setPhrase(COUNT_RANGE);
    slots[2].setValues(freeLower, freeUpper);
  }

  if (completion.lowestRankIsUsed(vside.side))
  {
    slots[3].setPhrase(TOPS_LOWER);
    slots[3].setValues(freeUpper);
  }
  else
  {
    slots[3].setPhrase(TOPS_BELOW);
    slots[3].setValues(freeUpper, numOptions);
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
  
  slots.resize(4);

  if (completion.getFreeUpper(vside.side) == 1)
    slots[2].setPhrase(BELOW_NORMAL);
  else
    slots[2].setPhrase(BELOW_COMPLETELY);

  slots[3].setPhrase(TOPS_RANKS);
  slots[3].setValues(rankNo);
}



void VerbalCover::fillSingular(
  const unsigned char lenCompletion,
  const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_LENGTH;

  slots.resize(3);

  slots[0].setPhrase(vside.player());

  slots[1].setPhrase(TOPS_ACTUAL);
  slots[1].setSide(vside.side);
  slots[1].setBools(true);

  slots[2].setPhrase(LENGTH_ORDINAL_EXACT);
  slots[2].setValues(lenCompletion);
}


void VerbalCover::fillCompletion(const VerbalSide& vside)
{
  sentence = SENTENCE_LIST;

  slots.resize(2);

  slots[0].setPhrase(vside.player());

  slots[1].setPhrase(LIST_HOLDING_EXACT);
  slots[1].setSide(vside.side);
  slots[1].setBools(true);

}


void VerbalCover::fillCompletionWithLows(const VerbalSide& vside)
{
  sentence = SENTENCE_LIST;

  slots.resize(2);

  slots[0].setPhrase(vside.player());

  slots[1].setPhrase(LIST_HOLDING_WITH_LOWS);
  slots[1].setSide(vside.side);
  slots[1].setBools(false);
}


void VerbalCover::fillList(const VerbalSide& vside)
{
  sentence = SENTENCE_LIST;

  slots.resize(completions.size() + 1);
  slots[0].setPhrase(vside.player());

  for (size_t i = 1; i < slots.size(); i++)
  {
    slots[i].setPhrase(LIST_HOLDING_EXACT);
    slots[i].setSide(vside.side);
    slots[i].setBools(true, true);
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
  return verbalTemplates.get(sentence, ranksNames, completions, slots);
}

