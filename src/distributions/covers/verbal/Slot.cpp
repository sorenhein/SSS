/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
// #include <sstream>
#include <cassert>

#include "Slot.h"
#include "Completion.h"

#include "../../../languages/Dictionary.h"
#include "../../../languages/connections/words.h"

#include "../../../languages/PhraseExpansion.h"

#include "../../../ranks/RanksNames.h"

extern Dictionary dictionary;


 /*
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

 const vector<string> cardStrings =
 {
   "card",
   "cards"
 };
 */



Slot::Slot()
{
  numOpp = 0;
  numUchars = 0;
  numBools = 0;

  side = static_cast<Opponent>(0);
  uchars.clear();
  bools.clear();
}


void Slot::setPhrase(const VerbalPhrase phraseIn)
{
  phrase = phraseIn;
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


/*
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
*/


VerbalPhrase Slot::getPhrase() const
{
  return phrase;
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
  const PhraseExpansion expansion,
  const string& text,
  const RanksNames& ranksNames,
  const Completion& completion) const
{
  string s = text;

  if (expansion == PHRASE_NONE)
  {
    assert(Slot::has(0, 0, 0));
    return s;
  }
  else if (expansion == PHRASE_NUMERICAL)
  {
    assert(Slot::has(0, 1, 0) || Slot::has(0, 2, 0));

    for (unsigned char field = 0; field < numUchars; field++)
      Slot::replace(s, field, dictionary.numerals.get(uchars[field]).text);
      // Slot::replace(s, field, topCount[uchars[field]]);

    return s;
  }
  else if (expansion == PHRASE_ORDINAL)
  {
    assert(Slot::has(0, 1, 0) || Slot::has(0, 2, 0));

    for (unsigned char field = 0; field < numUchars; field++)
      Slot::replace(s, field, dictionary.ordinals.get(uchars[field]).text);
      // Slot::replace(s, field, topOrdinal[uchars[field]]);

    return s;
  }
  else if (expansion == PHRASE_RANKS)
  {
    assert(Slot::has(0, 1, 0));
    Slot::replace(s, "%0", ranksNames.lowestCard(uchars[0]));
    return s;
  }
  else if (expansion == PHRASE_TEXT_LOWER)
  {
    assert(Slot::has(0, 1, 0));
    Slot::replace(s, "%0", dictionary.words.get(
      uchars[0] == 1 ? WORDS_CARD : WORDS_CARDS ).text);
    // Slot::replace(s, "%0", cardStrings[uchars[0] == 1 ? 0 : 1]);
    return s;
  }
  else if (expansion == PHRASE_TEXT_BELOW)
  {
    // If we permit full card names here, they should be in dative
    // (in German, and it doesn't hurt in English and Danish).
    // I don't have a good general mechanism for this.
    assert(Slot::has(0, 2, 0));
    // Slot::replace(s, "%0", cardStrings[uchars[0] == 1 ? 0 : 1]);
    Slot::replace(s, "%0", dictionary.words.get(
      uchars[0] == 1 ? WORDS_CARD : WORDS_CARDS ).text);
    Slot::replace(s, "%1", ranksNames.lowestCard(uchars[1]));
    return s;
  }
  else if (expansion == PHRASE_RANGE_OF)
  {
    // Dative.
    assert(Slot::has(0, 3, 0));
    Slot::replace(s, "%0", uchars[0]);
    Slot::replace(s, "%1", uchars[1]);
    Slot::replace(s, "%2", ranksNames.getOpponents(uchars[2]).
      strComponent(RANKNAME_ACTUAL_FULL));
    return s;
  }
  else if (expansion == PHRASE_SOME_OF)
  {
    // Dative.
    assert(Slot::has(0, 2, 0));
    // Slot::replace(s, "%0", topCount[uchars[0]]);
    Slot::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    Slot::replace(s, "%1", ranksNames.getOpponents(uchars[1]).
      strComponent(RANKNAME_ACTUAL_FULL));
    return s;
  }
  else if (expansion == PHRASE_SOME_RANK_SET)
  {
    assert(Slot::has(0, 2, 0));
    // Slot::replace(s, "%0", topCount[uchars[0]]);
    Slot::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    Slot::replace(s, "%1", ranksNames.getOpponents(uchars[1]).
      strComponent(RANKNAME_ACTUAL_FULL));
    return s;
  }
  else if (expansion == PHRASE_FULL_RANK_SET)
  {
    assert(Slot::has(0, 1, 0));
    Slot::replace(s, "%0", ranksNames.getOpponents(uchars[0]).
      strComponent(RANKNAME_ACTUAL_FULL));
    return s;
  }
  else if (expansion == PHRASE_COMPLETION_SET)
  {
    assert(Slot::has(1, 0, 0) || Slot::has(1, 0, 1));

    if (numBools == 0)
      Slot::replace(s, "%0", completion.strSet(ranksNames, side));
    else
      Slot::replace(s, "%0", completion.strSet(ranksNames, side, bools[0]));

    return s;
  }
  else if (expansion == PHRASE_COMPLETION_BOTH)
  {
    assert(Slot::has(1, 0, 1));

    Slot::replace(s, "%0", completion.strSet(ranksNames, side, bools[0]));

    Slot::replace(s, "%1", completion.strUnset(ranksNames, side));

    return s;
  }
  else if (expansion == PHRASE_COMPLETION_XES)
  {
    assert(Slot::has(1, 0, 0));
    Slot::replace(s, "%0", completion.strXes(side));
    return s;
  }
  else
  {
    assert(false);
    return "";
  }
}
