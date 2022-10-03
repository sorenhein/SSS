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
#include "./Completion.h"

#include "../../../ranks/RanksNames.h"

#include "../../../utils/table.h"

extern VerbalTemplates verbalTemplates;


const vector<string> topCount =
{
  "none",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "twelve",
  "thirteen"
};

const vector<string> topOrdinal =
{
  "void",
  "singleton",
  "doubleton",
  "tripleton",
  "fourth",
  "fifth",
  "sixth",
  "seventh",
  "eighth",
  "ninth",
  "tenth",
  "eleventh",
  "twelfth",
  "thirteenth"
};


VerbalCover::VerbalCover()
{
  sentence = SENTENCE_SIZE;
  lengthFlag = false;
  lengthLower = 0;
  lengthUpper = 0;
  lengthOper = COVER_EQUAL;

  templateFills.clear();
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


void VerbalCover::fillLengthOnly(
  const Term& length,
  const unsigned char oppsLength,
  const bool symmFlag)
{
  VerbalCover::setLength(length);
  sentence = SENTENCE_LENGTH_ONLY;

  const bool westFlag = (completion.length(OPP_WEST) > 0);
  const bool eastFlag = (completion.length(OPP_EAST) > 0);

  Opponent simplestOpponent;
  if (symmFlag)
    simplestOpponent = OPP_WEST;
  else if (westFlag == eastFlag)
    simplestOpponent = VerbalCover::simplestOpponent(oppsLength);
  else if (westFlag)
    simplestOpponent = OPP_WEST;
  else
    simplestOpponent = OPP_EAST;

// cout << "wf " << westFlag << " ef " << eastFlag << " symm " <<
  // symmFlag << " sopp " << simplestOpponent << endl;

  const VerbalSide vside = {simplestOpponent, symmFlag};
  VerbalCover::getLengthData(oppsLength, vside, true, templateFills);
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
      vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST,
      templateFills);
  }
  else
  {
    VerbalCover::getOnetopData(
      top.upper() == 0xf ? 0 : oppsSize - top.upper(),
      oppsSize - top.lower(),
      oppsSize,
      onetopIndex,
      vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST,
      templateFills);
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


void VerbalCover::fillTopsExcluding(
  const VerbalSide& vside,
  const RanksNames& ranksNames)
{
  sentence = SENTENCE_TOPS_EXCLUDING;

  BlankPlayerCap bside = vside.blank();

  templateFills.resize(4);

  templateFills[0].set(BLANK_PLAYER_CAP, bside);

  // These should be expanded later in VerbalTemplates.
  templateFills[1].setBlank(BLANK_TOPS);
  templateFills[1].setData(BLANK_TOPS_ACTUAL, 
    completion.strSetNew(ranksNames, vside.side, true, true));

  const unsigned topsFull = completion.getTopsFull(
    vside.side == OPP_WEST ? OPP_EAST : OPP_WEST);

  if (topsFull <= 1)
    templateFills[2].set(BLANK_EXCLUDING, BLANK_EXCLUDING_NOT);
  else if (topsFull == 2)
    templateFills[2].set(BLANK_EXCLUDING, BLANK_EXCLUDING_NEITHER);
  else
    templateFills[2].set(BLANK_EXCLUDING, BLANK_EXCLUDING_NONE);

  templateFills[3].setBlank(BLANK_TOPS);

  templateFills[3].setData(BLANK_TOPS_ACTUAL, 
    completion.strSetNew(ranksNames, 
      vside.side == OPP_WEST ? OPP_EAST : OPP_WEST, true, true));
}


Completion& VerbalCover::getCompletion()
{
  return completion;
}


void VerbalCover::stable_sort()
{
  std::stable_sort(completions.begin(), completions.end());
}


unsigned char VerbalCover::size() const
{
  return static_cast<unsigned char>(completions.size());
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
  const bool abstractableFlag,
  vector<TemplateData>& tdata) const
{
  // Here lower and upper are identical.
  BlankPlayerCap bside;
  unsigned char value;

  if (vside.side == OPP_WEST)
  {
    bside = (vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST);
    value = lengthLower;
  }
  else
  {
    bside = (vside.symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST);
    value = oppsLength - lengthLower;
  }

  tdata.resize(2);
  if (value == 0)
  {
    tdata[0].set(BLANK_PLAYER_CAP, bside);
    tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_VOID);
  }
  else if (value == 1)
  {
    tdata[0].set(BLANK_PLAYER_CAP, bside);
    tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_SINGLE);
  }
  else if (value == 2 && (! abstractableFlag || oppsLength > 4))
  {
    tdata[0].set(BLANK_PLAYER_CAP, bside);
    tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_DOUBLE);
  }
  else if (value == 3 && (! abstractableFlag || oppsLength > 6))
  {
    tdata[0].set(BLANK_PLAYER_CAP, bside);
    tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_TRIPLE);
  }
  else if (! abstractableFlag)
  {
cout << "bside " << bside << endl;
cout << "value " << +value << endl;
    assert(false);
  }
  else if (value + value == oppsLength)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT);
    tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_EVENLY);
  }
  else
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT);
    tdata[1].setBlank(BLANK_LENGTH_VERB);
    tdata[1].setData(BLANK_LENGTH_VERB_SPLIT_PARAMS,
      lengthLower, oppsLength - lengthLower);
  }
}


void VerbalCover::getLengthInsideData(
  const unsigned char oppsLength,
  const VerbalSide& vside,
  const bool abstractableFlag,
  vector<TemplateData>& tdata) const
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

  tdata.resize(2);
  if (vLower == 0)
  {
    if (vUpper == 1)
    {
      tdata[0].set(BLANK_PLAYER_CAP, bside);
      tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_SINGLE_ATMOST);
    }
    else if (vUpper == 2)
    {
      tdata[0].set(BLANK_PLAYER_CAP, bside);
      tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_DOUBLE_ATMOST);
    }
    else if (vUpper == 3)
    {
      tdata[0].set(BLANK_PLAYER_CAP, bside);
      tdata[1].setBlank(BLANK_LENGTH_VERB);
      tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_TRIPLE_ATMOST);
    }
    else
    {
      tdata[0].set(BLANK_PLAYER_CAP, bside);
      tdata[1].setBlank(BLANK_LENGTH_VERB);
      tdata[1].setData(BLANK_LENGTH_VERB_CARDS_ATMOST_PARAM, vUpper);
    }
  }
  else if (! abstractableFlag)
  {
    tdata[0].set(BLANK_PLAYER_CAP, bside);
    tdata[1].setBlank(BLANK_LENGTH_VERB);
    tdata[1].setData(BLANK_LENGTH_VERB_RANGE_PARAMS, vLower, vUpper);
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_NEITHER);
    tdata[1].setBlank(BLANK_LENGTH_VERB);
    tdata[1].setData(BLANK_LENGTH_VERB_VOID);
  }
  else if (vLower+1 == vUpper)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT);
    tdata[1].set(BLANK_LENGTH_VERB, BLANK_LENGTH_VERB_ODD_EVENLY);
  }
  else if (vLower + vUpper == oppsLength)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_EACH);
    tdata[1].setBlank(BLANK_LENGTH_VERB);
    tdata[1].setData(BLANK_LENGTH_VERB_RANGE_PARAMS, vLower, vUpper);
  }
  else
  {
    tdata[0].set(BLANK_PLAYER_CAP, bside);
    tdata[1].setBlank(BLANK_LENGTH_VERB);
    tdata[1].setData(BLANK_LENGTH_VERB_RANGE_PARAMS, vLower, vUpper);
  }
}


void VerbalCover::getLengthData(
  const unsigned char oppsLength,
  const VerbalSide& vside,
  const bool abstractableFlag,
  vector<TemplateData>& tdata) const
{
  // If abstractableFlag is set, we must state the sentence from
  // the intended side (and not e.g. "The suit splits 2=2" instead
  // of "West has a doubleton").

  if (lengthOper == COVER_EQUAL)
  {
    VerbalCover::getLengthEqualData(
      oppsLength, vside, abstractableFlag, tdata);
  }
  else if (lengthOper == COVER_INSIDE_RANGE ||
           lengthOper == COVER_LESS_EQUAL ||
           lengthOper == COVER_GREATER_EQUAL)
  {
    VerbalCover::getLengthInsideData(oppsLength, vside,
      abstractableFlag, tdata);
  }
  else
    assert(false);
}


void VerbalCover::fillLengthAdjElement(
  const unsigned char oppsLength,
  const Opponent simplestOpponent)
{
  templateFills.resize(3);
  TemplateData& telement = templateFills[2];

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
    if (vLower == 1)
      telement.set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_SINGLE);
    else if (vLower == 2)
      telement.set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_DOUBLE);
    else if (vLower == 3)
      telement.set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_TRIPLE);
    else
    {
      telement.setBlank(BLANK_LENGTH_ADJ);
      telement.setData(BLANK_LENGTH_ADJ_LONG, to_string(+vLower));
    }
  }
  else if (vLower == 0)
  {
    if (vUpper == 1)
      telement.set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_SINGLE_ATMOST);
    else if (vUpper == 2)
      telement.set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_DOUBLE_ATMOST);
    else if (vUpper == 3)
      telement.set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_TRIPLE_ATMOST);
    else
    {
      telement.setBlank(BLANK_LENGTH_ADJ);
      telement.setData(BLANK_LENGTH_ADJ_LONG_ATMOST, to_string(+vUpper));
    }
  }
  else if (vLower == 2 && vUpper == 3)
    telement.set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_23);
  else
  {
cout << "range " << +lengthLower << " to " << +lengthUpper << endl;
cout << "here  " << +vLower << " to " << +vUpper << endl;
cout << "oppsLength " << +oppsLength << endl;
    assert(false);
  }
}


void VerbalCover::fillBelow(
  const unsigned char freeLower,
  const unsigned char freeUpper,
  const unsigned char numBottoms,
  const RanksNames& ranksNames,
  const unsigned char rankNo,
  const VerbalSide& vside)
{
  sentence = SENTENCE_ONLY_BELOW;

  // Make a synthetic length of small cards.
  lengthFlag = true;
  lengthLower = freeLower;
  lengthUpper = freeUpper;
  if (freeLower == freeUpper)
    lengthOper = COVER_EQUAL;
  else if (freeLower == 0)
    lengthOper = COVER_LESS_EQUAL;
  else if (freeUpper == numBottoms)
    lengthOper = COVER_GREATER_EQUAL;
  else
    lengthOper = COVER_INSIDE_RANGE;

  // This sets numbers 0 and 1.
  // In a kludge, we first pretend that this is always from West,
  // and then we adjust to the actual side.  This is to prevent
  // getLengthData() from reversing the viewpoint.
  //                          TODO
  const VerbalSide vside2 = {OPP_WEST, false};
  VerbalCover::getLengthData(numBottoms, vside2, false, templateFills);
  
  // And here is the fix.
  const BlankPlayerCap bside = vside.blank();

  templateFills[0].set(BLANK_PLAYER_CAP, bside);

  templateFills.resize(4);

  if (freeUpper == 1)
    templateFills[2].set(BLANK_BELOW, BLANK_BELOW_NORMAL);
  else
    templateFills[2].set(BLANK_BELOW, BLANK_BELOW_COMPLETELY);

  // This should be expanded later in VerbalTemplates.
  templateFills[3].setBlank(BLANK_TOPS);
  templateFills[3].setData(BLANK_TOPS_ACTUAL,
    ranksNames.lowestCard(rankNo));
}



void VerbalCover::fillSingular(
  const unsigned char lenCompletion,
  const VerbalSide& vside)
{
  sentence = SENTENCE_TOPS_LENGTH;

  const BlankPlayerCap bside = vside.blank();

  templateFills.resize(3);

  templateFills[0].set(BLANK_PLAYER_CAP, bside);

  templateFills[1].setBlank(BLANK_TOPS);
  templateFills[1].setCompletion(BLANK_TOPS_ACTUAL, completion);

  if (lenCompletion == 1)
    templateFills[2].set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_SINGLE);
  else if (lenCompletion == 2)
    templateFills[2].set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_DOUBLE);
  else if (lenCompletion == 3)
    templateFills[2].set(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_TRIPLE);
  else
    templateFills[2].setData(BLANK_LENGTH_ADJ, BLANK_LENGTH_ADJ_LONG, 
      lenCompletion);
}


/*
void VerbalCover::getTopsData(
  const BlankPlayerCap side,
  const Completion& completionIn,
  const RanksNames& ranksNames,
  vector<TemplateData>& tdata) const
{
  tdata.resize(2);
  tdata[0].set(BLANK_PLAYER_CAP, side);
  tdata[1].setBlank(BLANK_TOPS_PHRASE);
  tdata[1].setData(BLANK_TOPS_PHRASE_HOLDING,
                                  // TODO !!!
    completionIn.strSet(ranksNames, OPP_WEST,
      false, false));

}
*/


void VerbalCover::fillCompletion(
  const VerbalSide& vside,
  const RanksNames& ranksNames)
{
  sentence = SENTENCE_LIST;
  const BlankPlayerCap bside = vside.blank();

  templateFills.resize(2);
  templateFills[0].set(BLANK_PLAYER_CAP, bside);

  templateFills[1].setBlank(BLANK_LIST_PHRASE);
  templateFills[1].setData(BLANK_LIST_PHRASE_HOLDING, 
    completion.strSetNew(ranksNames, vside.side, true, true));
}


void VerbalCover::fillCompletionWithLows(
  const VerbalSide& vside,
  const RanksNames& ranksNames)
{
  sentence = SENTENCE_LIST;
  const BlankPlayerCap bside = vside.blank();

  templateFills.resize(2);
  templateFills[0].set(BLANK_PLAYER_CAP, bside);

  const string s = completion.strSetNew(ranksNames, vside.side, false, true) +
    "(" + completion.strUnset(ranksNames, vside.side) + ")";

  templateFills[1].setBlank(BLANK_LIST_PHRASE);
  templateFills[1].setData(BLANK_LIST_PHRASE_HOLDING, s);
}


void VerbalCover::fillBottoms(
  const VerbalSide& vside,
  const RanksNames& ranksNames)
{
  sentence = SENTENCE_TOPS_AND_XES;
  const BlankPlayerCap bside = vside.blank();

  templateFills.resize(3);
  templateFills[0].set(BLANK_PLAYER_CAP, bside);

  const string s = completion.strSetNew(ranksNames, vside.side, false, true);
  templateFills[1].setBlank(BLANK_TOPS);
  templateFills[1].setData(BLANK_TOPS_ACTUAL, s);


  templateFills[2].setBlank(BLANK_BOTTOMS);
  templateFills[2].setData(BLANK_BOTTOMS_NORMAL, completion.strXes(vside.side));
}


void VerbalCover::fillTopsAndLower(
  const VerbalSide& vside,
  const RanksNames& ranksNames,
  const unsigned char numOptions,
  const VerbalData& data)
{
  sentence = SENTENCE_TOPS_AND_LOWER;
  const BlankPlayerCap bside = vside.blank();

  templateFills.resize(4);
  templateFills[0].set(BLANK_PLAYER_CAP, bside);

  const string s = completion.strSetNew(ranksNames, vside.side,
    true, true);
  templateFills[1].setBlank(BLANK_TOPS);
  templateFills[1].setData(BLANK_TOPS_ACTUAL, s);

  const unsigned char freeLower = completion.getFreeLower(vside.side);
  const unsigned char freeUpper = completion.getFreeUpper(vside.side);

  templateFills[2].setBlank(BLANK_COUNT);
  if (freeLower == freeUpper)
  {
    templateFills[2].setData(BLANK_COUNT_EQUAL, topCount[freeLower]);
  }
  else if (freeLower == 0)
  {
    templateFills[2].setData(BLANK_COUNT_ATMOST, topCount[freeUpper]);
  }
  else
  {
    templateFills[2].setData(BLANK_COUNT_RANGE_PARAMS, 
      to_string(+freeLower), to_string(+freeUpper));
  }

  string t;
  if (data.lowestRankActive == data.lowestRankUsed)
  {
    t = ", lower-ranked ";
    t += (freeUpper == 1 ? "card" : "cards");
  }
  else
  {
    t = " ";
    t += (freeUpper == 1 ? "card" : "cards");
    t += " below the " + ranksNames.lowestCard(numOptions);
  }

  templateFills[3].setBlank(BLANK_TOPS);
  templateFills[3].setData(BLANK_TOPS_ACTUAL, t);
}


void VerbalCover::fillList(
  const VerbalSide& vside,
  const RanksNames& ranksNames,
  const list<Completion>& completionsIn)
{
  sentence = SENTENCE_LIST;
  const BlankPlayerCap bside = vside.blank();

  templateFills.resize(completionsIn.size() + 1);
  templateFills[0].set(BLANK_PLAYER_CAP, bside);

  size_t i = 1;
  for (auto& completionIn: completionsIn)
  {
// cout << "i = " << i << ": " << completionIn.strDebug() << endl;
    templateFills[i].setBlank(BLANK_LIST_PHRASE);
    templateFills[i].setData(BLANK_LIST_PHRASE_HOLDING, 
                                    // TODO !!!
      completionIn.strSet(ranksNames, vside.side, false, false, true));
    i++;
  }
}


string VerbalCover::strGeneral(
  const unsigned char oppsLength,
  const bool symmFlag,
  const RanksNames& ranksNames,
  vector<TemplateData>& tdata) const
{
  assert(lengthFlag);

  string lstr = "", wstr = "", estr = "";

  const bool westFlag = (completion.length(OPP_WEST) > 0);
  const bool eastFlag = (completion.length(OPP_EAST) > 0);

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

  VerbalCover::getLengthData(oppsLength, vside, true, tdata);
    
  lstr = verbalTemplates.get(SENTENCE_LENGTH_ONLY, ranksNames, tdata);

  if (westFlag)
    wstr = completion.strSet(ranksNames, OPP_WEST, false, false);

  if (eastFlag)
    estr = completion.strSet(ranksNames, OPP_EAST, false, false);

  if (westFlag)
  {
    if (eastFlag)
    {
      if (symmFlag)
      {
        if (wstr == estr)
          return lstr + ", and West and East each have " + wstr;
        else
          return lstr + ", and " + wstr + " and " + estr +
            " are split";
      }
      else if (wstr == estr)
        return lstr + ", West and East each have " + wstr;
      else
        return lstr + ", West has " + wstr + " and East has " + estr;
    }
    else
    {
      if (symmFlag)
        return lstr + " with " + wstr;
      else
        return lstr + " and West has " + wstr;
    }
  }
  else if (eastFlag)
  {
    if (symmFlag)
      return lstr + " without " + estr;
    else
      return lstr + " and East has " + estr;
  }
  else
    // This done exclusively in the new way.
    return lstr;

  return "";
}


void VerbalCover::getOnetopElement(
  const unsigned char oppsValue1,
  const unsigned char oppsValue2,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  TemplateData& telement) const
{
  if (oppsValue1 == 0)
  {
    telement.setBlank(BLANK_TOPS);
    telement.setData(BLANK_TOPS_ONE_ATMOST, 
      oppsValue2, onetopIndex);
  }
  else if (oppsValue2 == oppsSize || oppsValue2 == 0xf)
  {
    telement.setBlank(BLANK_TOPS);
    telement.setData(BLANK_TOPS_ONE_ATLEAST, 
      oppsValue1, onetopIndex);
  }
  else if (oppsValue1 + oppsValue2 == oppsSize)
  {
    telement.setBlank(BLANK_TOPS);
    telement.setData(BLANK_TOPS_ONE_RANGE_PARAMS, 
      oppsValue1, 
      oppsValue2, 
      onetopIndex);
  }
  else
  {
    telement.setBlank(BLANK_TOPS);
    telement.setData(BLANK_TOPS_ONE_RANGE_PARAMS, 
      oppsValue1, 
      oppsValue2, 
      onetopIndex);
  }
}


// TODO Later on private and at the right place in the file again
void VerbalCover::getOnetopData(
  const unsigned char oppsValue1,
  const unsigned char oppsValue2,
  const unsigned char oppsSize,
  const unsigned char onetopIndex,
  const BlankPlayerCap side,
  vector<TemplateData>& tdata) const
{
  // Here lower and upper are different.
  tdata.resize(2);

  if (oppsValue1 == 0)
    tdata[0].set(BLANK_PLAYER_CAP, side);
  else if (oppsValue2 == oppsSize || oppsValue2 == 0xf)
    tdata[0].set(BLANK_PLAYER_CAP, side);
  else if (oppsValue1 + oppsValue2 == oppsSize)
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_EACH);
  else
    tdata[0].set(BLANK_PLAYER_CAP, side);

  VerbalCover::getOnetopElement(oppsValue1, oppsValue2, oppsSize,
    onetopIndex, tdata[1]);
}


string VerbalCover::str(const RanksNames& ranksNames) const
{
  return verbalTemplates.get(sentence, ranksNames, templateFills);
}

