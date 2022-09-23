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

// TODO Need whole file or just TemplateData?
#include "VerbalTemplates.h"

#include "../term/Length.h"

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



VerbalCover::VerbalCover()
{
  lengthFlag = false;
  lengthLower = 0;
  lengthUpper = 0;
  lengthOper = COVER_EQUAL;
  westFlag = false;
  eastFlag = false;
}


void VerbalCover::push_back(const Completion& completion)
{
  completions.push_back(completion);
}


void VerbalCover::setLength(const Length& length)
{
  lengthFlag = true;
  lengthLower = length.lower();
  lengthUpper = length.upper();
  lengthOper = length.getOperator();
}


Completion& VerbalCover::activateSide(const Opponent opponent)
{
  assert(opponent == OPP_WEST || opponent == OPP_EAST);
  if (opponent == OPP_WEST)
  {
    westFlag = true;
    return west;
  }
  else
  {
    eastFlag = true;
    return east;
  }
}


void VerbalCover::setSide(
  const Completion& completion,
  const Opponent opponent)
{
  assert(opponent == OPP_WEST || opponent == OPP_EAST);
  if (opponent == OPP_WEST)
  {
    westFlag = true;
    west = completion;
  }
  else
  {
    eastFlag = true;
    east = completion;
  }
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
  const Opponent simplestOpponent,
  const bool symmFlag,
  vector<TemplateData>& tdata) const
{
  // Here lower and upper are identical.
  BlankPlayerCap side;
  unsigned char value;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST);
    value = lengthLower;
  }
  else
  {
    side = (symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST);
    value = oppsLength - lengthLower;
  }

  tdata.resize(2);
  if (value == 0)
  {
    tdata[0].set(BLANK_PLAYER_CAP, side);
    tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_VOID);
  }
  else if (value == 1)
  {
    tdata[0].set(BLANK_PLAYER_CAP, side);
    tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_SINGLE);
  }
  else if (value == 2 && oppsLength > 4)
  {
    tdata[0].set(BLANK_PLAYER_CAP, side);
    tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_DOUBLE);
  }
  else if (value == 3 && oppsLength > 6)
  {
    tdata[0].set(BLANK_PLAYER_CAP, side);
    tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_TRIPLE);
  }
  else if (value + value == oppsLength)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT);
    tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_EVENLY);
  }
  else
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT);
    tdata[1].setBlank(BLANK_LENGTH_PHRASE);
    tdata[1].setData(BLANK_LENGTH_PHRASE_SPLIT_PARAMS,
      lengthLower, oppsLength - lengthLower);
  }
}


void VerbalCover::getLengthInsideData(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  const bool symmFlag,
  vector<TemplateData>& tdata) const
{
  BlankPlayerCap side;
  unsigned char vLower, vUpper;

  if (simplestOpponent == OPP_WEST)
  {
    side = (symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST);
    vLower = lengthLower;
    vUpper = (lengthOper == COVER_GREATER_EQUAL ?
      oppsLength : lengthUpper);
  }
  else
  {
    side = (symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST);
    vLower = (lengthOper == COVER_GREATER_EQUAL ?
      0 : oppsLength - lengthUpper);
    vUpper = oppsLength - lengthLower;
  }

  tdata.resize(2);
  if (vLower == 0)
  {
    if (vUpper == 1)
    {
      tdata[0].set(BLANK_PLAYER_CAP, side);
      tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_SINGLE_ATMOST);
    }
    else if (vUpper == 2)
    {
      tdata[0].set(BLANK_PLAYER_CAP, side);
      tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_DOUBLE_ATMOST);
    }
    else if (vUpper == 3)
    {
      tdata[0].set(BLANK_PLAYER_CAP, side);
      tdata[1].setBlank(BLANK_LENGTH_PHRASE);
      tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_TRIPLE_ATMOST);
    }
    else
    {
      tdata[0].set(BLANK_PLAYER_CAP, side);
      tdata[1].setBlank(BLANK_LENGTH_PHRASE);
      tdata[1].setData(BLANK_LENGTH_PHRASE_CARDS_ATMOST_PARAM, vUpper);
    }
  }
  else if (vLower == 1 && vUpper+1 == oppsLength)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_NEITHER);
    tdata[1].setBlank(BLANK_LENGTH_PHRASE);
    tdata[1].setData(BLANK_LENGTH_PHRASE_VOID);
  }
  else if (vLower+1 == vUpper)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_SUIT);
    tdata[1].set(BLANK_LENGTH_PHRASE, BLANK_LENGTH_PHRASE_ODD_EVENLY);
  }
  else if (vLower + vUpper == oppsLength)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_EACH);
    tdata[1].setBlank(BLANK_LENGTH_PHRASE);
    tdata[1].setData(BLANK_LENGTH_PHRASE_RANGE_PARAMS, vLower, vUpper);
  }
  else
  {
    tdata[0].set(BLANK_PLAYER_CAP, side);
    tdata[1].setBlank(BLANK_LENGTH_PHRASE);
    tdata[1].setData(BLANK_LENGTH_PHRASE_RANGE_PARAMS, vLower, vUpper);
  }
}


void VerbalCover::getLengthData(
  const unsigned char oppsLength,
  const Opponent simplestOpponent,
  const bool symmFlag,
  vector<TemplateData>& tdata) const
{
  if (lengthOper == COVER_EQUAL)
  {
    VerbalCover::getLengthEqualData(
      oppsLength, simplestOpponent, symmFlag, tdata);
  }
  else if (lengthOper == COVER_INSIDE_RANGE ||
           lengthOper == COVER_LESS_EQUAL ||
           lengthOper == COVER_GREATER_EQUAL)
  {
    VerbalCover::getLengthInsideData(oppsLength, simplestOpponent, symmFlag,
      tdata);
  }
  else
    assert(false);
}


void VerbalCover::getTopsData(
  const BlankPlayerCap side,
  const Completion& completion,
  const RanksNames& ranksNames,
  vector<TemplateData>& tdata) const
{
  tdata.resize(2);
  tdata[0].set(BLANK_PLAYER_CAP, side);
  tdata[1].setBlank(BLANK_TOPS_PHRASE);
  tdata[1].setData(BLANK_TOPS_PHRASE_HOLDING, 
    completion.strSet(ranksNames, false, false));

}


void VerbalCover::makeList(
  const BlankPlayerCap side,
  const RanksNames& ranksNames,
  vector<TemplateData>& tdata) const
{
  tdata.resize(completions.size() + 1);
  tdata[0].set(BLANK_PLAYER_CAP, side);

  size_t i = 1;
  for (auto& completion: completions)
  {
    tdata[i].setBlank(BLANK_LIST_PHRASE);
    tdata[i].setData(BLANK_LIST_PHRASE_HOLDING, 
      completion.strSet(ranksNames, false, false, true));
    i++;
  }
}


string VerbalCover::strGeneral(
  const unsigned char oppsLength,
  const bool symmFlag,
  const RanksNames& ranksNames,
  vector<TemplateData>& tdata) const
{
  string lstr = "", wstr = "", estr = "";
  if (lengthFlag)
  {
    Opponent simplestOpponent;
    if (symmFlag)
      simplestOpponent = OPP_WEST;
    else if (westFlag == eastFlag)
      simplestOpponent = VerbalCover::simplestOpponent(oppsLength);
    else if (westFlag)
      simplestOpponent = OPP_WEST;
    else
      simplestOpponent = OPP_EAST;

    VerbalCover::getLengthData(oppsLength, simplestOpponent, symmFlag, 
      tdata);
    
    lstr = verbalTemplates.get(TEMPLATES_LENGTH_ONLY, tdata);
  }

  if (westFlag)
    wstr = west.strSet(ranksNames, false, false);

  if (eastFlag)
    estr = east.strSet(ranksNames, false, false);

  if (lengthFlag)
  {
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
  }
  else if (westFlag)
  {
    if (eastFlag)
    {
      // This branch currently doesn't happen?

      assert(wstr == estr);
      string sold = "West and East each have " + wstr;

      VerbalCover::getTopsData(BLANK_PLAYER_CAP_EACH, west, ranksNames, 
        tdata);
      string snew = verbalTemplates.get(TEMPLATES_TOPS_ONLY, tdata);

      if (sold == snew)
        cout << setw(40) << left << sold << "W1W " << snew << "\n";
      else
        cout << setw(40) << left << sold << "W2W " << snew << "\n";

      return sold;
    }
    else
    {
      VerbalCover::getTopsData(
        (symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_WEST), 
        west, 
        ranksNames, 
        tdata);
      return verbalTemplates.get(TEMPLATES_TOPS_ONLY, tdata);
    }
  }
  else if (eastFlag)
  {
    // This branch currently doesn't happen?

    VerbalCover::getTopsData(
      (symmFlag ? BLANK_PLAYER_CAP_EITHER : BLANK_PLAYER_CAP_EAST), 
      east, 
      ranksNames, 
      tdata);
    const string estrNew = verbalTemplates.get(
      TEMPLATES_TOPS_ONLY, tdata);

    string s;
    if (symmFlag)
      s = "Either side has " + estr;
    else
      s = "West has " + estr;

      if (s == estrNew)
        cout << "\n" << setw(30) << left << s << "Z3Z " << estrNew << "\n";
      else
        cout << "\n" << setw(30) << left << s << "Z4Z " << estrNew << "\n";

    return s;
  }
  else
    assert(false);

  return "";
}


// TODO Later on private and at the right place in the file again
void VerbalCover::getOnetopData(
  const unsigned char oppsValue1,
  const unsigned char oppsValue2,
  const unsigned char oppsSize,
  const string& choice,
  const BlankPlayerCap side,
  vector<TemplateData>& tdata) const
{
  // Here lower and upper are different.
  tdata.resize(2);

  // const string choice = completion.strSet(ranksNames, false, true);
  // cout << "choice " << choice << endl;

  if (oppsValue1 == 0)
  {
    tdata[0].set(BLANK_PLAYER_CAP, side);
    tdata[1].setBlank(BLANK_ONETOP_PHRASE);
    tdata[1].setData(BLANK_ONETOP_PHRASE_HAS_ATMOST, 
      topCount[oppsValue2], choice);
  }
  else if (oppsValue2 == oppsSize || oppsValue2 == 0xf)
  {
    tdata[0].set(BLANK_PLAYER_CAP, side);
    tdata[1].setBlank(BLANK_ONETOP_PHRASE);
    tdata[1].setData(BLANK_ONETOP_PHRASE_HAS_ATLEAST, 
      topCount[oppsValue1], choice);
  }
  else if (oppsValue1 + oppsValue2 == oppsSize)
  {
    tdata[0].set(BLANK_PLAYER_CAP, BLANK_PLAYER_CAP_EACH);
    tdata[1].setBlank(BLANK_ONETOP_PHRASE);
    tdata[1].setData(BLANK_ONETOP_PHRASE_RANGE_PARAMS, 
      to_string(+oppsValue1), 
      to_string(+oppsValue2), 
      choice);
  }
  else
  {
    tdata[0].set(BLANK_PLAYER_CAP, side);
    tdata[1].setBlank(BLANK_ONETOP_PHRASE);
    tdata[1].setData(BLANK_ONETOP_PHRASE_RANGE_PARAMS, 
      to_string(+oppsValue1), 
      to_string(+oppsValue2), 
      choice);
  }
}


