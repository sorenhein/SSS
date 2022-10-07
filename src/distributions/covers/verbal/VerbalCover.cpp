/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cassert>

#include "VerbalCover.h"
#include "VerbalBlank.h"

#include "../product/Profile.h"
#include "../product/VerbalData.h"

// TODO Need whole file or just TemplateData?
#include "VerbalTemplates.h"
#include "VerbalDimensions.h"
#include "./Completion.h"

#include "../../../ranks/RanksNames.h"

#include "../../../utils/table.h"

extern VerbalTemplates verbalTemplates;


VerbalCover::VerbalCover()
{
  sentence = SENTENCE_SIZE;
  lengthFlag = false;
  lengthLower = 0;
  lengthUpper = 0;
  lengthOper = COVER_EQUAL;

  completions.resize(1);
  slots.clear();
}


void VerbalCover::push_back(const Completion& completionIn)
{
  completions.push_back(completionIn);
}


void VerbalCover::setSentence(const Sentence sentenceIn)
{
  sentence = sentenceIn;
}


void VerbalCover::setLength(const Term& length)
{
  lengthFlag = true;
  lengthLower = length.lower();
  lengthUpper = length.upper();
  lengthOper = length.getOperator();
}


void VerbalCover::setLength(
  const unsigned char lower,
  const unsigned char upper,
  const unsigned char maximum)
{
  lengthFlag = true;
  lengthLower = lower;
  lengthUpper = upper;

  if (lower == upper)
    lengthOper = COVER_EQUAL;
  else if (lower == 0)
    lengthOper = COVER_LESS_EQUAL;
  else if (upper == maximum)
    lengthOper = COVER_GREATER_EQUAL;
  else
    lengthOper = COVER_INSIDE_RANGE;
}


void VerbalCover::fillLengthOnly(
  const Term& length,
  const unsigned char oppsLength,
  const bool symmFlag)
{
  VerbalCover::setLength(length);
  sentence = SENTENCE_LENGTH_ONLY;

  const bool westFlag = (completions.front().length(OPP_WEST) > 0);
  const bool eastFlag = (completions.front().length(OPP_EAST) > 0);

  Opponent simplestOpponent;
  if (symmFlag)
    simplestOpponent = OPP_WEST;
  else if (westFlag == eastFlag)
    simplestOpponent = VerbalCover::simplestOpponent(oppsLength);
  else if (westFlag)
    simplestOpponent = OPP_WEST;
  else
    simplestOpponent = OPP_EAST;

  const VerbalSide vside = {simplestOpponent, symmFlag};
  VerbalCover::getLengthData(oppsLength, vside, true);
}


void VerbalCover::fillOnetopOnly(
  const Term& top,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  sentence = SENTENCE_ONETOP;

  if (vside.side == OPP_WEST || vside.side == OPP_EITHER)
  {
    VerbalCover::getOnetopData(
      top.lower(),
      top.upper(),
      oppsSize,
      onetopIndex,
      vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST);
  }
  else
  {
    VerbalCover::getOnetopData(
      top.upper() == 0xf ? 0 : oppsSize - top.upper(),
      oppsSize - top.lower(),
      oppsSize,
      onetopIndex,
      vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST);
  }
}


void VerbalCover::fillOnetopLength(
  const Term& length,
  const Term& top,
  const Profile& sumProfile,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  VerbalCover::setLength(length);

  // Fill templateFills positions 0 and 1.
  VerbalCover::fillOnetopOnly(
    top,
    sumProfile[onetopIndex],
    onetopIndex,
    vside);

  // Fill templateFills position 2 (not pretty -- too implicit?).
  VerbalCover::fillLengthAdjElement(sumProfile.length(), vside.side);

  sentence = SENTENCE_TOPS_LENGTH;
}


void VerbalCover::fillTopsExcluding(const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_EXCLUDING;

  // BlankPlayerCap bside = vside.blank();

  slots.resize(4);

  slots[0].setPhrase(vside.player());

  slots[1].setPhrase(TOPS_ACTUAL);
  slots[1].setSide(vside.side);
  slots[1].setBools(true, true);

  const unsigned topsFull = completions.front().getTopsFull(
    vside.side == OPP_WEST ? OPP_EAST : OPP_WEST);

  if (topsFull <= 1)
    slots[2].setPhrase(EXCLUDING_NOT);
  else if (topsFull == 2)
    slots[2].setPhrase(EXCLUDING_NEITHER);
  else
    slots[2].setPhrase(EXCLUDING_NONE);

  slots[3].setPhrase(TOPS_ACTUAL);
  slots[3].setSide(vside.side == OPP_WEST ? OPP_EAST : OPP_WEST);
  slots[3].setBools(true, true);
}


Completion& VerbalCover::getCompletion()
{
  return completions.front();
}


list<Completion>& VerbalCover::getCompletions()
{
  return completions;
}


Opponent VerbalCover::simplestOpponent(const unsigned char oppsLength) const
{
  if (! lengthFlag)
    return OPP_EITHER;

  // Choose the shorter side, as this tends to be more intuitive.
  const unsigned char lsum = lengthLower + lengthUpper;

  if (lsum > oppsLength)
    return OPP_EAST;
  else if (lsum < oppsLength)
    return OPP_WEST;
  else
    return OPP_EITHER;
}


void VerbalCover::getLengthEqualData(
  const unsigned char oppsLength,
  const VerbalSide& vside,
  const bool abstractableFlag)
{
  // Here lower and upper are identical.
  // BlankPlayerCap bside;
  unsigned char value;

  if (vside.side == OPP_WEST)
  {
    // bside = (vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST);
    value = lengthLower;
  }
  else
  {
    // bside = (vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST);
    value = oppsLength - lengthLower;
  }

  slots.resize(2);

  if (value == 0)
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_VOID);
  }
  else if (value == 1)
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_XTON);
    slots[1].setValues(value);
  }
  else if (value == 2 && (! abstractableFlag || oppsLength > 4))
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_XTON);
    slots[1].setValues(value);
  }
  else if (value == 3 && (! abstractableFlag || oppsLength > 6))
  {
    slots[0].setPhrase(vside.player());
    slots[1].setPhrase(LENGTH_VERB_XTON);
    slots[1].setValues(value);
  }
  else if (! abstractableFlag)
  {
// cout << "bside " << bside << endl;
cout << "value " << +value << endl;
    assert(false);
  }
  else if (value + value == oppsLength)
  {
    slots[0].setSemantics(
      PHRASE_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT, SLOT_NONE);
    slots[1].setPhrase(LENGTH_VERB_EVENLY);
  }
  else
  {
    slots[0].setSemantics(
      PHRASE_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT, SLOT_NONE);
    slots[1].setPhrase(LENGTH_VERB_SPLIT);
    slots[1].setValues(lengthLower, oppsLength - lengthLower);
  }
}


void VerbalCover::getLengthInsideData(
  const unsigned char oppsLength,
  const VerbalSide& vside,
  const bool abstractableFlag)
{
  BlankPlayerCap bside;
  unsigned char vLower, vUpper;

  if (vside.side == OPP_WEST)
  {
    bside = (vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST);
    vLower = lengthLower;
    vUpper = (lengthOper == COVER_GREATER_EQUAL ?
      oppsLength : lengthUpper);
  }
  else
  {
    bside = (vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST);
    vLower = (lengthOper == COVER_GREATER_EQUAL ?
      0 : oppsLength - lengthUpper);
    vUpper = oppsLength - lengthLower;
  }

  slots.resize(2);

  if (vLower == 0)
  {
    if (vUpper == 1)
    {
      slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);
      slots[1].setPhrase(LENGTH_VERB_XTON_ATMOST);
      slots[1].setValues(vUpper);
    }
    else if (vUpper == 2)
    {
      slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);
      slots[1].setPhrase(LENGTH_VERB_XTON_ATMOST);
      slots[1].setValues(vUpper);
    }
    else if (vUpper == 3)
    {
      slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);
      slots[1].setPhrase(LENGTH_VERB_XTON_ATMOST);
      slots[1].setValues(vUpper);
    }
    else
    {
      slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);
      slots[1].setPhrase(LENGTH_VERB_ATMOST);
      slots[1].setValues(vUpper);
    }
  }
  else if (! abstractableFlag)
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);
    slots[1].setPhrase(LENGTH_VERB_RANGE);
    slots[1].setValues(vLower, vUpper);
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, BLANK_PLAYER_CAP_NEITHER, 
      SLOT_NONE);
    slots[1].setPhrase(LENGTH_VERB_VOID);
  }
  else if (vLower+1 == vUpper)
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT, 
      SLOT_NONE);
    slots[1].setPhrase(LENGTH_VERB_ODD_EVENLY);
  }
  else if (vLower + vUpper == oppsLength)
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, BLANK_PLAYER_CAP_EACH, 
      SLOT_NONE);
    slots[1].setPhrase(LENGTH_VERB_RANGE);
    slots[1].setValues(vLower, vUpper);
  }
  else
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);
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

  if (lengthOper == COVER_EQUAL)
  {
    VerbalCover::getLengthEqualData(
      oppsLength, vside, abstractableFlag);
  }
  else if (lengthOper == COVER_INSIDE_RANGE ||
           lengthOper == COVER_LESS_EQUAL ||
           lengthOper == COVER_GREATER_EQUAL)
  {
    VerbalCover::getLengthInsideData(oppsLength, vside,
      abstractableFlag);
  }
  else
    assert(false);
}


void VerbalCover::fillLengthAdjElement(
  const unsigned char oppsLength,
  const Opponent simplestOpponent)
{
  slots.resize(3);
  Slot& selement = slots[2];

  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    vLower = lengthLower;
    vUpper = lengthUpper;
  }
  else if (lengthUpper == 0xf)
  {
    vLower = 0;
    vUpper = oppsLength - lengthLower;
  }
  else
  {
    vLower = oppsLength - lengthUpper;
    vUpper = oppsLength - lengthLower;
  }

  if (vLower == vUpper)
  {
    selement.setPhrase(LENGTH_ORDINAL_EXACT);
    selement.setValues(vLower);
  }
  else if (vLower == 0)
  {
    selement.setPhrase(LENGTH_ORDINAL_ATMOST);
    selement.setValues(vUpper);
  }
  else if (vLower == 2 && vUpper == 3)
  {
    selement.setPhrase(LENGTH_ORDINAL_23);
  }
  else
  {
cout << "range " << +lengthLower << " to " << +lengthUpper << endl;
cout << "here  " << +vLower << " to " << +vUpper << endl;
cout << "oppsLength " << +oppsLength << endl;
    assert(false);
  }
}


void VerbalCover::fillBelow(
  const unsigned char numBottoms,
  const unsigned char rankNo,
  const VerbalSide& vside)
{
  sentence = SENTENCE_ONLY_BELOW;

  const unsigned char freeLower = completions.front().getFreeLower(OPP_WEST);
  const unsigned char freeUpper = completions.front().getFreeUpper(OPP_WEST);

  // Make a synthetic length of small cards.
  VerbalCover::setLength(freeLower, freeUpper, numBottoms);

  // This sets templateFills numbers 0 and 1 (and the size of 2).
  VerbalCover::getLengthData(numBottoms, vside, false);
  
  slots.resize(4);

  if (completions.front().getFreeUpper(vside.side) == 1)
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

  const BlankPlayerCap bside = vside.blank();

  slots.resize(3);

  slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);

  slots[1].setPhrase(TOPS_ACTUAL);
  // TODO This is part of the reversal problem in Pverbal!
  // slots[1].setValues(static_cast<unsigned char>(vside.side));
  slots[1].setSide(OPP_WEST);
  slots[1].setBools(false, false);

  slots[2].setPhrase(LENGTH_ORDINAL_EXACT);
  slots[2].setValues(lenCompletion);
}


void VerbalCover::fillCompletion(const VerbalSide& vside)
{
  sentence = SENTENCE_LIST;
  const BlankPlayerCap bside = vside.blank();

  slots.resize(2);

  slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);

  slots[1].setPhrase(LIST_HOLDING_EXACT);
  slots[1].setSide(vside.side);
  slots[1].setBools(true, true);

}


void VerbalCover::fillCompletionWithLows(const VerbalSide& vside)
{
  sentence = SENTENCE_LIST;
  const BlankPlayerCap bside = vside.blank();

  slots.resize(2);

  slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);

  slots[1].setPhrase(LIST_HOLDING_WITH_LOWS);
  slots[1].setSide(vside.side);
  slots[1].setBools(false, true);
}


void VerbalCover::fillBottoms(const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_AND_XES;
  const BlankPlayerCap bside = vside.blank();

  slots.resize(3);
  slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);

  slots[1].setPhrase(TOPS_ACTUAL);
  slots[1].setSide(vside.side);
  slots[1].setBools(false, true);

  slots[2].setPhrase(BOTTOMS_NORMAL);
  slots[2].setSide(vside.side);
}


void VerbalCover::fillTopsAndLower(
  const VerbalSide& vside,
  [[maybe_unused]]const RanksNames& ranksNames,
  const unsigned char numOptions)
{
  sentence = SENTENCE_TOPS_AND_LOWER;
  const BlankPlayerCap bside = vside.blank();

  slots.resize(4);

  slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);

  slots[1].setPhrase(TOPS_ACTUAL);
  slots[1].setSide(vside.side);
  slots[1].setBools(true, true);

  const unsigned char freeLower = completions.front().getFreeLower(vside.side);
  const unsigned char freeUpper = completions.front().getFreeUpper(vside.side);

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

  if (completions.front().lowestRankIsUsed(vside.side))
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


void VerbalCover::fillList(const VerbalSide& vside)
{
  sentence = SENTENCE_LIST;
  const BlankPlayerCap bside = vside.blank();

  slots.resize(completions.size() + 1);
  slots[0].setSemantics(PHRASE_PLAYER_CAP, bside, SLOT_NONE);

  for (size_t i = 1; i < slots.size(); i++)
  {
    slots[i].setPhrase(LIST_HOLDING_EXACT);
    slots[i].setSide(vside.side);
    slots[i].setBools(false, false, true);
  }
}


void VerbalCover::setGeneral(
  const unsigned char oppsLength,
  const bool symmFlag,
  const RanksNames& ranksNames)
{
  // TODO This needs to conform to the rest.
  // For now we just store the string.
  sentence = SENTENCE_SIZE;

  assert(lengthFlag);

  string lstr = "", wstr = "", estr = "";

  const bool westFlag = (completions.front().length(OPP_WEST) > 0);
  const bool eastFlag = (completions.front().length(OPP_EAST) > 0);

  Opponent simplestOpponent;
  if (symmFlag)
    simplestOpponent = OPP_WEST;
  else if (westFlag == eastFlag)
    simplestOpponent = VerbalCover::simplestOpponent(oppsLength);
  else if (westFlag)
    simplestOpponent = OPP_WEST;
  else
    simplestOpponent = OPP_EAST;

  const VerbalSide vside = {simplestOpponent, symmFlag};

  VerbalCover::getLengthData(oppsLength, vside, true);
    
  lstr = verbalTemplates.get(SENTENCE_LENGTH_ONLY, ranksNames, 
    completions, slots);

  if (westFlag)
    wstr = completions.front().strSet(ranksNames, OPP_WEST, false, false);

  if (eastFlag)
    estr = completions.front().strSet(ranksNames, OPP_EAST, false, false);

  if (westFlag)
  {
    if (eastFlag)
    {
      if (symmFlag)
      {
        if (wstr == estr)
          strTMP = lstr + ", and West and East each have " + wstr;
        else
          strTMP = lstr + ", and " + wstr + " and " + estr +
            " are split";
      }
      else if (wstr == estr)
        strTMP = lstr + ", West and East each have " + wstr;
      else
        strTMP = lstr + ", West has " + wstr + " and East has " + estr;
    }
    else
    {
      if (symmFlag)
        strTMP = lstr + " with " + wstr;
      else
        strTMP = lstr + " and West has " + wstr;
    }
  }
  else if (eastFlag)
  {
    if (symmFlag)
      strTMP = lstr + " without " + estr;
    else
      strTMP = lstr + " and East has " + estr;
  }
  else
    // This done exclusively in the new way.
    strTMP = lstr;
}


void VerbalCover::getOnetopElement(
  const unsigned char oppsValue1,
  const unsigned char oppsValue2,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  Slot& slot) const
{
  if (oppsValue1 == 0)
  {
    slot.setPhrase(TOPS_ATMOST);
    slot.setValues(oppsValue2, onetopIndex);
  }
  else if (oppsValue2 == oppsSize || oppsValue2 == 0xf)
  {
    slot.setPhrase(TOPS_ATLEAST);
    slot.setValues(oppsValue1, onetopIndex);
  }
  else if (oppsValue1 + oppsValue2 == oppsSize)
  {
    slot.setPhrase(TOPS_RANGE);
    slot.setValues(oppsValue1, oppsValue2, onetopIndex);
  }
  else
  {
    // TODO This looks identical?!
    slot.setPhrase(TOPS_RANGE);
    slot.setValues(oppsValue1, oppsValue2, onetopIndex);
  }
}


// TODO Later on private and at the right place in the file again
void VerbalCover::getOnetopData(
  const unsigned char oppsValue1,
  const unsigned char oppsValue2,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  const BlankPlayerCap side)
{
  // Here lower and upper are different.
  slots.resize(2);

  if (oppsValue1 == 0)
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, side, SLOT_NONE);
  }
  else if (oppsValue2 == oppsSize || oppsValue2 == 0xf)
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, side, SLOT_NONE);
  }
  else if (oppsValue1 + oppsValue2 == oppsSize)
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, BLANK_PLAYER_CAP_EACH, 
      SLOT_NONE);
  }
  else
  {
    slots[0].setSemantics(PHRASE_PLAYER_CAP, side, SLOT_NONE);
  }

  VerbalCover::getOnetopElement(oppsValue1, oppsValue2, oppsSize,
    onetopIndex, slots[1]);
}


string VerbalCover::str(const RanksNames& ranksNames) const
{
  if (sentence == SENTENCE_SIZE)
    return strTMP;
  else
    return verbalTemplates.get(
      sentence, ranksNames, completions, slots);
}

