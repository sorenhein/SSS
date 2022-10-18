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


void VerbalCover::fillOnetopOnlyOld(
  const Term& top,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  sentence = SENTENCE_COUNT_ONETOP;

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
assert(false);
    phrases.resize(3);

    phrases[1].setPhrase(COUNT_OR);
    phrases[1].setValues(vLower, vUpper, onetopIndex);
  }
}


void VerbalCover::fillOnetopOnly(
  const Term& top,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  sentence = SENTENCE_COUNT_ONETOP;

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
    // phrases.resize(3);

    phrases[1].setPhrase(COUNT_OR);
    phrases[1].setValues(vLower, vUpper);

    phrases[2].setPhrase(TOPS_OF_DEFINITE);
    phrases[2].setValues(onetopIndex);
    phrases[2].setBools(vUpper > 1);
  }
  else
  {
    // phrases.resize(3);

    phrases[1].setPhrase(DIGITS_RANGE);
    phrases[1].setValues(vLower, vUpper);

    phrases[2].setPhrase(TOPS_OF_DEFINITE);
    phrases[2].setValues(onetopIndex);
    phrases[2].setBools(vUpper > 1);
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


void VerbalCover::fillOnetopLength(
  const Term& lengthIn,
  const Term& top,
  const Profile& sumProfile,
  const unsigned char onetopIndex,
  const VerbalSide& vside)
{
  VerbalCover::setLength(lengthIn);

  // Fill templateFills positions 0 and 1.
  VerbalCover::fillOnetopOnlyOld(
    top, sumProfile[onetopIndex], onetopIndex, vside);

  sentence = SENTENCE_TOPS_LENGTH_OLD;

  phrases.resize(4);

  VerbalCover::fillLengthOrdinal(
    sumProfile.length(), vside.side, phrases[3]);
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


void VerbalCover::fillOnesided(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  // length is already set.
  const Completion& completion = completions.front();
  if (completion.expandable(vside.side) &&
      ! completion.fullRanked(vside.side))
  {
    sentence = SENTENCE_TOPS_LENGTH_OLDER;

    phrases.resize(4);

    phrases[0].setPhrase(vside.player());

    const unsigned char len = completion.getTopsUsed(vside.side);

    phrases[1].setPhrase(COUNT_EXACT);
    phrases[1].setValues(len);

    phrases[2].setPhrase(TOPS_OF_DEFINITE);
    phrases[2].setValues(completion.getLowestRankActive(vside.side));
    phrases[2].setBools(len > 1);

    VerbalCover::fillLengthOrdinal(
      sumProfile.length(), vside.side, phrases[3]);
  }
  else
  {
    sentence = SENTENCE_TOPS_LENGTH;

    phrases.resize(3);

    phrases[0].setPhrase(vside.player());

    VerbalCover::fillTopsActual(vside.side, phrases[1]);

    VerbalCover::fillLengthOrdinal(
      sumProfile.length(), vside.side, phrases[2]);
  }

}


void VerbalCover::fillTopsBothLength(
  const Profile& sumProfile,
  const VerbalSide& vside)
{
  // length is already set.
  sentence = SENTENCE_TOPS_BOTH_LENGTH;

  phrases.resize(5);

  const Completion& completion = completions.front();
  const bool bothExpandableFlag =
    completion.expandable(OPP_WEST) &&
    completion.expandable(OPP_EAST);

  const Opponent sideOther = (vside.side == OPP_WEST ? OPP_EAST : OPP_WEST);

  if (bothExpandableFlag)
  {
    phrases[0].setPhrase(TOPS_INDEFINITE);
    phrases[0].setValues(completion.getLowestRankActive(vside.side));

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

  if (vside.symmFlag)
    phrases[4].setPhrase(EITHER_WAY);
  else
    phrases[4].setPhrase(ONE_WAY);
}


void VerbalCover::fillTopsBoth(const VerbalSide& vside)
{
  // This is an expansion with quite a lot of parameters in
  // a single Phrase.

  const Completion& completion = completions.front();
  const bool bothExpandableFlag =
    completion.expandable(OPP_WEST) &&
    completion.expandable(OPP_EAST);

  phrases.resize(2);

  Opponent side1, side2;
  if (vside.symmFlag)
  {
    sentence = SENTENCE_TOPS_BOTH_SYMM;

    side1 = vside.side;
    side2 = (side1 == OPP_WEST ? OPP_EAST : OPP_WEST);
  }
  else
  {
    sentence = SENTENCE_TOPS_BOTH_NOT_SYMM;

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


void VerbalCover::fillTopsAndLower(
  const Profile& sumProfile,
  const VerbalSide& vside,
  const unsigned char numOptions)
{
  const auto& completion = completions.front();

  if (completion.expandable(vside.side) &&
      ! completion.fullRanked(vside.side) &&
      completion.highRanked(vside.side))
  {
    VerbalCover::fillHonorsOrdinal(sumProfile.length(), vside);
    return;
  }

  if (completion.lowestRankIsUsed(vside.side))
  {
    if (completion.expandable(vside.side) &&
        ! completion.fullRanked(vside.side))
    {
      sentence = SENTENCE_TOPS_AND_LOWER_NEW;

      phrases.resize(5);

      phrases[0].setPhrase(vside.player());

      const unsigned char len = completion.getTopsUsed(vside.side);

      phrases[1].setPhrase(COUNT_EXACT);
      phrases[1].setValues(len);

      phrases[2].setPhrase(TOPS_OF_DEFINITE);
      phrases[2].setValues(completion.getLowestRankActive(vside.side));
      phrases[2].setBools(len > 1);

      const unsigned char freeLower = completion.getFreeLower(vside.side);
      const unsigned char freeUpper = completion.getFreeUpper(vside.side);

      VerbalCover::fillFreeCount(vside, phrases[3]);


      phrases[4].setPhrase(TOPS_CARDS_WORD);
      phrases[4].setBools(freeUpper > 1);
    }
    else
    {
      phrases.resize(4);

      phrases[0].setPhrase(vside.player());

      VerbalCover::fillTopsActual(vside.side, phrases[1]);

      const unsigned char freeLower = completion.getFreeLower(vside.side);
      const unsigned char freeUpper = completion.getFreeUpper(vside.side);

      VerbalCover::fillFreeCount(vside, phrases[2]);

      sentence = SENTENCE_TOPS_AND_LOWER;

      phrases[3].setPhrase(TOPS_CARDS_WORD);
      phrases[3].setBools(freeUpper > 1);
    }
  }
  else
  {
    if (completion.expandable(vside.side) &&
        ! completion.fullRanked(vside.side))
    {
      sentence = SENTENCE_TOPS_AND_BELOW_NEW;

      phrases.resize(6);

      phrases[0].setPhrase(vside.player());

      const unsigned char len = completion.getTopsUsed(vside.side);

      phrases[1].setPhrase(COUNT_EXACT);
      phrases[1].setValues(len);

      phrases[2].setPhrase(TOPS_OF_DEFINITE);
      phrases[2].setValues(completion.getLowestRankActive(vside.side));
      phrases[2].setBools(len > 1);

      const unsigned char freeLower = completion.getFreeLower(vside.side);
      const unsigned char freeUpper = completion.getFreeUpper(vside.side);

      VerbalCover::fillFreeCount(vside, phrases[3]);

      // phrases.resize(5);

      phrases[4].setPhrase(TOPS_CARDS_WORD);
      phrases[4].setBools(freeUpper > 1);

      phrases[5].setPhrase(TOPS_LOWEST);
      phrases[5].setValues(numOptions);
    }
    else
    {
      phrases.resize(4);

      phrases[0].setPhrase(vside.player());

      VerbalCover::fillTopsActual(vside.side, phrases[1]);

      const unsigned char freeLower = completion.getFreeLower(vside.side);
      const unsigned char freeUpper = completion.getFreeUpper(vside.side);

      VerbalCover::fillFreeCount(vside, phrases[2]);

      sentence = SENTENCE_TOPS_AND_BELOW;
      phrases.resize(5);

      phrases[3].setPhrase(TOPS_CARDS_WORD);
      phrases[3].setBools(freeUpper > 1);

      phrases[4].setPhrase(TOPS_LOWEST);
      phrases[4].setValues(numOptions);
    }

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

  phrases[3].setPhrase(TOPS_LOWEST);
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


void VerbalCover::fillHonorsOrdinal(
  const unsigned char oppsLength,
  const VerbalSide& vside)
{
  phrases.resize(3);

  const Completion& completion = completions.front();
  const Opponent side = vside.side;

  sentence = SENTENCE_HONORS_ORDINAL;

  phrases[0].setPhrase(vside.player());

  VerbalCover::fillHonorsEqual(
    completion.getTopsUsed(side), phrases[1]);

  VerbalCover::fillLengthOrdinal(oppsLength, side, phrases[2]);
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
    sentence = SENTENCE_TOPS_LENGTH;

    phrases[0].setPhrase(vside.player());

    VerbalCover::fillTopsActual(side, phrases[1]);

    phrases[2].setPhrase(ORDINAL_EXACT);
    phrases[2].setValues(lenCompletion);
  }
  else if (completion.highRanked(side))
  {
    VerbalCover::fillHonorsOrdinal(sumProfile.length(), vside);
  }
  else
  {
    sentence = SENTENCE_SINGULAR_EXACT;

    phrases[0].setPhrase(vside.player());

    phrases[1].setPhrase(ORDINAL_EXACT);
    phrases[1].setValues(lenCompletion);

    phrases[2].setPhrase(TOPS_DEFINITE);
    phrases[2].setValues(completion.getLowestRankActive(side));
  }
}


void VerbalCover::fillCompletion(const VerbalSide& vside)
{

  const Completion& completion = completions.front();

  if (! completion.expandable(vside.side))
  {
    phrases.resize(2);

    sentence = SENTENCE_LIST;

    phrases[0].setPhrase(vside.player());

    phrases[1].setPhrase(LIST_HOLDING_EXACT);
    phrases[1].setSide(vside.side);
    phrases[1].setBools(false);
  }
  else if (completion.expandable(vside.side) &&
      ! completion.fullRanked(vside.side))
  {
    phrases.resize(3);

    sentence = SENTENCE_EXACT_COUNT_ONETOP;

    phrases[0].setPhrase(vside.player());

    const unsigned char len = completion.getTopsUsed(vside.side);

    phrases[1].setPhrase(COUNT_EXACT);
    phrases[1].setValues(len);

    phrases[2].setPhrase(TOPS_OF_DEFINITE);
    phrases[2].setValues(completion.getLowestRankActive(vside.side));
    phrases[2].setBools(len > 1);
  }
  else
  {
    phrases.resize(2);

    sentence = SENTENCE_ONETOP_ONLY;

    phrases[0].setPhrase(vside.player());

    VerbalCover::fillTopsActual(vside.side, phrases[1]);
  }
}


void VerbalCover::fillCompletionWithLows(const VerbalSide& vside)
{
  sentence = SENTENCE_SET_UNSET;

  phrases.resize(3);

  phrases[0].setPhrase(vside.player());

  phrases[1].setPhrase(TOPS_SET);
  phrases[1].setSide(vside.side);
  phrases[1].setBools(false);

  phrases[2].setPhrase(TOPS_UNSET);
  phrases[2].setSide(vside.side);
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

