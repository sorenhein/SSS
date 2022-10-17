/*
   SSS, a bridge single-suit single-dummy solver.

   Copyright (C) 2020-2022 by Soren Hein.

   See LICENSE and README.
*/

#include <iostream>
#include <iomanip>
#include <cassert>

#include "Phrase.h"
#include "Completion.h"

#include "../../../languages/Dictionary.h"
#include "../../../languages/connections/words.h"

#include "../../../languages/PhraseExpansion.h"

#include "../../../ranks/RanksNames.h"

#include "../../../utils/table.h"

extern Dictionary dictionary;


// Methods for each phrase type.


typedef string (Phrase::*PhraseMethod)(
  const string& text,
  const RanksNames& ranksNames,
  const Completion& completion) const;

static const vector<PhraseMethod> phraseMethods =
{
  &Phrase::strNone,            // PHRASE_NONE
  &Phrase::strDigits,          // PHRASE_DIGIT
  &Phrase::strNumerical,       // PHRASE_NUMERICAL
  &Phrase::strOrdinal          // PHRASE_ORDINAL
};


Phrase::Phrase()
{
  numOpp = 0;
  numUchars = 0;
  numBools = 0;

  side = static_cast<Opponent>(0);
  uchars.clear();
  bools.clear();
}


void Phrase::setPhrase(const PhrasesEnum phraseIn)
{
  phrase = phraseIn;
}


void Phrase::setSide(const Opponent sideIn)
{
  numOpp = 1;
  side = sideIn;
}


void Phrase::setValues(const unsigned char value1)
{
  numUchars = 1;
  uchars.resize(1);
  uchars[0] = value1;
}


void Phrase::setValues(
  const unsigned char value1,
  const unsigned char value2)
{
  numUchars = 2;
  uchars.resize(2);
  uchars[0] = value1;
  uchars[1] = value2;
}


void Phrase::setValues(
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


void Phrase::setBools(const bool bool1)
{
  numBools = 1;
  bools.resize(1);
  bools[0] = bool1;
}


void Phrase::setBools(
  const bool bool1,
  const bool bool2)
{
  numBools = 2;
  bools.resize(2);
  bools[0] = bool1;
  bools[1] = bool2;
}


PhrasesEnum Phrase::getPhrase() const
{
  return phrase;
}


bool Phrase::has(
  const unsigned char actOpp,
  const unsigned char actUchars,
  const unsigned char actBools) const
{
  return (numOpp == actOpp &&
    numUchars == actUchars &&
    numBools == actBools);
}


void Phrase::replace(
  string& s,
  const string& percent,
  const string& repl) const
{
  auto p = s.find(percent);
  assert(p != string::npos);
  s.replace(p, 2, repl);
}


void Phrase::replace(
  string& s,
  const string& percent,
  const unsigned char uchar) const
{
  Phrase::replace(s, percent, to_string(uchar));
}


void Phrase::replace(
  string& s,
  const unsigned char field,
  const string& repl) const
{
  Phrase::replace(s, "%" + to_string(field), repl);
}


void Phrase::replace(
  string& s,
  const unsigned char field,
  const unsigned char uchar) const
{
  Phrase::replace(s, "%" + to_string(field), to_string(uchar));
}


string Phrase::strNone(
  const string& text,
  [[maybe_unused]] const RanksNames& ranksNames,
  [[maybe_unused]] const Completion& completion) const
{
  assert(Phrase::has(0, 0, 0));
  return text;
}


string Phrase::strDigits(
  const string& text,
  [[maybe_unused]] const RanksNames& ranksNames,
  [[maybe_unused]] const Completion& completion) const
{
  assert(Phrase::has(0, 1, 0) || Phrase::has(0, 2, 0));

  string s = text;
  for (unsigned char field = 0; field < numUchars; field++)
    Phrase::replace(s, field, uchars[field]);
  return s;
}


string Phrase::strNumerical(
  const string& text,
  [[maybe_unused]] const RanksNames& ranksNames,
  [[maybe_unused]] const Completion& completion) const
{
  assert(Phrase::has(0, 1, 0) || Phrase::has(0, 2, 0));

  string s = text;
  for (unsigned char field = 0; field < numUchars; field++)
    Phrase::replace(s, field, 
      dictionary.numerals.get(uchars[field]).text);
  return s;
}


string Phrase::strOrdinal(
  const string& text,
  [[maybe_unused]] const RanksNames& ranksNames,
  [[maybe_unused]] const Completion& completion) const
{
  assert(Phrase::has(0, 1, 0) || Phrase::has(0, 2, 0));

  string s = text;
  for (unsigned char field = 0; field < numUchars; field++)
    Phrase::replace(s, field, 
      dictionary.ordinals.get(uchars[field]).text);
  return s;
}


string Phrase::str(
  const PhraseExpansion expansion,
  const string& text,
  const RanksNames& ranksNames,
  const Completion& completion) const
{
  string s = text;

  if (expansion == PHRASE_NONE)
  {
    assert(Phrase::has(0, 0, 0));

    // return Phrase::strNone(text, ranksNames, completion);
  }
  else if (expansion == PHRASE_DIGITS)
  {
    assert(Phrase::has(0, 1, 0) || Phrase::has(0, 2, 0));

    for (unsigned char field = 0; field < numUchars; field++)
      Phrase::replace(s, field, uchars[field]);
    
    // return Phrase::strDigits(text, ranksNames, completion);
  }
  else if (expansion == PHRASE_NUMERICAL)
  {
    assert(Phrase::has(0, 1, 0) || Phrase::has(0, 2, 0));

    for (unsigned char field = 0; field < numUchars; field++)
      Phrase::replace(s, field, 
        dictionary.numerals.get(uchars[field]).text);

    // return Phrase::strNumerical(text, ranksNames, completion);
  }
  else if (expansion == PHRASE_ORDINAL)
  {
    assert(Phrase::has(0, 1, 0) || Phrase::has(0, 2, 0));

    for (unsigned char field = 0; field < numUchars; field++)
      Phrase::replace(s, field, 
        dictionary.ordinals.get(uchars[field]).text);

    // return Phrase::strOrdinal(text, ranksNames, completion);
  }
  else if (expansion == PHRASE_RANKS)
  {
    assert(Phrase::has(0, 1, 0));
    Phrase::replace(s, "%0", ranksNames.lowestCard(uchars[0]));
  }
  else if (expansion == PHRASE_TEXT_LOWER)
  {
    assert(Phrase::has(0, 1, 0));
    Phrase::replace(s, "%0", dictionary.words.get(
      uchars[0] == 1 ? WORDS_CARD : WORDS_CARDS).text);
  }
  else if (expansion == PHRASE_TEXT_BELOW)
  {
    // If we permit full card names here, they should be in dative
    // (in German, and it doesn't hurt in English and Danish).
    // I don't have a good general mechanism for this.
    assert(Phrase::has(0, 2, 0));
    Phrase::replace(s, "%0", dictionary.words.get(
      uchars[0] == 1 ? WORDS_CARD : WORDS_CARDS).text);
    Phrase::replace(s, "%1", ranksNames.lowestCard(uchars[1]));
  }
  else if (expansion == PHRASE_ADJACENT)
  {
    // TODO Call is PHRASE_COMPONENT_DEF_OF
    // Dative.
    assert(Phrase::has(0, 3, 0));
    Phrase::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    Phrase::replace(s, "%1", dictionary.numerals.get(uchars[1]).text);
    Phrase::replace(s, "%2", ranksNames.strComponent(
      RANKNAME_ACTUAL_FULL_DEF_OF, uchars[2], uchars[1] > 1));
  }
  else if (expansion == PHRASE_RANGE_OF)
  {
    // Dative.
    assert(Phrase::has(0, 1, 1));
    // Phrase::replace(s, "%0", uchars[0]);
    // Phrase::replace(s, "%1", uchars[1]);
    Phrase::replace(s, "%0", ranksNames.strComponent(
      RANKNAME_ACTUAL_FULL_DEF_OF, uchars[0], bools[0]));
  }
  else if (expansion == PHRASE_SOME_OF)
  {
    // Dative.
    assert(Phrase::has(0, 2, 0));
    Phrase::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    Phrase::replace(s, "%1", ranksNames.strComponent(
      RANKNAME_ACTUAL_FULL_DEF_OF, uchars[1], uchars[0] > 1));
  }
  else if (expansion == PHRASE_SOME_RANK_SET)
  {
    assert(Phrase::has(0, 2, 0));
    Phrase::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    Phrase::replace(s, "%1", ranksNames.strComponent(
      RANKNAME_ACTUAL_FULL_DEF_OF, uchars[1], uchars[0] > 1));
  }
  else if (expansion == PHRASE_FULL_RANK_SET)
  {
    assert(Phrase::has(0, 1, 0));
    Phrase::replace(s, "%0", ranksNames.getOpponents(uchars[0]).
      strComponent(RANKNAME_ACTUAL_FULL_DEF));
  }
  else if (expansion == PHRASE_COMPLETION_SET)
  {
    assert(Phrase::has(1, 0, 0) || Phrase::has(1, 0, 1));

    if (numBools == 0)
      Phrase::replace(s, "%0", completion.strSet(ranksNames, side));
    else
      Phrase::replace(s, "%0", completion.strSet(ranksNames, side, bools[0]));
  }
  else if (expansion == PHRASE_COMPLETION_BOTH)
  {
    assert(Phrase::has(1, 0, 1));

    Phrase::replace(s, "%0", completion.strSet(ranksNames, side, bools[0]));

    Phrase::replace(s, "%1", completion.strUnset(ranksNames, side));
  }
  else if (expansion == PHRASE_BOTH)
  {
    if (bools[1]) // expandable
    {
      assert(Phrase::has(0, 2, 2));

      Phrase::replace(s, "%0", ranksNames.getOpponents(uchars[0]).
        strComponent(RANKNAME_ACTUAL_FULL_INDEF));
      Phrase::replace(s, "%1", ranksNames.getOpponents(uchars[1]).
        strComponent(RANKNAME_ACTUAL_FULL_INDEF));
    }
    else if (bools[0]) // symmFlag
    {
      assert(Phrase::has(1, 0, 2));

      const Opponent sideOther = 
        (side == OPP_WEST ? OPP_EAST : OPP_WEST);
      Phrase::replace(s, "%0", completion.strSet(ranksNames, side));
      Phrase::replace(s, "%1", completion.strSet(ranksNames, sideOther));
    }
    else // Asymmetric
    {
      assert(Phrase::has(0, 0, 2));

      // Always state West first.
      Phrase::replace(s, "%0", completion.strSet(ranksNames, OPP_WEST));
      Phrase::replace(s, "%1", completion.strSet(ranksNames, OPP_EAST));
    }
  }
  else if (expansion == PHRASE_BOTH_ENTRY)
  {
    if (bools[0]) // expandable)
    {
      assert(Phrase::has(0, 3, 1));

      Phrase::replace(s, "%0", ranksNames.getOpponents(uchars[0]).
        strComponent(RANKNAME_ACTUAL_FULL_INDEF));

      Phrase::replace(s, "%1", to_string(+uchars[1]));
      Phrase::replace(s, "%2", to_string(+uchars[2]));
    }
    else
    {
      // Symmetry is handled elsewhere.
      assert(Phrase::has(1, 2, 1));

      Phrase::replace(s, "%0", completion.strSet(ranksNames, side));

      Phrase::replace(s, "%1", to_string(+uchars[0]));
      Phrase::replace(s, "%2", to_string(+uchars[1]));
    }
  }
  else if (expansion == PHRASE_HONORS)
  {
    if (numUchars == 0)
    {
      assert(Phrase::has(0, 0, 0));
    }
    else if (numUchars == 1)
    {
      assert(Phrase::has(0, 1, 0));
      Phrase::replace(s, "%0", dictionary.numerals.get(uchars[0]).text);
    }
    else
    {
      assert(Phrase::has(0, 2, 0));
      Phrase::replace(s, "%0", dictionary.ordinals.get(uchars[0]).text);
      Phrase::replace(s, "%1", dictionary.ordinals.get(uchars[1]).text);
    }
  }
  else if (expansion == PHRASE_COMPLETION_XES)
  {
    assert(Phrase::has(1, 0, 0));
    Phrase::replace(s, "%0", completion.strXes(side));
  }
  else
  {
    assert(false);
  }

  return s;
}
