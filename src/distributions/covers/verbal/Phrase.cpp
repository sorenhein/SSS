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
#include "../../../languages/PhraseExpansion.h"
#include "../../../languages/connections/words.h"

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
  &Phrase::strOrdinal,         // PHRASE_ORDINAL
  &Phrase::strLowestCard,      // PHRASE_LOWEST_CARD
  &Phrase::strIndefiniteRank,  // PHRASE_INDEFINITE_RANK
  &Phrase::strDefiniteRank,    // PHRASE_DEFINITE_RANK
  &Phrase::strOfDefiniteRank,  // PHRASE_OF_DEFINITE_RANK
  &Phrase::strCompletionSet,   // PHRASE_COMPLETION_SET
  &Phrase::strCompletionUnset, // PHRASE_COMPLETION_UNSET
  &Phrase::strXes              // PHRASE_XES
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


string Phrase::strLowestCard(
  const string& text,
  const RanksNames& ranksNames,
  [[maybe_unused]] const Completion& completion) const
{
  assert(Phrase::has(0, 1, 0));

  string s = text;
  Phrase::replace(s, "%0", ranksNames.lowestCard(uchars[0]));
  return s;
}


string Phrase::strIndefiniteRank(
  const string& text,
  const RanksNames& ranksNames,
  [[maybe_unused]] const Completion& completion) const
{
  assert(Phrase::has(0, 1, 0));

  string s = text;
  Phrase::replace(s, "%0", ranksNames.getOpponents(uchars[0]).
    strComponent(RANKNAME_ACTUAL_FULL_INDEF));
  return s;
}


string Phrase::strDefiniteRank(
  const string& text,
  const RanksNames& ranksNames,
  [[maybe_unused]] const Completion& completion) const
{
  assert(Phrase::has(0, 1, 0));

  string s = text;
  Phrase::replace(s, "%0", ranksNames.getOpponents(uchars[0]).
    strComponent(RANKNAME_ACTUAL_FULL_DEF));
  return s;
}


string Phrase::strOfDefiniteRank(
  const string& text,
  const RanksNames& ranksNames,
  [[maybe_unused]] const Completion& completion) const
{
  assert(Phrase::has(0, 1, 1));

  string s = text;
  Phrase::replace(s, "%0", ranksNames.strComponent(
    RANKNAME_ACTUAL_FULL_DEF_OF, uchars[0], bools[0]));
  return s;
}


string Phrase::strCompletionSet(
  const string& text,
  [[maybe_unused]] const RanksNames& ranksNames,
  const Completion& completion) const
{
  assert(Phrase::has(1, 0, 0) || Phrase::has(1, 0, 1));

  string s = text;
  if (numBools == 0)
    Phrase::replace(s, "%0", completion.strSet(ranksNames, side));
  else
    Phrase::replace(s, "%0", completion.strSet(ranksNames, side, bools[0]));
  return s;
}


string Phrase::strCompletionUnset(
  const string& text,
  [[maybe_unused]] const RanksNames& ranksNames,
  const Completion& completion) const
{
  assert(Phrase::has(1, 0, 0));

  string s = text;
  Phrase::replace(s, "%0", completion.strUnset(ranksNames, side));
  return s;
}


string Phrase::strXes(
  const string& text,
  [[maybe_unused]] const RanksNames& ranksNames,
  const Completion& completion) const
{
  assert(Phrase::has(1, 0, 0));

  string s = text;
  Phrase::replace(s, "%0", completion.strXes(side));
  return s;
}


string Phrase::str(
  const PhraseExpansion expansion,
  const string& text,
  const RanksNames& ranksNames,
  const Completion& completion) const
{
  assert(expansion < phraseMethods.size());

  return (this->*(phraseMethods[expansion]))
    (text, ranksNames, completion);
}
