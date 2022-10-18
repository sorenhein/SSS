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
/*                         Length methods                             */
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
    phrases[1].setPhrase(LENGTH_VOID);
  }
  else if (value == 1 ||
    (value == 2 && (! abstractableFlag || oppsLength > 4)) ||
    (value == 3 && (! abstractableFlag || oppsLength > 6)))
  {
    phrases[0].setPhrase(vside.player());
    phrases[1].setPhrase(LENGTH_XTON);
    phrases[1].setValues(value);
  }
  else if (! abstractableFlag)
  {
    assert(false);
  }
  else if (value + value == oppsLength)
  {
    phrases[0].setPhrase(PLAYER_SUIT);
    phrases[1].setPhrase(LENGTH_EVENLY);
  }
  else
  {
    phrases[0].setPhrase(PLAYER_SUIT);
    phrases[1].setPhrase(LENGTH_SPLIT);
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
      phrases[1].setPhrase(LENGTH_NOT_VOID);
    }
    else if (vUpper <= 3)
    {
      phrases[0].setPhrase(vside.player());
      phrases[1].setPhrase(LENGTH_XTON_ATMOST);
      phrases[1].setValues(vUpper);
    }
    else
    {
      phrases[0].setPhrase(vside.player());
      phrases[1].setPhrase(LENGTH_ATMOST);
      phrases[1].setValues(vUpper);
    }
  }
  else if (! abstractableFlag)
  {
    if (vLower == 1 && vUpper == 2)
    {
      phrases[0].setPhrase(vside.player());
      phrases[1].setPhrase(LENGTH_12);
    }
    else
    {
      phrases[0].setPhrase(vside.player());
      phrases[1].setPhrase(
        vLower+1 == vUpper ? LENGTH_RANGE : LENGTH_BETWEEN);
      phrases[1].setValues(vLower, vUpper);
    }
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
    phrases[0].setPhrase(PLAYER_NEITHER);
    phrases[1].setPhrase(LENGTH_VOID);
  }
  else if (vLower+1 == vUpper && vLower + vUpper == oppsLength)
  {
    phrases[0].setPhrase(PLAYER_SUIT);
    phrases[1].setPhrase(LENGTH_ODD_EVENLY);
  }
  else if (vLower + vUpper == oppsLength)
  {
    phrases[0].setPhrase(PLAYER_EACH);
    phrases[1].setPhrase(
      vLower+1 == vUpper ? LENGTH_RANGE : LENGTH_BETWEEN);
    phrases[1].setValues(vLower, vUpper);
  }
  else if (vLower == 1 && vUpper == 2)
  {
    phrases[0].setPhrase(vside.player());
    phrases[1].setPhrase(LENGTH_12);
  }
  else
  {
    phrases[0].setPhrase(vside.player());
    phrases[1].setPhrase(
      vLower+1 == vUpper ? LENGTH_RANGE : LENGTH_BETWEEN);
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
/*                   Single-phrase help methods                       */
/*                                                                    */
/**********************************************************************/

void VerbalCover::fillLengthOrdinal(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  Phrase& phrase)
{
  unsigned char vLower, vUpper;
  length.range(oppsLength, simplestOpponent, vLower, vUpper);

  if (vLower == vUpper)
  {
    phrase.setPhrase(ORDINAL_EXACT);
    phrase.setValues(vLower);
  }
  else if (vLower == 0)
  {
    phrase.setPhrase(ORDINAL_ATMOST);
    phrase.setValues(vUpper);
  }
  else if (vLower + 1 == vUpper)
  {
    phrase.setPhrase(ORDINAL_ADJACENT);
    phrase.setValues(vLower, vUpper);
  }
  else
  {
    phrase.setPhrase(ORDINAL_RANGE);
    phrase.setValues(vLower, vUpper);
  }
}


void VerbalCover::fillTopsActual(
  const Opponent side,
  Phrase& phrase)
{
  const Completion& completion = completions.front();

  if (! completion.expandable(side))
  {
    phrase.setPhrase(TOPS_SET);
    phrase.setSide(side);
    phrase.setBools(false);
  }
  else
  {
    phrase.setPhrase(TOPS_DEFINITE);
    phrase.setValues(completion.getLowestRankActive(side));
  }
}


void VerbalCover::fillFreeCount(
  const VerbalSide& vside,
  Phrase& phrase) const
{
  const auto& completion = completions.front();
  const unsigned char freeLower = completion.getFreeLower(vside.side);
  const unsigned char freeUpper = completion.getFreeUpper(vside.side);

  if (freeLower == freeUpper)
  {
    phrase.setPhrase(COUNT_EXACT);
    phrase.setValues(freeLower);
  }
  else if (freeLower == 0)
  {
    phrase.setPhrase(COUNT_ATMOST);
    phrase.setValues(freeUpper);
  }
  else
  {
    phrase.setPhrase(COUNT_RANGE);
    phrase.setValues(freeLower, freeUpper);
  }
}


/**********************************************************************/
/*                                                                    */
/*                          Fill methods                              */
/*                                                                    */
/**********************************************************************/

void VerbalCover::fillLength(
  const Term& lengthIn,
  const unsigned char oppsLength,
  const bool symmFlag)
{
  sentence = SENTENCE_LENGTH;
  VerbalCover::setLength(lengthIn);

  const Opponent side = (symmFlag ? OPP_WEST : length.shorter(oppsLength));
  const VerbalSide vside = {side, symmFlag};

  VerbalCover::getLengthData(oppsLength, vside, true);
}


void VerbalCover::fillTops(const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS;
  phrases.resize(2);

  phrases[0].setPhrase(vside.player());

  VerbalCover::fillTopsActual(vside.side, phrases[1]);
}


void VerbalCover::fillLengthBelowTops(
  const unsigned char numBottoms,
  const unsigned char rankNo,
  const VerbalSide& vside)
{
  sentence = SENTENCE_LENGTH_BELOW_TOPS;

  const auto& completion = completions.front();
  const unsigned char freeLower = completion.getFreeLower(OPP_WEST);
  const unsigned char freeUpper = completion.getFreeUpper(OPP_WEST);

  // Make a synthetic length of small cards.
  VerbalCover::setLength(freeLower, freeUpper, numBottoms);

  // Set templateFills numbers 0 and 1 (and the size of 2).
  VerbalCover::getLengthData(numBottoms, vside, false);
  
  phrases.resize(4);

  if (completion.getFreeUpper(vside.side) == 1)
    phrases[2].setPhrase(DICT_BELOW);
  else
    phrases[2].setPhrase(DICT_BELOW_COMPLETELY);

  phrases[3].setPhrase(TOPS_LOWEST);
  phrases[3].setValues(rankNo);
}


void VerbalCover::fillOrdinalFromTops(
  const VerbalSide& vside,
  const unsigned char lenCompletion)
{
  sentence = SENTENCE_ORDINAL_FROM_TOPS;

  const auto& completion = completions.front();

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(ORDINAL_EXACT);
  phrases[1].setValues(lenCompletion);

  phrases[2].setPhrase(TOPS_DEFINITE);
  phrases[2].setValues(completion.getLowestRankActive(vside.side));
}


void VerbalCover::fillCountTops(
  const Term& top,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  sentence = SENTENCE_COUNT_TOPS;

  unsigned char vLower, vUpper;
  top.range(oppsSize, vside.side, vLower, vUpper);

  // Here lower and upper are different.
  phrases.resize(3);

  if (vLower + vUpper == oppsSize)
    phrases[0].setPhrase(PLAYER_EACH);
  else
    phrases[0].setPhrase(vside.player());

  if (vLower == 0)
  {
    phrases[1].setPhrase(COUNT_ATMOST);
    phrases[1].setValues(vUpper);

    phrases[2].setPhrase(TOPS_OF_DEFINITE);
    phrases[2].setValues(onetopIndex);
    phrases[2].setBools(vUpper > 1);
  }
  else if (vUpper == oppsSize)
  {
    phrases[1].setPhrase(COUNT_ATLEAST);
    phrases[1].setValues(vLower);

    phrases[2].setPhrase(TOPS_OF_DEFINITE);
    phrases[2].setValues(onetopIndex);
    phrases[2].setBools(vLower > 1);
  }
  else if (vLower+1 == vUpper)
  {
    phrases[1].setPhrase(COUNT_OR);
    phrases[1].setValues(vLower, vUpper);

    phrases[2].setPhrase(TOPS_OF_DEFINITE);
    phrases[2].setValues(onetopIndex);
    phrases[2].setBools(vUpper > 1);
  }
  else
  {
    phrases[1].setPhrase(DIGITS_RANGE);
    phrases[1].setValues(vLower, vUpper);

    phrases[2].setPhrase(TOPS_OF_DEFINITE);
    phrases[2].setValues(onetopIndex);
    phrases[2].setBools(vUpper > 1);
  }
}


void VerbalCover::fillExactlyCountTops(const VerbalSide& vside)
{
  sentence = SENTENCE_EXACTLY_COUNT_TOPS;
  phrases.resize(3);

  const auto& completion = completions.front();
  const unsigned char len = completion.getTopsUsed(vside.side);

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(COUNT_EXACT);
  phrases[1].setValues(len);

  phrases[2].setPhrase(TOPS_OF_DEFINITE);
  phrases[2].setValues(completion.getLowestRankActive(vside.side));
  phrases[2].setBools(len > 1);
}


void VerbalCover::fillTopsOrdinal(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_ORDINAL;
  phrases.resize(3);

  phrases[0].setPhrase(vside.player());

  VerbalCover::fillTopsActual(vside.side, phrases[1]);

  VerbalCover::fillLengthOrdinal(
    sumProfile.length(), vside.side, phrases[2]);
}


void VerbalCover::fillTopsOrdinal(
  [[maybe_unused]] const Profile& sumProfile,
  const unsigned char lenCompletion,
  const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_ORDINAL;
  // TODO resize goes here

  phrases[0].setPhrase(vside.player());

  VerbalCover::fillTopsActual(vside.side, phrases[1]);

  phrases[2].setPhrase(ORDINAL_EXACT);
  phrases[2].setValues(lenCompletion);
}


void VerbalCover::fillCountTopsOrdinal(
  const Term& lengthIn,
  const Term& top,
  const Profile& sumProfile,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  VerbalCover::setLength(lengthIn);

  // Fill positions 0 and 1.
  VerbalCover::fillCountTops(
    top, sumProfile[onetopIndex], onetopIndex, vside);

  sentence = SENTENCE_COUNT_TOPS_ORDINAL;

  phrases.resize(4);

  VerbalCover::fillLengthOrdinal(
    sumProfile.length(), vside.side, phrases[3]);
}


void VerbalCover::fillCountHonorsOrdinal(
  const unsigned char oppsLength,
  const VerbalSide& vside)
{
  sentence = SENTENCE_COUNT_HONORS_ORDINAL;
  phrases.resize(4);

  const Opponent side = vside.side;
  const Completion& completion = completions.front();
  const unsigned char numHonors = completion.getTopsUsed(side);

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(COUNT_EXACT);
  phrases[1].setValues(numHonors);

  phrases[2].setPhrase(numHonors == 1 ? DICT_HONOR : DICT_HONORS);

  VerbalCover::fillLengthOrdinal(oppsLength, side, phrases[3]);
}


void VerbalCover::fillExactlyCountTopsOrdinal(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  sentence = SENTENCE_EXACTLY_COUNT_TOPS_ORDINAL;
  phrases.resize(4);

  const Completion& completion = completions.front();
  const unsigned char len = completion.getTopsUsed(vside.side);

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(COUNT_EXACT);
  phrases[1].setValues(len);

  phrases[2].setPhrase(TOPS_OF_DEFINITE);
  phrases[2].setValues(completion.getLowestRankActive(vside.side));
  phrases[2].setBools(len > 1);

  VerbalCover::fillLengthOrdinal(
    sumProfile.length(), vside.side, phrases[3]);
}


void VerbalCover::fillTopsAndLower(const VerbalSide& vside)
{
  phrases.resize(4);

  const auto& completion = completions.front();

  phrases[0].setPhrase(vside.player());

  VerbalCover::fillTopsActual(vside.side, phrases[1]);

  const unsigned char freeLower = completion.getFreeLower(vside.side);
  const unsigned char freeUpper = completion.getFreeUpper(vside.side);

  VerbalCover::fillFreeCount(vside, phrases[2]);

  sentence = SENTENCE_TOPS_AND_LOWER;

  if (freeUpper == 1)
    phrases[3].setPhrase(DICT_CARD);
  else
    phrases[3].setPhrase(DICT_CARDS);
}


void VerbalCover::fillExactlyTopsAndLower(const VerbalSide& vside)
{
  sentence = SENTENCE_EXACTLY_TOPS_AND_LOWER;
  phrases.resize(5);

  const auto& completion = completions.front();
  const unsigned char len = completion.getTopsUsed(vside.side);
  const unsigned char freeLower = completion.getFreeLower(vside.side);
  const unsigned char freeUpper = completion.getFreeUpper(vside.side);

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(COUNT_EXACT);
  phrases[1].setValues(len);

  phrases[2].setPhrase(TOPS_OF_DEFINITE);
  phrases[2].setValues(completion.getLowestRankActive(vside.side));
  phrases[2].setBools(len > 1);

  VerbalCover::fillFreeCount(vside, phrases[3]);

  if (freeUpper == 1)
    phrases[4].setPhrase(DICT_CARD);
  else
    phrases[4].setPhrase(DICT_CARDS);
}


void VerbalCover::fillTopsAndCountBelowCard(
  const VerbalSide& vside,
  const unsigned char numOptions)
{
  // TODO Set directly to 5?
  phrases.resize(4);

  const auto& completion = completions.front();
  const unsigned char freeLower = completion.getFreeLower(vside.side);
  const unsigned char freeUpper = completion.getFreeUpper(vside.side);

  phrases[0].setPhrase(vside.player());

  VerbalCover::fillTopsActual(vside.side, phrases[1]);

  VerbalCover::fillFreeCount(vside, phrases[2]);

  sentence = SENTENCE_TOPS_AND_COUNT_BELOW_CARD;
  phrases.resize(5);

  if (freeUpper == 1)
    phrases[3].setPhrase(DICT_CARD);
  else
    phrases[3].setPhrase(DICT_CARDS);

  phrases[4].setPhrase(TOPS_LOWEST);
  phrases[4].setValues(numOptions);
}


void VerbalCover::fillExactlyTopsMaybeUnset(const VerbalSide& vside)
{
  sentence = SENTENCE_EXACTLY_TOPS_MAYBE_UNSET;
  phrases.resize(3);

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(TOPS_SET);
  phrases[1].setSide(vside.side);
  phrases[1].setBools(false);

  phrases[2].setPhrase(TOPS_UNSET);
  phrases[2].setSide(vside.side);
}


void VerbalCover::fillTopsAndXes(const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_AND_XES;
  phrases.resize(3);

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(TOPS_SET);
  phrases[1].setSide(vside.side);
  phrases[1].setBools(false);

  phrases[2].setPhrase(TOPS_XES);
  phrases[2].setSide(vside.side);
}


void VerbalCover::fillExactlyList(const VerbalSide& vside)
{
  sentence = SENTENCE_EXACTLY_LIST;
  phrases.resize(completions.size() + 1);

  phrases[0].setPhrase(vside.player());

  for (size_t i = 1; i < phrases.size(); i++)
  {
    phrases[i].setPhrase(LIST_HOLDING);
    phrases[i].setSide(vside.side);
    phrases[i].setBools(true);
  }
}


/**********************************************************************/
/*                                                                    */
/*                 Methods with multiple sentence options             */
/*                                                                    */
/**********************************************************************/

void VerbalCover::fillOnesided(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  // length is already set.
  // TODO Let VerbalCover do this?
  const Completion& completion = completions.front();

  if (completion.expandable(vside.side) &&
      ! completion.fullRanked(vside.side))
  {
    VerbalCover::fillExactlyCountTopsOrdinal(sumProfile, vside);
  }
  else
  {
    VerbalCover::fillTopsOrdinal(sumProfile, vside);
  }
}


void VerbalCover::fillTopsBothLength(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  // length is already set.
  if (vside.symmFlag)
    sentence = SENTENCE_2SPLIT_TOPS_DIGITS_SYMM;
  else
    sentence = SENTENCE_2SPLIT_TOPS_DIGITS_ASYMM;

  phrases.resize(4);

  const Completion& completion = completions.front();
  // TODO completion method
  const bool bothExpandableFlag =
    completion.expandable(OPP_WEST) &&
    completion.expandable(OPP_EAST);

  // TODO VerbalSide method
  const Opponent sideOther = (vside.side == OPP_WEST ? OPP_EAST : OPP_WEST);

  if (bothExpandableFlag)
  {
    phrases[0].setPhrase(TOPS_INDEFINITE);
    phrases[0].setValues(completion.getLowestRankActive(vside.side));

    // TODO Here too like below, length.range?
    phrases[1].setPhrase(DIGITS_RANGE);
    phrases[1].setValues(length.lower(), length.upper());

    phrases[2].setPhrase(TOPS_INDEFINITE);
    phrases[2].setValues(completion.getLowestRankActive(sideOther));

    phrases[3].setPhrase(DIGITS_RANGE);
    phrases[3].setValues( 
      sumProfile.length() - length.upper(),
      sumProfile.length() - length.lower());
  }
  else
  {
    // TODO VerbalSide method
    Opponent side1, side2;
    if (vside.symmFlag)
    {
      side1 = vside.side;
      side2 = sideOther;
    }
    else
    {
      side1 = OPP_WEST;
      side2 = OPP_EAST;
    }

    unsigned char vLower, vUpper;
    length.range(sumProfile.length(), side1, vLower, vUpper);

    phrases[0].setPhrase(TOPS_SET);
    phrases[0].setSide(side1);

    phrases[1].setPhrase(DIGITS_RANGE);
    phrases[1].setValues(vLower, vUpper);

    length.range(sumProfile.length(), side2, vLower, vUpper);

    phrases[2].setPhrase(TOPS_SET);
    phrases[2].setSide(side2);

    phrases[3].setPhrase(DIGITS_RANGE);
    phrases[3].setValues(vLower, vUpper);
  }
}


void VerbalCover::fillTopsBoth(const VerbalSide& vside)
{
  // TODO New method completion.expandableBoth()
  const Completion& completion = completions.front();
  const bool bothExpandableFlag =
    completion.expandable(OPP_WEST) &&
    completion.expandable(OPP_EAST);

  phrases.resize(2);

  // TODO VerbalSide new method?
  Opponent side1, side2;
  if (vside.symmFlag)
  {
    sentence = SENTENCE_2SPLIT_TOPS_SYMM;

    side1 = vside.side;
    side2 = (side1 == OPP_WEST ? OPP_EAST : OPP_WEST);
  }
  else
  {
    sentence = SENTENCE_2SPLIT_TOPS_ASYMM;

    side1 = OPP_WEST;
    side2 = OPP_EAST;
  }

  if (bothExpandableFlag)
  {
    phrases[0].setPhrase(TOPS_INDEFINITE);
    phrases[0].setValues(completion.getLowestRankActive(side1));

    phrases[1].setPhrase(TOPS_INDEFINITE);
    phrases[1].setValues(completion.getLowestRankActive(side2));
  }
  else
  {
    phrases[0].setPhrase(TOPS_SET);
    phrases[0].setSide(side1);

    phrases[1].setPhrase(TOPS_SET);
    phrases[1].setSide(side2);
  }
}


void VerbalCover::fillTopsAndLowerMultiple(
  const Profile& sumProfile,
  const VerbalSide& vside,
  const unsigned char numOptions)
{
  const auto& completion = completions.front();

  if (completion.expandable(vside.side) &&
      ! completion.fullRanked(vside.side) &&
      completion.highRanked(vside.side))
  {
    VerbalCover::fillCountHonorsOrdinal(sumProfile.length(), vside);
    return;
  }

  if (completion.lowestRankIsUsed(vside.side))
  {
    if (completion.expandable(vside.side) &&
        ! completion.fullRanked(vside.side))
    {
      VerbalCover::fillExactlyTopsAndLower(vside);
    }
    else
    {
      VerbalCover::fillTopsAndLower(vside);
    }
  }
  else
  {
    assert((! completion.expandable(vside.side)) ||
        completion.fullRanked(vside.side));

    VerbalCover::fillTopsAndCountBelowCard(vside, numOptions);
  }
}


void VerbalCover::fillSingular(
  const Profile& sumProfile,
  const unsigned char lenCompletion,
  const VerbalSide& vside)
{
  phrases.resize(3);

  const Completion& completion = completions.front();
  const Opponent side = vside.side;

  if (! completion.expandable(side) || completion.fullRanked(side))
  {
    VerbalCover::fillTopsOrdinal(sumProfile, lenCompletion, vside);
  }
  else if (completion.highRanked(side))
  {
    VerbalCover::fillCountHonorsOrdinal(sumProfile.length(), vside);
  }
  else
  {
    VerbalCover::fillOrdinalFromTops(vside, lenCompletion);
  }
}


void VerbalCover::fillCompletion(const VerbalSide& vside)
{
  const Completion& completion = completions.front();

  if (! completion.expandable(vside.side))
  {
    // Make a list with one completion.
    VerbalCover::fillExactlyList(vside);
  }
  else if (! completion.fullRanked(vside.side))
  {
    VerbalCover::fillExactlyCountTops(vside);
  }
  else
  {
    VerbalCover::fillTops(vside);
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

