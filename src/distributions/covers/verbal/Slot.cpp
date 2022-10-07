/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "Slot.h"
#include "Completion.h"
#include "VerbalBlank.h"

#include "../../../ranks/RanksNames.h"

// #include "../../../utils/table.h"


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

 const vector<string> plural =
 {
   "",
   "s"
 };



Slot::Slot()
{
  phraseCategory = PHRASE_SIZE;
  phraseInstance = BLANK_MAX_VERSIONS;
  expansion = SLOT_SIZE;

  numOpp = 0;
  numUchars = 0;
  numBools = 0;

  uchars.clear();
  bools.clear();
}


void Slot::setSemantics(
  const PhraseCategory phraseCategoryIn,
  const unsigned phraseInstanceIn,
  const SlotExpansion expansionIn)
{
  phraseCategory = phraseCategoryIn;
  phraseInstance = phraseInstanceIn;
  expansion = expansionIn;
}


void Slot::setSide(const Opponent sideIn)
{
  numOpp = 1;
  side = sideIn;
}


void Slot::setValues(const unsigned char value1)
{
  numUchars = 1;
  uchars.resize(1);
  uchars[0] = value1;
}


void Slot::setValues(
  const unsigned char value1,
  const unsigned char value2)
{
  numUchars = 2;
  uchars.resize(2);
  uchars[0] = value1;
  uchars[1] = value2;
}


void Slot::setValues(
  const unsigned char value1,
  const unsigned char value2,
  const unsigned char value3)
{
  numUchars = 3;
  uchars.resize(3);
  uchars[0] = value1;
  uchars[1] = value2;
  uchars[2] = value3;
}


void Slot::setBools(const bool bool1)
{
  numBools = 1;
  bools.resize(1);
  bools[0] = bool1;
}


void Slot::setBools(
  const bool bool1,
  const bool bool2)
{
  numBools = 2;
  bools.resize(2);
  bools[0] = bool1;
  bools[1] = bool2;
}


void Slot::setBools(
  const bool bool1,
  const bool bool2,
  const bool bool3)
{
  numBools = 3;
  bools.resize(3);
  bools[0] = bool1;
  bools[1] = bool2;
  bools[2] = bool3;
}


PhraseCategory Slot::phrase() const
{
  return phraseCategory;
}


bool Slot::has(
  const unsigned char actOpp,
  const unsigned char actUchars,
  const unsigned char actBools) const
{
  return (numOpp == actOpp &&
    numUchars == actUchars &&
    numBools == actBools);
}


void Slot::replace(
  string& s,
  const string& percent,
  const string& repl) const
{
  auto p = s.find(percent);
  assert(p != string::npos);
  s.replace(p, 2, repl);
}


void Slot::replace(
  string& s,
  const string& percent,
  const unsigned char uchar) const
{
  Slot::replace(s, percent, to_string(uchar));
}


void Slot::replace(
  string& s,
  const unsigned char field,
  const string& repl) const
{
  Slot::replace(s, "%" + to_string(field), repl);
}


void Slot::replace(
  string& s,
  const unsigned char field,
  const unsigned char uchar) const
{
  Slot::replace(s, "%" + to_string(field), to_string(uchar));
}


string Slot::str(
  const vector<vector<string>>& dictionary,
  const RanksNames& ranksNames,
  const Completion& completion) const
{
  assert(phraseCategory < dictionary.size());
  assert(phraseInstance < dictionary[phraseCategory].size());

  string s = dictionary[phraseCategory][phraseInstance];

  if (expansion == SLOT_NONE)
  {
    assert(Slot::has(0, 0, 0));
    // assert(numOpp == 0);
    // assert(numUchars == 0);
    // assert(numBools == 0);
    return s;
  }
  else if (expansion == SLOT_NUMERICAL)
  {
    assert(Slot::has(0, 1, 0) || Slot::has(0, 2, 0));
    // assert(numOpp == 0);
    // assert(numUchars == 1 || numUchars == 2);
    // assert(numBools == 0);

    if (numUchars == 1)
    {
      Slot::replace(s, "%0", topCount[uchars[0]]);
      // auto p = s.find("%0");
      // assert(p != string::npos);
      // s.replace(p, 2, topCount[uchars[0]]);
    }
    else
    {
      for (unsigned char field = 0; field < numUchars; field++)
      {
        Slot::replace(s, field, uchars[field]);
        // auto p = s.find("%" + to_string(field));
        // assert(p != string::npos);
        // s.replace(p, 2, to_string(uchars[field]));
      }
    }
    return s;
  }
  else if (expansion == SLOT_ORDINAL)
  {
    assert(Slot::has(0, 1, 0));
    // assert(numOpp == 0);
    // assert(numUchars == 1);
    // assert(numBools == 0);

    Slot::replace(s, "%0", topOrdinal[uchars[0]]);
    // auto p = s.find("%0");
    // assert(p != string::npos);
    // s.replace(p, 2, topOrdinal[uchars[0]]);
    return s;
  }
  else if (expansion == SLOT_TEXT_LOWER)
  {
    assert(Slot::has(0, 1, 0));
    // assert(numUchars == 1);
    // assert(numBools == 0);
    
    Slot::replace(s, "%0", 
      plural[uchars[0] == 1 ? 0 : 1]);
    // auto p = s.find("%0");
    // assert(p != string::npos);
    // s.replace(p, 2, plural[uchars[0] == 1 ? 0 : 1]);
    return s;
  }
  else if (expansion == SLOT_TEXT_BELOW)
  {
    assert(Slot::has(0, 2, 0));
    // assert(numOpp == 0);
    // assert(numUchars == 2);
    // assert(numBools == 0);
    
    Slot::replace(s, "%0", plural[uchars[0] == 1 ? 0 : 1]);
    // auto p = s.find("%0");
    // assert(p != string::npos);
    // s.replace(p, 2, plural[uchars[0] == 1 ? 0 : 1]);

    Slot::replace(s, "%1", ranksNames.lowestCard(uchars[1]));
    // p = s.find("%1");
    // assert(p != string::npos);
    // s.replace(p, 2, ranksNames.lowestCard(uchars[1]));

    return s;
  }
  else if (expansion == SLOT_COMPLETION_SET)
  {
    assert(Slot::has(1, 0, 2) || Slot::has(1, 0, 3));
    // assert(numOpp == 1);
    // assert(numUchars == 0);
    // assert(numBools == 2 || numBools == 3);

    auto p = s.find("%0");
    assert(p != string::npos);
    if (numBools == 2)
      Slot::replace(s, "%0",
        completion.strSetNew(ranksNames, side, bools[0], bools[1]));
      // s.replace(p, 2, completion.strSetNew(ranksNames,
        // side, bools[0], bools[1]));
    else
      Slot::replace(s, "%0",
        completion.strSetNew(ranksNames, side, bools[0], bools[1], bools[2]));
      // s.replace(p, 2, completion.strSetNew(ranksNames,
        // side, bools[0], bools[1], bools[2]));

    return s;
  }
  else if (expansion == SLOT_COMPLETION_BOTH)
  {
    assert(Slot::has(1, 0, 2));
    // assert(numOpp == 1);
    // assert(numUchars == 0);
    // assert(numBools == 2);

    Slot::replace(s, "%0",
      completion.strSetNew(ranksNames, side, bools[0], bools[1]));
    // auto p = s.find("%0");
    // assert(p != string::npos);
    // s.replace(p, 2, completion.strSetNew(ranksNames,
      // side, bools[0], bools[1]));

    Slot::replace(s, "%1", completion.strUnset(ranksNames, side));
    // p = s.find("%1");
    // assert(p != string::npos);
    // s.replace(p, 2, completion.strUnset(ranksNames, side));

    return s;
  }
  else if (expansion == SLOT_COMPLETION_XES)
  {
    assert(Slot::has(1, 0, 0));
    // assert(numOpp == 1);
    // assert(numUchars == 0);
    // assert(numBools == 0);

    Slot::replace(s, "%0", completion.strXes(side));
    // auto p = s.find("%0");
    // assert(p != string::npos);
    // s.replace(p, 2, completion.strXes(side));
    return s;
  }
  else if (expansion == SLOT_RANGE_OF)
  {
    assert(Slot::has(0, 3, 0));
    // assert(numOpp == 0);
    // assert(numUchars == 3);
    // assert(numBools == 0);

    Slot::replace(s, "%0", uchars[0]);
    // auto p = s.find("%0");
    // assert(p != string::npos);
    // s.replace(p, 2, to_string(uchars[0]));

    Slot::replace(s, "%1", uchars[1]);
    // p = s.find("%1");
    // assert(p != string::npos);
    // s.replace(p, 2, to_string(uchars[1]));

    Slot::replace(s, "%2", 
      ranksNames.getOpponents(uchars[2]).strComponent(RANKNAME_ACTUAL_FULL));
    // p = s.find("%2");
    // assert(p != string::npos);
    // s.replace(p, 2, ranksNames.getOpponents(uchars[2]).strComponent(RANKNAME_ACTUAL_FULL));

    return s;
  }
  else if (expansion == SLOT_RANKS)
  {
    assert(Slot::has(0, 1, 0));
    // assert(numOpp == 0);
    // assert(numUchars == 1);
    // assert(numBools == 0);

    Slot::replace(s, "%0", ranksNames.lowestCard(uchars[0]));
    // auto p = s.find("%0");
    // assert(p != string::npos);
    // s.replace(p, 2, ranksNames.lowestCard(uchars[0]));
    return s;
  }
  else if (expansion == SLOT_SOME_OF)
  {
    assert(Slot::has(0, 2, 0));
    // assert(numOpp == 0);
    // assert(numUchars == 2);
    // assert(numBools == 0);

    Slot::replace(s, "%0", topCount[uchars[0]]);
    // auto p = s.find("%0");
    // assert(p != string::npos);
    // s.replace(p, 2, topCount[uchars[0]]);

    Slot::replace(s, "%1", 
      ranksNames.getOpponents(uchars[1]).strComponent(RANKNAME_ACTUAL_FULL));
    // p = s.find("%1");
    // assert(p != string::npos);
    // s.replace(p, 2, ranksNames.getOpponents(uchars[1]).strComponent(RANKNAME_ACTUAL_FULL));

    return s;
  }
  else
  {
    assert(false);
    return "";
  }
}

